/*
   Copyright 2025 University of Michigan-Dearborn

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#ifndef _INMEMORYSVIPLANNODE_H_
#define _INMEMORYSVIPLANNODE_H_

#include <arrow/api.h>
#include <arrow/result.h> // arrow::Result
#include <arrow/status.h>

#include <arrow/io/api.h>

#include <arrow/compute/api_aggregate.h>
#include <arrow/compute/api.h> // arrow::AsyncGenerator
#include <arrow/compute/exec/exec_plan.h>

#include <arrow/dataset/dataset.h> // arrow::dataset::Dataset
#include <arrow/dataset/file_base.h>
#include <arrow/dataset/file_csv.h>      // arrow::dataset::CsvFileFormat
#include <arrow/dataset/file_ipc.h>      // arrow::dataset::IpcFileFormat
#include <arrow/dataset/file_orc.h>      // arrow::dataset::OrcFileFormat
#include <arrow/dataset/file_parquet.h>  // arrow::dataset::ParquetFileFormat
#include <arrow/dataset/discovery.h>     // arrow::dataset::FileSystemFactoryOptions
#include <arrow/filesystem/filesystem.h> // <arrow::fs::FileSystem
#include <arrow/filesystem/path_util.h>  // arrow::fs::internal::GetAbstractPathParent(path)
#include <arrow/dataset/plan.h>          // arrow::dataset::internal::Initialize

#include <arrow/csv/api.h>
#include <arrow/csv/writer.h>

#include <chrono>
#include <tuple>
#include <vector>
#include <fstream>
#include <mutex>

template <typename SAT_T, typename DICE, typename SVI_TEMPLATE>
class InMemorySVIPlanNode : public arrow::compute::SinkNodeConsumer
{
public:
   arrow::Future<> finish;
   std::vector<int> &data;            // Reference to evaluation data
                                      //  Internal_State<DICE>& is;                       // Main internal state
                                      //  std::vector<float>& all_rhots;                  // Precomputed rhot values
                                      //  Vocabulary& vocab;                              // Vocabulary reference
   Vocabulary &malletVocab;           // Mallet vocabulary
   const std::string &malletStateDir; // Mallet state directory
   int nb_sat_assig_per_constraint;   // SAT assignments per constraint
   const std::string &outfileId;      // Output file identifier
   int batchSize;                     // Processing batch size
   int vi_iter_per_batch;             // VI iterations per batch
   int save_every;                    // Save interval
   int num_threads;
   int total_num_iter; // Parallel threads
   float *priors;
   MappingReader &mapping;

   std::mutex _mutex;
   std::vector<int> _data; // training data
   long long _numRecords;

   InMemorySVIPlanNode(
       arrow::Future<> finish,
       std::vector<int> &data,
       //  Internal_State<DICE>& is,
       //  Vocabulary& vocab,
       Vocabulary &malletVocab,
       const std::string &malletStateDir,
       int nb_sat_assig_per_constraint,
       const std::string &outfileId,
       int batchSize,
       int vi_iter_per_batch,
       int save_every,
       int num_threads,
       int total_num_iter,
       float *priors,
       MappingReader &mapping)
       : finish(std::move(finish)),
         data(data),
         //  is(is),
         //  vocab(vocab),
         malletVocab(malletVocab),
         malletStateDir(malletStateDir),
         nb_sat_assig_per_constraint(nb_sat_assig_per_constraint),
         outfileId(outfileId),
         batchSize(batchSize),
         vi_iter_per_batch(vi_iter_per_batch),
         save_every(save_every),
         num_threads(num_threads),
         total_num_iter(total_num_iter),
         _data(), _numRecords(0), priors(priors), mapping(mapping)
   {
   }

   arrow::Status Init(
       const std::shared_ptr<arrow::Schema> &schema,
       arrow::compute::BackpressureControl *backpressure_control) override
   {
      SUPPRESS_UNUSED_PARAM_WARNING(schema);
      SUPPRESS_UNUSED_PARAM_WARNING(backpressure_control);
      return arrow::Status::OK();
   }

   arrow::Status Consume(arrow::compute::ExecBatch batch) override
   {
      // reading NYTIMES/CSV2 (1.9G)
      // dumb reader: Elapsed time = 3918[ms]
      // In-memory LDA arrow plan execution: Elapsed time = 10698[ms]
      const long long numElements = batch[0].array()->length;
      const int64_t *col0 = batch[0].array()->GetValues<int64_t>(1);
      const int64_t *col1 = batch[1].array()->GetValues<int64_t>(1);
      const int64_t *col2 = batch[2].array()->GetValues<int64_t>(1);
      // const int64_t* col3 = batch[3].array()->GetValues<int64_t>(1);

      {
         const std::lock_guard<std::mutex> lock(_mutex);

         for (long long i = 0; i < numElements; ++i)
         {
            _data.push_back(col0[i]);
            _data.push_back(col1[i]);
            _data.push_back(col2[i]);
            // _data.push_back(col3[i]);
         }
         _numRecords += numElements;
      } // lock release


      return arrow::Status::OK();
   }

   // Computes the internal_state is_batch of a given batch of constraints.
   void processBatch(
       Internal_State<DICE> &is_batch,
       long long startIdx,
       int current_batch_size)
   {
      std::cout << "Running processBatch" << std::endl;
      for (int it = 0; it < vi_iter_per_batch; it++)
      {
         // this does the necessary digamma computations
         SVI_TEMPLATE::prepare_dice(is_batch);

         // Compute the local parameters (lambdas)
         for (long long record = startIdx; record < startIdx + current_batch_size; record++)
         {
            // SVIExecutionContext context(&_data[record*4], &_data[0], 4);
            SAT_T::computeLocalParams(is_batch, &_data[record * 3], ((record - startIdx) * nb_sat_assig_per_constraint));
         }

         // normalize lambdas
         is_batch.normalizeLambdas();

         // you want the mus to be set to the priors so that you can compute the new ones
         SVI_TEMPLATE::setAllToZero(is_batch);

         // update global parameters (mu of each die)
         for (long long record = startIdx; record < startIdx + current_batch_size; record++)
         {
            // SVIExecutionContext context(&_data[record*4], &_data[0], 4);
            SAT_T::updateGlobalParams(is_batch, &_data[record * 3], ((record - startIdx) * nb_sat_assig_per_constraint));
         }

         // set lambdas to zero to compute the new ones in the next iteration
         is_batch.setLambdasToZero();
      }
   };

   void processBatchP(
       Internal_State<DICE> &is_batch,
       long long startIdx,
       int current_batch_size)
   {

      std::cout << "Running processBatchP" << std::endl;
      for (int it = 0; it < vi_iter_per_batch; it++)
      {
         // this does the necessary digamma computations

         SVI_TEMPLATE::prepare_diceP(is_batch, num_threads);

// Compute the local parameters (lambdas)
#pragma omp parallel for num_threads(num_threads)
         for (long long record = startIdx; record < startIdx + current_batch_size; record++)
         {
            SAT_T::computeLocalParams(is_batch, &_data[record * 3], ((record - startIdx) * nb_sat_assig_per_constraint));
         }

         // normalize lambdas
         is_batch.normalizeLambdasP(num_threads);

         // you want the mus to be set to the priors so that you can compute the new ones
         SVI_TEMPLATE::setAllToZeroP(is_batch, num_threads);

// update global parameters (mu of each die)
#pragma omp parallel for num_threads(num_threads)
         for (long long record = startIdx; record < startIdx + current_batch_size; record++)
         {
            SAT_T::updateGlobalParams(is_batch, &_data[record * 3], ((record - startIdx) * nb_sat_assig_per_constraint));
         }

         // set lambdas to zero to compute the new ones in the next iteration
         is_batch.setLambdasToZeroP(num_threads);
      }
   };

   void run_one_svi_iter(
       int batchId,
       int it,
       int num_batches,
       long long total_unique_nb_sat_assig,
       int current_batch_size,
       Internal_State<DICE> &is,
       std::vector<float> &all_rhots)
   {
      long long startIdx = batchId * batchSize;

      long long nb_uniq_sat_assign_in_batch = current_batch_size * nb_sat_assig_per_constraint;

      static DICE vidice_batch;

      Internal_State<DICE> is_batch(nb_uniq_sat_assign_in_batch, nb_sat_assig_per_constraint, &vidice_batch);
      SVI_TEMPLATE::setPriors(is_batch, priors);

      // Preparing the batch_internal_state by coping the current mu values of the dice of the main internal state
      SVI_TEMPLATE::copy_mus(is, is_batch);

      // is_batch.setVIDice(&vidice_batch);
      // is_batch.setLambdasToZero();

      // Training the batch_internal_state is_batch
      if (num_threads == 1)
      {
         processBatch(is_batch, startIdx, current_batch_size);
      }
      else
      {
         processBatchP(is_batch, startIdx, current_batch_size);
      }

      auto merge_start = std::chrono::high_resolution_clock::now();

      // Merge the internal_state is with the internal_state is_batch learned from this batch.
      SVI_TEMPLATE::merge_internal_states(is, is_batch, total_unique_nb_sat_assig, nb_uniq_sat_assign_in_batch, all_rhots[it], num_threads);

      auto merge_end = std::chrono::high_resolution_clock::now();
      long merge_timeMs = std::chrono::duration_cast<std::chrono::milliseconds>(merge_end - merge_start).count();
      std::cout << "Merge execution time: " << merge_timeMs << " ms" << std::endl;
   }

   void runSVI(int numIterations)
   {
      const std::lock_guard<std::mutex> lock(_mutex);

      // init
      {
         long long nb_unique_constraints = _numRecords;
         long long total_unique_nb_sat_assig = nb_unique_constraints * nb_sat_assig_per_constraint;
         std::cout << "_numRecords = " << _numRecords << std::endl;
         DICE vidice;
         Internal_State<DICE> is(total_unique_nb_sat_assig, nb_sat_assig_per_constraint, &vidice, 0);
         SVI_TEMPLATE::initmuGamma(is);
         SVI_TEMPLATE::setPriors(is, priors);

         int size_last_batch = nb_unique_constraints % batchSize;
         if (size_last_batch == 0)
         {
            size_last_batch = batchSize;
         }

         int num_batches = (nb_unique_constraints + batchSize - 1) / batchSize;
         std::vector<float> all_rhots = precompute_rhot(total_num_iter);

         for (int it = 0; it < total_num_iter; it++)
         {
            int batchId = (it % num_batches);
            int current_batch_size = (batchId == num_batches - 1) ? size_last_batch : batchSize;
            auto start = std::chrono::high_resolution_clock::now();

            run_one_svi_iter(batchId, it, num_batches, total_unique_nb_sat_assig, current_batch_size, is, all_rhots);

            auto end = std::chrono::high_resolution_clock::now();
            long timeMs = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

            std::cout << "#### " << it + 1 << "," << timeMs << std::endl;

            // ##### SAVING THE STATE AND DISPLAYING THE TOPICS
            if ((it + 1) % save_every == 0 && save_every != -1)
            {

               std::vector<int> tokenTopic;
               tokenTopic.reserve(data.size() / 3);
               // printImportantTopics(is,&malletVocab);
               // is.setLambdasToZero();

               // Compute the local parameters (lambdas)
               auto start_prepare_die = std::chrono::high_resolution_clock::now();

               SVI_TEMPLATE::prepare_dice(is);
               auto end_prepare_die = std::chrono::high_resolution_clock::now();
               long timeMsprepare = std::chrono::duration_cast<std::chrono::milliseconds>(end_prepare_die - start_prepare_die).count();

               std::cout << " prepapre die " << timeMsprepare << "ms " << std::endl;

               for (int batchId = 0; batchId < num_batches; batchId++)
               {
                  // int batchId = (tt % num_batches);
                  int current_batch_size = (batchId == num_batches - 1) ? size_last_batch : batchSize;
                  long long startIdx = batchId * batchSize;
                  long long nb_uniq_sat_assign_in_batch = current_batch_size * nb_sat_assig_per_constraint;
                  is.prepare_lambda_for_eval(nb_uniq_sat_assign_in_batch);

#pragma omp parallel for num_threads(24)
                  for (long long record = startIdx; record < startIdx + current_batch_size; record++)
                  {
                     SAT_T::computeLocalParams(is, &_data[record * 3], ((record - startIdx) * nb_sat_assig_per_constraint));
                  }

                  auto &lambda_eval = is.lambdas;
                  getTokenTopic(tokenTopic, lambda_eval, nb_sat_assig_per_constraint);
                  // std::cout << "size of tokenTopic in round: " << tokenTopic.size() << std::endl;
                  // std::cout<<"finished evaluation round "<<batchId<<"/"<<num_batches-1<<std::endl;
               }

               std::ofstream malletStateOFS;
               std::string snapshotFilePath = malletStateDir + "/chain-state_" + outfileId + ".txt." + std::to_string(it + 1);
               std::cout << "writing in file: " << snapshotFilePath << std::endl;
               malletStateOFS.open(snapshotFilePath);
               if (!malletStateOFS.is_open())
               {
                  std::cerr << "Failed to open file: " << snapshotFilePath << std::endl;
               }
               saveAsMalletBatchVI(malletStateOFS, nb_sat_assig_per_constraint, &malletVocab, data, tokenTopic, mapping);
               malletStateOFS.flush();
               malletStateOFS.close();
            }
         }
      }

   } // runGibbsSampler

   arrow::Future<> Finish() override
   {
      return finish;
   }

}; // class

#endif // _INMEMORYSVIPLANNODE_H_