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

#ifndef _GIBBSSINKNODECONSUMER_H_
#define _GIBBSSINKNODECONSUMER_H_

#include "../utils/DebugHelper.h"

#include "../BDExpr.h"
#include "../BDExpr2.h"
#include "../BDSet.h"
#include "../SamplerExecutionContext.h"

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

#include <tuple>



template <typename EXPR_T, typename MODEL_T>
class GibbsInitSinkNodeConsumer : public arrow::compute::SinkNodeConsumer 
{

protected:
   arrow::Future<> finish;
   MODEL_T* _model;

public:
   
   GibbsInitSinkNodeConsumer(arrow::Future<>finish, MODEL_T* model): finish(std::move(finish)),_model(model)
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

   inline
   virtual void performMcmcUpdate(SamplerExecutionContext<EXPR_T> &context)
   {
      EXPR_T::sampleSat(*_model, context);
   }

   arrow::Status Consume(arrow::compute::ExecBatch batch) override 
   {
      // reading NYTIMES/CSV2 (1.9G)
      // dumb reader: Elapsed time = 3918[ms]
      // arrow/csv:   Elapsed time = 9728[ms]

      //SUPPRESS_UNUSED_PARAM_WARNING(batch);
      
      //std::cout << "batch! " << batch.ToString() << std::endl;
      //std::cout << "batch! "  << std::endl;
      
      const int numElements = batch[0].array()->length;
      const int64_t* col0 = batch[0].array()->GetValues<int64_t>(1);
      const int64_t* col1 = batch[1].array()->GetValues<int64_t>(1);
      const int64_t* col2 = batch[2].array()->GetValues<int64_t>(1);

      int tmpArr[] = {0, 0, 0};
      //int *pos = &tmpArr[0];
      uint64_t rndSeed = 2023;
      double probs[] = {0.0, 0.0, 0.0};
      int _retracted[] = { 1, 1, 1 };
      SamplerExecutionContext<EXPR_T> context(&tmpArr[0], &tmpArr[0], 3, &probs[0], &probs[0], 1, &_retracted[0], &_retracted[0], 1, &rndSeed);

      for (int i=0; i<numElements; ++i)
      {
         tmpArr[0] = col0[i];
         tmpArr[1] = col1[i];
         tmpArr[2] = col2[i];
         //EXPR_T::sampleSat(*_model, pos, &rndSeed);
         performMcmcUpdate(context);
      }
      
      return arrow::Status::OK();
   }

   arrow::Future<> Finish() override 
   { 
      return finish; 
   }

}; // class GibbsInitSinkNodeConsumer

template <typename EXPR_T, typename MODEL_T>
class GibbsSweepSinkNodeConsumer : public GibbsInitSinkNodeConsumer<EXPR_T,MODEL_T>
{
public:

   GibbsSweepSinkNodeConsumer(arrow::Future<>finish, MODEL_T* model): GibbsInitSinkNodeConsumer<EXPR_T,MODEL_T>(finish, model)
   {}

   inline
   virtual void performMcmcUpdate(SamplerExecutionContext<EXPR_T> &context) override
   {
      EXPR_T::retract(*GibbsInitSinkNodeConsumer<EXPR_T,MODEL_T>::_model, context);
      EXPR_T::sampleSat(*GibbsInitSinkNodeConsumer<EXPR_T,MODEL_T>::_model, context);
   }
}; // GibbsSweepSinkNodeConsumer

#endif // _LDAPLANBUILDER_H_


