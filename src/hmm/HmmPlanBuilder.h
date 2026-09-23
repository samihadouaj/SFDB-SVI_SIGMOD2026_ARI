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

#ifndef _HMMPLANBUILDER_H_
#define _HMMPLANBUILDER_H_

#include "InMemoryHmmPlanNode.h"

#include "../lda/LdaPlanBuilder.h"
#include "../utils/DebugHelper.h"
#include "../utils/TimeThis.h"
//#include "Vocabulary.h"
//#include "GibbsSinkNodeConsumer.h"
//#include "InMemoryGibbsPlanNode.h"
//#include "InMemoryGibbsPlanNodeP.h"


#include "../BDSet.h"
#include "../BDSetP.h"

#include "../BDExpr.h"
#include "../BDExpr2.h"

#include "../BDExpr3.h"

#include "../VrdExprLiterals.h"
#include "../VrdExprOperators.h"

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

#include <iostream>   // std::cout
#include <string>
#include <sstream>    // std::stringstream
#include <tuple>
#include <iostream>



static std::string buildHmmModelExpression_variadic()
{
   std::stringstream ss;
   ss << "std::tuple<" ;

   // HMM recursive expression
   ss << "VrdExpr::IAND<";
      // em[eobsid]=ecode
      ss << "VrdExpr::VAR3_DT<1,0,6,5>"; // VAR3<VGID=EDIE,VARID=0,OBSID_ARG=c#06,VALID_ARG=c#05>
      ss << ",";
      ss << "VrdExpr::EXOR<";
         
         ss << "VrdExpr::IAND<";
            ss << "VrdExpr::VAR3_DT<0,0,7,9>"; // VAR3<VGID=TDIE,VARID=0,OBSID_ARG=c#07,VALID_ARG=c#09>
            ss << ",";
            ss << "VrdExpr::REF_DT<10>";
         ss << ">"; // VrdExpr::IAND

         ss << ",";

         ss << "VrdExpr::IAND<";
            ss << "VrdExpr::VAR3_DT<0,0,7,12>"; // VAR3<VGID=TDIE,VARID=0,OBSID_ARG=c#07,VALID_ARG=c#12>
            ss << ",";
            ss << "VrdExpr::REF_DT<13>";
         ss << ">"; // VrdExpr::IAND
         
         ss << ",";

         ss << "VrdExpr::IAND<";
            ss << "VrdExpr::VAR3_DT<0,0,7,15>"; // VAR3<VGID=TDIE,VARID=0,OBSID_ARG=c#07,VALID_ARG=c#15>
            ss << ",";
            ss << "VrdExpr::REF_DT<16>";
         ss << ">"; // VrdExpr::IAND


      ss << ">"; // VrdExpr::EXOR
   ss << ">"; // VrdExpr::IAND

   ss << "," ;
   ss << "BDSets<";
      // one die for state transitions (NUMVARS=1, NUMOUTCOMES=3*3, NUMOBS=(seqLength-1)*numSeqs)
      ss << "BDSet<" << 1  << "," << 9 << "," << 9*1000 << ">";
      ss << ",";
      // one die for symbol emission (NUMVARS=1, NUMOUTCOMES=3*3, NUMOBS=seqLength*numSeqs)
      ss << "BDSet<" << 1  << "," << 9 << "," << 10*1000 << ">";
   ss << ">"; // BDSets
   ss << ">"; // std::tuple
   return ss.str();
}



template <typename SAMPLER_T>
[[clang::jit]] arrow::Status compileAndExecuteInMemoryHmmPlan(
   std::string& inputCsvUri, 
   std::string& outfileId,
   int numIterations,
   int rndSeedInit,
   int reportEvery)
{
   using EXPR_T = tuple_element_t<0, SAMPLER_T>;
   using MODEL_T = tuple_element_t<1, SAMPLER_T>;

   MODEL_T model;
   std::get<0>(model).setSymmetricDirichletHyperParam(10.0); // alpha
   std::get<1>(model).setSymmetricDirichletHyperParam(1.0); // beta

   std::shared_ptr<arrow::dataset::Dataset> dataset = buildArrowDatasetFromURI(inputCsvUri);

   // build and execute the execution plan
   {
      arrow::compute::ExecContext exec_context;
      std::shared_ptr<arrow::compute::ExecPlan> plan;
      ARROW_ASSIGN_OR_RAISE(plan, arrow::compute::ExecPlan::Make(&exec_context));

      // create the scanNode
      arrow::compute::ExecNode* scanNode; 
      ARROW_RETURN_NOT_OK(buildArrowScanNodeFromDataset(scanNode, dataset, plan));

      // create sink node
      arrow::Future<> finish = arrow::Future<>::Make();

      
      std::shared_ptr<InMemoryHmmPlanNode<EXPR_T, MODEL_T>> consumer = 
         std::make_shared<InMemoryHmmPlanNode<EXPR_T, MODEL_T>>(finish, &model, outfileId, reportEvery);

      arrow::compute::ExecNode* consuming_sink;
      ARROW_ASSIGN_OR_RAISE(consuming_sink, 
         MakeExecNode("consuming_sink", 
            plan.get(), 
            {scanNode}, // {union_node}, 
            arrow::compute::ConsumingSinkNodeOptions(consumer)
         )
      );

      // validate the plan
      ARROW_RETURN_NOT_OK(scanNode->Validate());
      //ARROW_RETURN_NOT_OK(scanNode2->Validate());
      ARROW_RETURN_NOT_OK(consuming_sink->Validate());
      ARROW_RETURN_NOT_OK(plan->Validate());

      std::cout << "In-memory Hmm Plan Created : " << plan->ToString() << std::endl;

      //consumer->saveAsNumpyArray(std::cout);
      // run the plan
      {
         TimeThis tt("In-memory HMM arrow plan execution");
         std::cout << "Executing plan..." << std::endl;
         ARROW_RETURN_NOT_OK(plan->StartProducing());
         
         ARROW_RETURN_NOT_OK(scanNode->finished().status());
         //ARROW_RETURN_NOT_OK(scanNode2->finished().status());

         uint64_t rndSeed = rndSeedInit;
         consumer->runGibbsSampler(numIterations, rndSeed);

         finish.MarkFinished(arrow::Status::OK());
         ARROW_RETURN_NOT_OK(plan->finished().status());

         //consumer->saveAsNumpyArray(std::cout);

         // save tdie params
         std::string tdieparams_filepath = outfileId+"tdie.csv";
         std::ofstream tdieOFS;
         tdieOFS.open(tdieparams_filepath);
         consumer->saveTdieParams(tdieOFS);
         tdieOFS.close();

         // save edie params
         std::string edieparams_filepath = outfileId+"edie.csv";
         std::ofstream edieOFS;
         edieOFS.open(edieparams_filepath);
         consumer->saveEdieParams(edieOFS);
         edieOFS.close();
      }
      std::cout << "Finished!" << std::endl;
   }

   // return OK
   return arrow::Status::OK();

}



#endif // _HMMPLANBUILDER_H_