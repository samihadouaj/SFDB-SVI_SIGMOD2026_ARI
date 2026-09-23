/*
   Copyright 2023 University of Michigan-Dearborn

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

#ifndef _INMEMORYGIBBSPLANNODEP_H_
#define _INMEMORYGIBBSPLANNODEP_H_

#include "../utils/DebugHelper.h"

#include "../BDExpr.h"
#include "../BDExpr2.h"
// #include "../BDSet.h"
#include "../BDSetP.h"
#include "../SamplerExecutionContext.h"


#include "Vocabulary.h"

#include <arrow/api.h>
#include <arrow/result.h>                // arrow::Result
#include <arrow/status.h>

#include <arrow/io/api.h>

#include <arrow/compute/api_aggregate.h>
#include <arrow/compute/api.h>           // arrow::AsyncGenerator
#include <arrow/compute/exec/exec_plan.h>

#include <arrow/dataset/dataset.h>       // arrow::dataset::Dataset
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
#include "omp.h"

template <typename EXPR_T, typename MODEL_T>
class InMemoryGibbsPlanNodeP : public arrow::compute::SinkNodeConsumer 
{

protected:
   arrow::Future<> finish;
   MODEL_T* _model;
   Vocabulary* _vocab;
   std::string _malletStateDir;
   std::string _outfileId;
   int _reportEvery;
   int _num_threads;
   int _startSavingFromIteration;

private:

   std::mutex _mutex;
   std::vector<int> _data;
   std::vector<double> _probs;
   std::vector<int> _retracted;
   int _numRecords;

   template <std::size_t I, class T>
   using tuple_element_t = typename std::tuple_element<I, T>::type;

public:


   //std::vector<int> _col0;
   //std::vector<int> _col1;
   //std::vector<int> _col2;

   
   InMemoryGibbsPlanNodeP(arrow::Future<>finish, MODEL_T* model, Vocabulary* vocab, std::string &lda_malletStateDir, std::string & lda_outfileId, int reportEvery,int num_threads, int startSavingFromIteration ): 
      finish(std::move(finish)), _model(model), _vocab(vocab),
      _malletStateDir(lda_malletStateDir), _outfileId(lda_outfileId),
      _reportEvery(reportEvery),
      _data(), _probs(), _retracted(), _numRecords(0),_num_threads(num_threads),
      _startSavingFromIteration(startSavingFromIteration)
      //_col0(1000),_col1(1000),_col2(1000)
   {}

   arrow::Status Init(
      const std::shared_ptr<arrow::Schema>& schema, 
      arrow::compute::BackpressureControl* backpressure_control
      ) override
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
      
      const int numElements = batch[0].array()->length;
      const int64_t* col0 = batch[0].array()->GetValues<int64_t>(1);
      const int64_t* col1 = batch[1].array()->GetValues<int64_t>(1);
      const int64_t* col2 = batch[2].array()->GetValues<int64_t>(1);

      {
         const std::lock_guard<std::mutex> lock(_mutex);
         
         for (int i=0; i<numElements; ++i)
         {
            _data.push_back(col0[i]);
            _data.push_back(col1[i]);
            _data.push_back(col2[i]);
            _probs.push_back(-1.0);
            _retracted.push_back(1);
         }
         _numRecords += numElements;
      } // lock release

      return arrow::Status::OK();
   }

   
   //template <typename MODEL_T>
   void saveAsMalletState(std::ostream &os) // std::string datasetName
   {
      //using RED_BDSET_T = tuple_element_t<0, MODEL_T>;
      using BLUE_BDSET_T = tuple_element_t<1, MODEL_T>;

      os << "#doc source pos typeindex type topic" << std::endl;
      os << "#alpha : ";
      std::stringstream ss;

      int numTopics = BLUE_BDSET_T::getNumVars();
      //int numDocs = RED_BDSET_T::getNumVars();

      for (int topicId=0; topicId<numTopics; topicId++)
      {
         os << std::get<0>(*_model).getDirichletHyperParam(0, topicId) << " ";
      }
      os << std::endl;

      os << "#beta : ";
      os << std::get<1>(*_model).getDirichletHyperParam(0, 0) << std::endl;

      int *pos = &_data[0];
      int docId = pos[0];
      int docOffset = 0;
      for (int i=0; i<_numRecords; ++i)
      {
         if (docId!=pos[0]) {
            docOffset = 0;
         }
         docId = pos[0];
         int wordId = pos[2];
         int malletDocId = docId+1;
         int topicId = std::get<0>(*_model).getObservedValue(docId, i);

         // os << malletDocId << " NA " << docOffset << " " << _vocab->getWordByCode(wordId) << " " << topicId << std::endl;
         ss << malletDocId << " NA " << docOffset << " " << _vocab->getWordByCode(wordId) << " " << topicId << std::endl;
         

         docOffset+=1;
         pos += 3;
      }
      os << ss.str();

      /*
      for (int i=0; i<getNumberOfTopics(); i++)
      {
         os << getAlpha() << " ";
      }
      os << std::endl;
      os << "#beta : " << getBeta() << std::endl;
      for (int docId=0; docId<_corpus.getNumberOfTrainingDocuments(); docId++)
      {
         std::string docFilePath = "../"+datasetName+"/train/"+"doc"+intToStrWithLeadingZeroes(docId, 12)+".txt";
         for (int pos=0; pos<_corpus.getDocSize(docId); pos++)
         {
            int malletWordCode = _corpus.getMalletWordCode(docId, pos);
            std::string wordStr = _corpus.getWord(docId, pos);
            int topicId = getTopic(docId, pos);
            os << docId << " " << docFilePath << " " << pos << " " << malletWordCode << " " << wordStr << " " << topicId << std::endl;
         } // next token
      } // next training DocId
      */
      
   }
   

