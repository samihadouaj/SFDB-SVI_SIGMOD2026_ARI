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

#ifndef _INMEMORYHMMPLANNODE_H_
#define _INMEMORYHMMPLANNODE_H_

#include "../utils/DebugHelper.h"

#include "../BDExpr.h"
#include "../BDExpr2.h"
#include "../BDSet.h"

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



template <typename EXPR_T, typename MODEL_T>
class InMemoryHmmPlanNode : public arrow::compute::SinkNodeConsumer 
{

protected:
   arrow::Future<> finish;
   MODEL_T* _model;
   std::string _outfileId;
   int _reportEvery;

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

   
   InMemoryHmmPlanNode(arrow::Future<>finish, MODEL_T* model, std::string & outfileId, int reportEvery): 
      finish(std::move(finish)), _model(model),
      _outfileId(outfileId),
      _reportEvery(reportEvery),
      _data(), _probs(), _retracted(), _numRecords(0)
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
      const int numElements = batch[0].array()->length;
      const int64_t* col0 = batch[0].array()->GetValues<int64_t>(1);
      const int64_t* col1 = batch[1].array()->GetValues<int64_t>(1);
      const int64_t* col2 = batch[2].array()->GetValues<int64_t>(1);
      const int64_t* col3 = batch[3].array()->GetValues<int64_t>(1);
      const int64_t* col4 = batch[4].array()->GetValues<int64_t>(1);
      const int64_t* col5 = batch[5].array()->GetValues<int64_t>(1);
      const int64_t* col6 = batch[6].array()->GetValues<int64_t>(1);
      const int64_t* col7 = batch[7].array()->GetValues<int64_t>(1);
      const int64_t* col8 = batch[8].array()->GetValues<int64_t>(1);
      const int64_t* col9 = batch[9].array()->GetValues<int64_t>(1);
      const int64_t* col10 = batch[10].array()->GetValues<int64_t>(1);
      const int64_t* col11 = batch[11].array()->GetValues<int64_t>(1);
      const int64_t* col12 = batch[12].array()->GetValues<int64_t>(1);
      const int64_t* col13 = batch[13].array()->GetValues<int64_t>(1);
      const int64_t* col14 = batch[14].array()->GetValues<int64_t>(1);
      const int64_t* col15 = batch[15].array()->GetValues<int64_t>(1);
      const int64_t* col16 = batch[16].array()->GetValues<int64_t>(1);

      {
         const std::lock_guard<std::mutex> lock(_mutex);
         
         for (int i=0; i<numElements; ++i)
         {
            /*
            std::cout << col0[i] << ",";
            std::cout << col1[i] << ",";
            std::cout << col2[i] << ",";
            std::cout << col3[i] << ",";
            std::cout << col4[i] << ",";
            std::cout << col5[i] << ",";
            std::cout << col6[i] << ",";
            std::cout << col7[i] << ",";
            std::cout << col8[i] << ",";
            std::cout << col9[i] << ",";
            std::cout << col10[i] << ",";
            std::cout << col11[i] << ",";
            std::cout << col12[i] << ",";
            std::cout << col13[i] << ",";
            std::cout << col14[i] << ",";
            std::cout << col15[i] << ",";
            std::cout << col16[i] << std::endl;
            */
            _data.push_back(col0[i]);
            _data.push_back(col1[i]);
            _data.push_back(col2[i]);
            _data.push_back(col3[i]);
            _data.push_back(col4[i]);
            _data.push_back(col5[i]);
            _data.push_back(col6[i]);
            _data.push_back(col7[i]);
            _data.push_back(col8[i]);
            _data.push_back(col9[i]);
            _data.push_back(col10[i]);
            _data.push_back(col11[i]);
            _data.push_back(col12[i]);
            _data.push_back(col13[i]);
            _data.push_back(col14[i]);
            _data.push_back(col15[i]);
            _data.push_back(col16[i]);
            _probs.push_back(-1.0);
            _retracted.push_back(1);
         }
         _numRecords += numElements;
      } // lock release

