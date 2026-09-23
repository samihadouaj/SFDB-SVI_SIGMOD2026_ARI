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

#ifndef _LDASVIPLANBUILDER_H_
#define _LDASVIPLANBUILDER_H_

#include "../utils/DebugHelper.h"
#include "../utils/TimeThis.h"
#include "../Atom.h"
#include "../Tau.h"
#include "../Internal_State.h"
#include "../VIDie.h"
#include "../utils/TimeThis.h"
#include "../SavingMalletState.h"
#include "../utils/argparse.hpp"
#include "../utils/DebugHelper.h"
#include <string>
#include <signal.h> 
#include "../ViUtils.h"
#include "omp.h"
#include "../Svi_Template.h"
#include "InMemorySVIPlanNode.h"

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
#include "../lda/LdaPlanBuilder.h"



template <std::size_t I, class T>
using tuple_element_t = typename std::tuple_element<I, T>::type;

std::string generateLDAPattern(int K, int vidie1_a, int vidie1_b, int vidie2_a, int vidie2_b)
{
   std::ostringstream oss;
   oss << "std::tuple< SAT<std::tuple< ";
   for (int i = 0; i < K; ++i)
   {
      if (i > 0)
      {
         oss << ", ";
      }
      oss << "Tau<ATOM3<0,0," << i << ",3>,ATOM2<1," << i << ",1,3>>";
   }
   oss << " >>, VIDice<VIDie<" << vidie1_a << "," << vidie1_b << ">,VIDie<"
       << vidie2_a << "," << vidie2_b << ">>, Svi_template<2> >";
   return oss.str();
}


// std::shared_ptr<arrow::dataset::Dataset> buildArrowDatasetFromURI(const std::string& inputCsvUri)
// {  
//    // ensure arrow::dataset node factories are in the registry
//    arrow::dataset::internal::Initialize();

//    std::string inputCsvPath;
//    std::shared_ptr<arrow::fs::FileSystem> inputFs = arrow::fs::FileSystemFromUri(inputCsvUri, &inputCsvPath).ValueOrDie();
   
//    arrow::dataset::FileSystemFactoryOptions fsFactoryOptions;
//    auto inputFormat = std::make_shared<arrow::dataset::CsvFileFormat>();
//    auto dataSetFactory = arrow::dataset::FileSystemDatasetFactory::Make(inputFs, {inputCsvPath}, inputFormat, fsFactoryOptions).ValueOrDie();
   
//    return dataSetFactory->Finish().ValueOrDie();
// }

// arrow::Status buildArrowScanNodeFromDataset(
//    arrow::compute::ExecNode* &target, 
//    std::shared_ptr<arrow::dataset::Dataset> dataset,
//    std::shared_ptr<arrow::compute::ExecPlan> plan)
// {
//    // ensure arrow::dataset node factories are in the registry
//    arrow::dataset::internal::Initialize();

//    auto scanOptions = std::make_shared<arrow::dataset::ScanOptions>();
//    scanOptions->projection = arrow::compute::project({}, {});  // create empty projection
//    auto scan_node_options = arrow::dataset::ScanNodeOptions{dataset, scanOptions};
//    ARROW_ASSIGN_OR_RAISE(target, ::arrow::compute::MakeExecNode("scan", plan.get(), {}, scan_node_options));
//    return arrow::Status::OK();
// }





// 
template <typename EXPRESSION>
[[clang::jit]]arrow::Status compileAndExecuteInMemorySviLDAPlan(
   std::string& inputCsvUri, 
    std::vector<int>& data, //this is not data for training, it's for eval later
    int nb_sat_assig_per_constr,
    int total_num_iter,
    float* priors,
    Vocabulary& malletVocab,
    const std::string& malletStateDir,
    const std::string& outfileId,
    int batchSize,   
    int vi_iter_per_batch,    
    int save_every,
   int num_threads,
   MappingReader& mapping)
   {   
      auto inner_start = std::chrono::high_resolution_clock::now();
// Preparing the main internal_state
    using SAT_T = tuple_element_t<0,EXPRESSION>;
    using DICE = tuple_element_t<1,EXPRESSION>;
    using SVI_TEMPLATE = tuple_element_t<2,EXPRESSION>;

   // DICE vidice;
   // Internal_State<DICE> is(total_unique_nb_sat_assig,nb_sat_assig_per_constr,&vidice);
   // SVI_TEMPLATE::initmuGamma(is);
   // SAT_T::setPriors(is,redPrior,bluePrior);


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

      std::shared_ptr<InMemorySVIPlanNode<SAT_T,DICE,SVI_TEMPLATE>> consumer = 
         std::make_shared<InMemorySVIPlanNode<SAT_T,DICE,SVI_TEMPLATE>>(finish,data,malletVocab,malletStateDir,nb_sat_assig_per_constr,outfileId,batchSize,vi_iter_per_batch,save_every,num_threads,total_num_iter,priors,mapping );

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

      std::cout << "In-memory SVI Plan Created : " << plan->ToString() << std::endl;

      // run the plan
       auto consume_start = std::chrono::high_resolution_clock::now();

      {
         TimeThis tt("In-memory LDA arrow plan execution");
         std::cout << "Executing plan..." << std::endl;
         ARROW_RETURN_NOT_OK(plan->StartProducing());
         
         {
            TimeThis tt2("scan node  time");
            ARROW_RETURN_NOT_OK(scanNode->finished().status());
            //ARROW_RETURN_NOT_OK(scanNode2->finished().status());
         }

    auto consume_end = std::chrono::high_resolution_clock::now();
    auto consume_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        consume_end - consume_start);
    
    std::cout << "[Profile] Approximate Consume Time (scan to consume completion): " 
              << consume_ms.count() << " ms" << std::endl;
         consumer->runSVI(total_num_iter);

         finish.MarkFinished(arrow::Status::OK());
         ARROW_RETURN_NOT_OK(plan->finished().status());
      }
      std::cout << "Finished!" << std::endl;
   }

    auto inner_end = std::chrono::high_resolution_clock::now();
    auto inner_ms = std::chrono::duration_cast<std::chrono::milliseconds>(inner_end - inner_start).count();
    std::cout << "[Profile] Function Execution Time (compileAndExecuteInMemorySviLDAPlan function): " << inner_ms << " ms" << std::endl;
   // return OK
   return arrow::Status::OK();

}


#endif // _LDASVIPLANBUILDER_H_