// void runMOK(int numIterations, uint64_t rndSeed)
// {
//  #pragma omp parallel for num_threads(4)
//     for (int i = 1; i <= 10; i++) {
//         int tid = omp_get_thread_num();
//         printf("The thread %d  executes i = %d\n", tid, i);
//     }

// }

   void runGibbsSampler(int numIterations, uint64_t rndSeed)
   {
      const std::lock_guard<std::mutex> lock(_mutex);
      uint64_t* rndSeedPtr = &rndSeed;
         // std::chrono::steady_clock::time_point bideya = std::chrono::steady_clock::now();

      // init
      {
         TimeThis tt("In-memory LDA init ");
         {
            std::chrono::steady_clock::time_point _start = std::chrono::steady_clock::now();
            // int *pos = &_data[0];
            std::cout<<"The init phase is being executed in parallel now. Num Threads is "<<_num_threads<<std::endl;
            #pragma omp parallel for num_threads(_num_threads)
            for (int i=0; i<_numRecords; ++i)
            {
               SamplerExecutionContext<EXPR_T> context(&_data[i*3], &_data[0], 3, &_probs[i], &_probs[0], 1, &_retracted[i], &_retracted[0], 1, rndSeedPtr);
               EXPR_T::sampleSat(*_model,  context);
               // pos += 3;
            }
            std::chrono::steady_clock::time_point _end = std::chrono::steady_clock::now();
               long timeMs = std::chrono::duration_cast<std::chrono::milliseconds> (_end - _start).count();
               std::cout << "#### " << 1 << "," << timeMs << std::endl;
         }
      }
      // execution
      // int chunkSize = _numRecords/4;
      // std::cout<<"chunkSize is: "<<chunkSize<<std::endl;
      std::cout<<"The number of threads is: "<<_num_threads<<std::endl;
      for (int j=2; j<=numIterations; j++)
      {
         {
            TimeThis tt("In-memory LDA iteration "+std::to_string(j));
            {
               std::chrono::steady_clock::time_point _start = std::chrono::steady_clock::now();
               // int *pos = &_data[0];
               #pragma omp parallel for num_threads(_num_threads)
               for (int i=0; i<_numRecords; ++i)
               {  
                  // int tid = omp_get_thread_num();
                  // printf("The thread %d  executes query-answer = %d\n", tid, i);
                  // std::cout<<"thread "<<tid<<" executes query-answer "<<i<<std::endl;
                  SamplerExecutionContext<EXPR_T> context(&_data[i*3], &_data[0], 3, &_probs[i], &_probs[0], 1, &_retracted[i], &_retracted[0], 1, rndSeedPtr);
                  EXPR_T::retract(*_model, context);
                  EXPR_T::sampleSat(*_model, context);
                  // #pragma omp critical
                  // {std::cout<<"thread nb "<<tid<<" iter nb "<<i<<" :"<<pos[0]<<" "<<pos[1]<<" "<<pos[2]<<std::endl; pos+=3;}
               }
               std::chrono::steady_clock::time_point _end = std::chrono::steady_clock::now();
               long timeMs = std::chrono::duration_cast<std::chrono::milliseconds> (_end - _start).count();
               std::cout << "#### " << j << "," << timeMs << std::endl;
            }
         } 
         if ( (_reportEvery>=0) && ((j % _reportEvery) == 0) && (j>_startSavingFromIteration))
         {
            auto start = std::chrono::high_resolution_clock::now();

            std::ofstream malletStateOFS;
            std::string snapshotFilePath = _malletStateDir+"/chain-state_"+_outfileId;
            malletStateOFS.open(snapshotFilePath);
            saveAsMalletState(malletStateOFS);
            malletStateOFS.close();

            // std::string gzipSnapshotCmd("gzip "+snapshotFilePath);
            // system(&*gzipSnapshotCmd.begin());
            // std::string renameSnapshotCmd("mv "+snapshotFilePath+".gz "+snapshotFilePath+".gz."+std::to_string(j));
            std::string renameSnapshotCmd("mv "+snapshotFilePath+" "+snapshotFilePath+".txt."+std::to_string(j));
            system(&*renameSnapshotCmd.begin());
         
            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> diff = end-start;
            std::cout << "Time to save mallet states: " << diff.count() << " s\n";
         }
 

         //saveAsMalletState(std::cout);

      } // next iteration
         //   std::chrono::steady_clock::time_point niheya = std::chrono::steady_clock::now();
         // long timeMs2 = std::chrono::duration_cast<std::chrono::milliseconds> (niheya - bideya).count();
         // std::cout << "#### FINAL," << timeMs2 << std::endl;
   } // runGibbsSampler




   arrow::Future<> Finish() override 
   { 
      return finish; 
   }

}; // class InMemoryGibbsPlanNode



#endif // _INMEMORYGIBBSPLANNODEP_H_