      return arrow::Status::OK();
   }

   void saveTdieParams(std::ostream &os)
   {
      for (int hsA=0; hsA<3; hsA++)
      {
         for (int hsB=0; hsB<3; hsB++)
         {
            int tcode = (3*hsA)+hsB;
            int varId = 0;
            if (hsB>0)
            {
               os << ",";
            }
            os << std::get<0>(*_model).countObservations(varId, tcode)+std::get<0>(*_model).getDirichletHyperParam(varId, tcode);
         }
         if (hsA<2)
         {
            os << std::endl;
         }
      }
   }

   void saveEdieParams(std::ostream &os)
   {
      for (int hs=0; hs<3; hs++)
      {
         for (int symb=0; symb<3; symb++)
         {
            int ecode = (3*hs)+symb;
            int varId = 0;

            if (symb>0)
            {
               os << ",";
            }
            os << std::get<1>(*_model).countObservations(varId, ecode)+std::get<1>(*_model).getDirichletHyperParam(varId, ecode);
         }
         if (hs<2)
         {
            os << std::endl;
         }
      }
   }

   void saveAsNumpyArray(std::ostream &os) // std::string datasetName
   {
      //using TDIE_T = tuple_element_t<0, MODEL_T>;
      //using EDIE_T = tuple_element_t<1, MODEL_T>;

      os << "TDIE" << std::endl;

      for (int hsA=0; hsA<3; hsA++)
      {
         for (int hsB=0; hsB<3; hsB++)
         {
            int tcode = (3*hsA)+hsB;
            int varId = 0;
            os << "s" << hsA << " --> s" << hsB << " : " << std::get<0>(*_model).getPosteriorPredictive(varId, tcode) << " " << std::get<0>(*_model).countObservations(varId, tcode)+std::get<0>(*_model).getDirichletHyperParam(varId, tcode) << std::endl;
         }
      }

      os << std::endl;
      os << "EDIE" << std::endl;
      for (int hs=0; hs<3; hs++)
      {
         for (int symb=0; symb<3; symb++)
         {
            int ecode = (3*hs)+symb;
            int varId = 0;
            os << "s" << hs << " --> e" << symb << " : " << std::get<1>(*_model).getPosteriorPredictive(varId, ecode) << " " << std::get<1>(*_model).countObservations(varId, ecode)+std::get<1>(*_model).getDirichletHyperParam(varId, ecode) << std::endl;
         }
      }

      os << std::endl;
      
   }




   void runGibbsSampler(int numIterations, uint64_t rndSeed)
   {
      const std::lock_guard<std::mutex> lock(_mutex);
      uint64_t* rndSeedPtr = &rndSeed;
      const int blockLength = 31;
      const int recordLength = 17;

      // init
      {
         //TimeThis tt("In-memory HMM init ");
         {
            //std::chrono::steady_clock::time_point _start = std::chrono::steady_clock::now();
            //int *pos = &_data[0];

            //for (int i=0; i<blockLength; i++)
            //{
            //   SamplerExecutionContext<EXPR_T> context(&_data[i*recordLength], &_data[0], recordLength, &_probs[i], &_probs[0], 1, &_retracted[i], &_retracted[0], 1, rndSeedPtr);
            //   EXPR_T::toString(std::cout, *_model, context);
            //   std::cout << "\n";
            //}

            for (int i=(blockLength-1); i<_numRecords; i+=blockLength)
            {
               //for (int c=0; c<17; c++)
               //{
               //   std::cout << _data[(i*recordLength)+c] << ",";
               //}
               //std::cout << std::endl;
               SamplerExecutionContext<EXPR_T> context(&_data[i*recordLength], &_data[0], recordLength, &_probs[i], &_probs[0], 1, &_retracted[i], &_retracted[0], 1, rndSeedPtr);
               EXPR_T::sampleSat(*_model, context);
            }
            //std::chrono::steady_clock::time_point _end = std::chrono::steady_clock::now();
            //long timeMs = std::chrono::duration_cast<std::chrono::milliseconds> (_end - _start).count();
            //std::cout << "#### " << 1 << "," << timeMs << std::endl;
         }
      }
      
      // execution
      for (int j=2; j<=numIterations; j++)
      {
         {
            //TimeThis tt("In-memory HMM iteration "+std::to_string(j));
            {
               //std::chrono::steady_clock::time_point _start = std::chrono::steady_clock::now();
               //int *pos = &_data[0];
               for (int i=(blockLength-1); i<_numRecords; i+=blockLength)
               {
                  SamplerExecutionContext<EXPR_T> context(&_data[i*recordLength], &_data[0], recordLength, &_probs[i], &_probs[0], 1, &_retracted[i], &_retracted[0], 1, rndSeedPtr);
                  for (int ip=0; ip<_probs.size(); ip++ ) {
                     _probs[ip] = -1.0;
                  }
                  EXPR_T::retract(*_model, context);
                  EXPR_T::sampleSat(*_model, context);
                  //pos += 3;
               }
               //std::chrono::steady_clock::time_point _end = std::chrono::steady_clock::now();
               //long timeMs = std::chrono::duration_cast<std::chrono::milliseconds> (_end - _start).count();
               //std::cout << "#### " << j << "," << timeMs << std::endl;
            }
         }
         /*
         if ( (_reportEvery>=0) && ((j % _reportEvery) == 0))
         {
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
         }
         */
         //saveAsMalletState(std::cout);
         //saveAsNumpyArray(std::cout);

      } // next iteration

   } // runGibbsSampler

   arrow::Future<> Finish() override 
   { 
      return finish; 
   }

}; // class InMemoryHmmPlanNode












#endif // _INMEMORYHMMPLANNODE_H_


