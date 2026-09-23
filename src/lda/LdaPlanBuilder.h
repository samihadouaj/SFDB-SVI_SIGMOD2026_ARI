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

#ifndef _LDAPLANBUILDER_H_
#define _LDAPLANBUILDER_H_

#include "../utils/DebugHelper.h"
#include "../utils/TimeThis.h"
#include "Vocabulary.h"
#include "GibbsSinkNodeConsumer.h"
#include "InMemoryGibbsPlanNode.h"
#include "InMemoryGibbsPlanNodeP.h"


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


//static std::string buildLDAmodelExpression(int numTopics, int numDocuments, int vocabSize, int corpusSize);

template <std::size_t I, class T>
using tuple_element_t = typename std::tuple_element<I, T>::type;

std::shared_ptr<arrow::dataset::Dataset> buildArrowDatasetFromURI(const std::string& inputCsvUri)
{  
   // ensure arrow::dataset node factories are in the registry
   arrow::dataset::internal::Initialize();

   std::string inputCsvPath;
   std::shared_ptr<arrow::fs::FileSystem> inputFs = arrow::fs::FileSystemFromUri(inputCsvUri, &inputCsvPath).ValueOrDie();
   
   arrow::dataset::FileSystemFactoryOptions fsFactoryOptions;
   auto inputFormat = std::make_shared<arrow::dataset::CsvFileFormat>();
   auto dataSetFactory = arrow::dataset::FileSystemDatasetFactory::Make(inputFs, {inputCsvPath}, inputFormat, fsFactoryOptions).ValueOrDie();
   
   return dataSetFactory->Finish().ValueOrDie();
}

arrow::Status buildArrowScanNodeFromDataset(
   arrow::compute::ExecNode* &target, 
   std::shared_ptr<arrow::dataset::Dataset> dataset,
   std::shared_ptr<arrow::compute::ExecPlan> plan)
{
   // ensure arrow::dataset node factories are in the registry
   arrow::dataset::internal::Initialize();

   auto scanOptions = std::make_shared<arrow::dataset::ScanOptions>();
   scanOptions->projection = arrow::compute::project({}, {});  // create empty projection
   auto scan_node_options = arrow::dataset::ScanNodeOptions{dataset, scanOptions};
   ARROW_ASSIGN_OR_RAISE(target, ::arrow::compute::MakeExecNode("scan", plan.get(), {}, scan_node_options));
   return arrow::Status::OK();
}





static std::string buildLDAmodelExpression_pcexpr(int numTopics, int numDocuments, int vocabSize, int corpusSize)
{

   std::stringstream ss;
   ss << "std::tuple<" ;
   ss << "EXOR" << numTopics << "<";
      ss << "IAND2<";
         ss << "VAR2<0,0,1,0>"; // VAR2<VGID,VARID_ARG,OBSID_ARG,VALID>  docid_red_dice[pos]=topicid   C, V, V, C
         ss << ","; 
         ss << "VAR3<1,0,1,2>"; // VAR3<VGID,VARID,OBSID_ARG,VALID_ARG>   topicid_blue_dice[pos]=wordid C, C, V, V
      ss << ">"; // IAND
      for (int i=1; i<numTopics; i++)
      {
         ss << ",";
         ss << "IAND2<";
            ss << "VAR2<0,0,1," << i << ">"; // VAR2<VGID,VARID_ARG,OBSID_ARG,VALID>  docid_red_dice[pos]=topicid   C, V, V, C
            ss << ","; 
            ss << "VAR3<1," << i << ",1,2>"; // VAR3<VGID,VARID,OBSID_ARG,VALID_ARG>   topicid_blue_dice[pos]=wordid C, C, V, V
         ss << ">"; // IAND
      }
   ss << ">"; // EXOR
   ss << ",";
   ss << "BDSets<";

      // red dices  (NUMVARS=numDocs, NUMOUTCOMES=numTopics, NUMOBS=corpusSize)
      ss << "BDSet<" << numDocuments << "," << numTopics << "," << corpusSize << ">";

      ss << ",";

      // blue dices (NUMVARS=numTopics, NUMOUTCOMES=vocabSize, NUMOBS=corpusSize)
      ss << "BDSet<" << numTopics << "," << vocabSize << "," << corpusSize << ">";
   
   ss << ">"; // BDSets
   ss << ">"; // std::tuple
   return ss.str();
};



static std::string buildLDAmodelExpression_variadicP(int numTopics, int numDocuments, int vocabSize, int corpusSize)
{

   std::stringstream ss;
   ss << "std::tuple<" ;
   ss << "VrdExpr::EXOR" << "<";
      ss << "VrdExpr::IAND<";
         ss << "VrdExpr::VAR2<0,0,1,0>"; // VAR2<VGID,VARID_ARG,OBSID_ARG,VALID>  docid_red_dice[pos]=topicid   C, V, V, C
         ss << ","; 
         ss << "VrdExpr::VAR3<1,0,1,2>"; // VAR3<VGID,VARID,OBSID_ARG,VALID_ARG>   topicid_blue_dice[pos]=wordid C, C, V, V
      ss << ">"; // IAND
      for (int i=1; i<numTopics; i++)
      {
         ss << ",";
         ss << "VrdExpr::IAND<";
            ss << "VrdExpr::VAR2<0,0,1," << i << ">"; // VAR2<VGID,VARID_ARG,OBSID_ARG,VALID>  docid_red_dice[pos]=topicid   C, V, V, C
            ss << ","; 
            ss << "VrdExpr::VAR3<1," << i << ",1,2>"; // VAR3<VGID,VARID,OBSID_ARG,VALID_ARG>   topicid_blue_dice[pos]=wordid C, C, V, V
         ss << ">"; // IAND
      }
   ss << ">"; // EXOR
   ss << ",";
   ss << "BDSetsP<";

      // red dices  (NUMVARS=numDocs, NUMOUTCOMES=numTopics, NUMOBS=corpusSize)
      ss << "BDSetP<" << numDocuments << "," << numTopics << "," << corpusSize << ">";

      ss << ",";

      // blue dices (NUMVARS=numTopics, NUMOUTCOMES=vocabSize, NUMOBS=corpusSize)
      ss << "BDSetP<" << numTopics << "," << vocabSize << "," << corpusSize << ">";
   
   ss << ">"; // BDSets
   ss << ">"; // std::tuple
   return ss.str();
};

static std::string buildLDAmodelExpression_variadic(int numTopics, int numDocuments, int vocabSize, int corpusSize)
{

   std::stringstream ss;
   ss << "std::tuple<" ;
   ss << "VrdExpr::EXOR" << "<";
      ss << "VrdExpr::IAND<";
         ss << "VrdExpr::VAR2<0,0,1,0>"; // VAR2<VGID,VARID_ARG,OBSID_ARG,VALID>  docid_red_dice[pos]=topicid   C, V, V, C
         ss << ","; 
         ss << "VrdExpr::VAR3<1,0,1,2>"; // VAR3<VGID,VARID,OBSID_ARG,VALID_ARG>   topicid_blue_dice[pos]=wordid C, C, V, V
      ss << ">"; // IAND
      for (int i=1; i<numTopics; i++)
      {
         ss << ",";
         ss << "VrdExpr::IAND<";
            ss << "VrdExpr::VAR2<0,0,1," << i << ">"; // VAR2<VGID,VARID_ARG,OBSID_ARG,VALID>  docid_red_dice[pos]=topicid   C, V, V, C
            ss << ","; 
            ss << "VrdExpr::VAR3<1," << i << ",1,2>"; // VAR3<VGID,VARID,OBSID_ARG,VALID_ARG>   topicid_blue_dice[pos]=wordid C, C, V, V
         ss << ">"; // IAND
      }
   ss << ">"; // EXOR
   ss << ",";
   ss << "BDSets<";

      // red dices  (NUMVARS=numDocs, NUMOUTCOMES=numTopics, NUMOBS=corpusSize)
      ss << "BDSet<" << numDocuments << "," << numTopics << "," << corpusSize << ">";

      ss << ",";

      // blue dices (NUMVARS=numTopics, NUMOUTCOMES=vocabSize, NUMOBS=corpusSize)
      ss << "BDSet<" << numTopics << "," << vocabSize << "," << corpusSize << ">";
   
   ss << ">"; // BDSets
   ss << ">"; // std::tuple
   return ss.str();
};

static std::string buildLDAmodelExpression_bdexpr3(int numTopics, int numDocuments, int vocabSize, int corpusSize)
{

   std::stringstream ss;
   ss << "std::tuple<" ;
   ss << "BDExpr3::EXOR" << "<";
      ss << "BDExpr3::IAND<";
         ss << "BDExpr3::VAR2<0,0,1,0>"; // VAR2<VGID,VARID_ARG,OBSID_ARG,VALID>  docid_red_dice[pos]=topicid   C, V, V, C
         ss << ","; 
         ss << "BDExpr3::VAR3<1,0,1,2>"; // VAR3<VGID,VARID,OBSID_ARG,VALID_ARG>   topicid_blue_dice[pos]=wordid C, C, V, V
      ss << ">"; // IAND
      for (int i=1; i<numTopics; i++)
      {
         ss << ",";
         ss << "BDExpr3::IAND<";
            ss << "BDExpr3::VAR2<0,0,1," << i << ">"; // VAR2<VGID,VARID_ARG,OBSID_ARG,VALID>  docid_red_dice[pos]=topicid   C, V, V, C
            ss << ","; 
            ss << "BDExpr3::VAR3<1," << i << ",1,2>"; // VAR3<VGID,VARID,OBSID_ARG,VALID_ARG>   topicid_blue_dice[pos]=wordid C, C, V, V
         ss << ">"; // IAND
      }
   ss << ">"; // EXOR
   ss << ",";
   ss << "BDSets<";

      // red dices  (NUMVARS=numDocs, NUMOUTCOMES=numTopics, NUMOBS=corpusSize)
      ss << "BDSet<" << numDocuments << "," << numTopics << "," << corpusSize << ">";

      ss << ",";

      // blue dices (NUMVARS=numTopics, NUMOUTCOMES=vocabSize, NUMOBS=corpusSize)
      ss << "BDSet<" << numTopics << "," << vocabSize << "," << corpusSize << ">";
   
   ss << ">"; // BDSets
   ss << ">"; // std::tuple
   return ss.str();
};



// [[clang::jit]] arrow::Status compileAndExecuteInMemoryLdaPlanP(

 template <typename SAMPLER_T>
 [[clang::jit]] arrow::Status compileAndExecuteInMemoryLdaPlanP(
   std::string& inputCsvUri, 
   std::string& vocabFilePath, 
   std::string& malletVocabFilePath, 
   std::string& lda_malletStateDir,
   std::string& lda_outfileId,
   int numIterations,
   double alpha,
   double beta,
   int rndSeedInit,
   int reportEvery,
   int num_threads,
   int startSavingFromIteration)
{
   using EXPR_T = tuple_element_t<0, SAMPLER_T>;
   using MODEL_T = tuple_element_t<1, SAMPLER_T>;

   MODEL_T model;
   std::get<0>(model).setSymmetricDirichletHyperParam(alpha); // alpha
   std::get<1>(model).setSymmetricDirichletHyperParam(beta); // beta

   std::shared_ptr<arrow::dataset::Dataset> dataset = buildArrowDatasetFromURI(inputCsvUri);
   Vocabulary vocab(vocabFilePath);
   Vocabulary malletVocab(malletVocabFilePath);

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

      std::shared_ptr<InMemoryGibbsPlanNodeP<EXPR_T, MODEL_T>> consumer = 
         std::make_shared<InMemoryGibbsPlanNodeP<EXPR_T, MODEL_T>>(finish, &model, &malletVocab, lda_malletStateDir, lda_outfileId, reportEvery,num_threads,startSavingFromIteration);

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

      std::cout << "In-memory Gibbs Plan Created : " << plan->ToString() << std::endl;

      // run the plan
      {
         TimeThis tt("In-memory LDA arrow plan execution");
         std::cout << "Executing plan..." << std::endl;
         ARROW_RETURN_NOT_OK(plan->StartProducing());
         
         ARROW_RETURN_NOT_OK(scanNode->finished().status());
         //ARROW_RETURN_NOT_OK(scanNode2->finished().status());

         uint64_t rndSeed = rndSeedInit;
         consumer->runGibbsSampler(numIterations, rndSeed);

         finish.MarkFinished(arrow::Status::OK());
         ARROW_RETURN_NOT_OK(plan->finished().status());
      }
      std::cout << "Finished!" << std::endl;
   }

   // return OK
   return arrow::Status::OK();

}



// [[clang::jit]] 

template <typename SAMPLER_T>
[[clang::jit]] arrow::Status compileAndExecuteInMemoryLdaPlan(
   std::string& inputCsvUri, 
   std::string& vocabFilePath, 
   std::string& malletVocabFilePath, 
   std::string& lda_malletStateDir,
   std::string& lda_outfileId,
   int numIterations,
   double alpha,
   double beta,
   int rndSeedInit,
   int reportEvery,
   int startSavingFromIteration)
{
   using EXPR_T = tuple_element_t<0, SAMPLER_T>;
   using MODEL_T = tuple_element_t<1, SAMPLER_T>;

   MODEL_T model;
   std::get<0>(model).setSymmetricDirichletHyperParam(alpha); // alpha
   std::get<1>(model).setSymmetricDirichletHyperParam(beta); // beta

   std::shared_ptr<arrow::dataset::Dataset> dataset = buildArrowDatasetFromURI(inputCsvUri);
   Vocabulary vocab(vocabFilePath);
   Vocabulary malletVocab(malletVocabFilePath);

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

      std::shared_ptr<InMemoryGibbsPlanNode<EXPR_T, MODEL_T>> consumer = 
         std::make_shared<InMemoryGibbsPlanNode<EXPR_T, MODEL_T>>(finish, &model, &malletVocab, lda_malletStateDir, lda_outfileId, reportEvery,startSavingFromIteration);

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

      std::cout << "In-memory Gibbs Plan Created : " << plan->ToString() << std::endl;

      // run the plan
      {
         TimeThis tt("In-memory LDA arrow plan execution");
         std::cout << "Executing plan..." << std::endl;
         ARROW_RETURN_NOT_OK(plan->StartProducing());
         
         ARROW_RETURN_NOT_OK(scanNode->finished().status());
         //ARROW_RETURN_NOT_OK(scanNode2->finished().status());

         uint64_t rndSeed = rndSeedInit;
         consumer->runGibbsSampler(numIterations, rndSeed);

         finish.MarkFinished(arrow::Status::OK());
         ARROW_RETURN_NOT_OK(plan->finished().status());
      }
      std::cout << "Finished!" << std::endl;
   }

   // return OK
   return arrow::Status::OK();

}


template <typename SAMPLER_T>
[[clang::jit]] arrow::Status compileAndExecuteLdaPlan(int numIterations)
{
   using EXPR_T = tuple_element_t<0, SAMPLER_T>;
   using MODEL_T = tuple_element_t<1, SAMPLER_T>;

   MODEL_T model;

   //std::string inputCsvUri{"file:/scratch/lda_datasets_2023/NYTIMES_train/csv2/NYTIMES_train.csv"};
   std::string inputCsvUri{"file:/home/niccolom/vs_projects/gammapdb_arrow/data/NYTIMES_train/csv2/NYTIMES_train.csv"};

   std::shared_ptr<arrow::dataset::Dataset> dataset = buildArrowDatasetFromURI(inputCsvUri);

   //Vocabulary vocab("/scratch/lda_datasets_2023/NYTIMES_train/csv2/NYTIMES_vocab.csv");
   Vocabulary vocab("/home/niccolom/vs_projects/gammapdb_arrow/data/NYTIMES_train/csv2/NYTIMES_vocab.csv");

   // build and execute the init execution plan
   {
      arrow::compute::ExecContext exec_context;
      std::shared_ptr<arrow::compute::ExecPlan> plan;
      ARROW_ASSIGN_OR_RAISE(plan, arrow::compute::ExecPlan::Make(&exec_context));

      // create the scanNode
      arrow::compute::ExecNode* scanNode; 
      ARROW_RETURN_NOT_OK(buildArrowScanNodeFromDataset(scanNode, dataset, plan));

      // create sink node
      arrow::Future<> finish = arrow::Future<>::Make();

      std::shared_ptr<GibbsInitSinkNodeConsumer<EXPR_T, MODEL_T>> consumer = 
         std::make_shared<GibbsInitSinkNodeConsumer<EXPR_T, MODEL_T>>(finish, &model);

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

      std::cout << "Init Plan Created : " << plan->ToString() << std::endl;

      // run the plan
      {
         TimeThis tt("LDA arrow init plan execution");
         std::cout << "Executing init plan..." << std::endl;
         ARROW_RETURN_NOT_OK(plan->StartProducing());
         
         ARROW_RETURN_NOT_OK(scanNode->finished().status());
         //ARROW_RETURN_NOT_OK(scanNode2->finished().status());

         finish.MarkFinished(arrow::Status::OK());
         ARROW_RETURN_NOT_OK(plan->finished().status());
      }
      std::cout << "Finished!" << std::endl;
   }

   for (int i=0; i<numIterations; ++i)
   {
      arrow::compute::ExecContext exec_context;
      std::shared_ptr<arrow::compute::ExecPlan> plan;
      ARROW_ASSIGN_OR_RAISE(plan, arrow::compute::ExecPlan::Make(&exec_context));

      // create the scanNode
      arrow::compute::ExecNode* scanNode; 
      ARROW_RETURN_NOT_OK(buildArrowScanNodeFromDataset(scanNode, dataset, plan));

      /*
      arrow::compute::ExecNode* scanNode2; 
      ARROW_RETURN_NOT_OK(buildArrowScanNodeFromDataset(scanNode2, dataset, plan));

      auto union_node_options = arrow::compute::ExecNodeOptions();
      arrow::compute::ExecNode* union_node;
      ARROW_ASSIGN_OR_RAISE(union_node, 
         MakeExecNode("union", 
            plan.get(), 
            {scanNode, scanNode2},
            union_node_options
         )
      );
      */

      // create sink node
      arrow::Future<> finish = arrow::Future<>::Make();

      std::shared_ptr<GibbsSweepSinkNodeConsumer<EXPR_T, MODEL_T>> consumer = 
         std::make_shared<GibbsSweepSinkNodeConsumer<EXPR_T, MODEL_T>>(finish, &model);

      arrow::compute::ExecNode* consuming_sink;
      ARROW_ASSIGN_OR_RAISE(consuming_sink, 
         MakeExecNode("consuming_sink", 
            plan.get(), 
            {scanNode}, // {union_node}, // 
            arrow::compute::ConsumingSinkNodeOptions(consumer)
         )
      );

      // validate the plan
      ARROW_RETURN_NOT_OK(scanNode->Validate());
      //ARROW_RETURN_NOT_OK(scanNode2->Validate());
      ARROW_RETURN_NOT_OK(consuming_sink->Validate());
      ARROW_RETURN_NOT_OK(plan->Validate());

      std::cout << "MCMC Sweep Plan Created : " << plan->ToString() << std::endl;

      // run the plan
      {
         TimeThis tt("LDA arrow plan execution");
         std::cout << "Executing plan..." << std::endl;
         ARROW_RETURN_NOT_OK(plan->StartProducing());
         
         ARROW_RETURN_NOT_OK(scanNode->finished().status());
         //ARROW_RETURN_NOT_OK(scanNode2->finished().status());

         finish.MarkFinished(arrow::Status::OK());
         ARROW_RETURN_NOT_OK(plan->finished().status());
      }
      std::cout << "Finished!" << std::endl;

      // report, if needed
      if ((i>0)&&((i%2)==0))
      {
         std::vector<std::vector<int>> summary = std::get<1>(model).summarizePosterior(20,10); // 20 topics, 10 top words each
         
         for (auto j : summary)
         {
            std::cout << std::endl;
            for (auto wordCode : j)
            {
               std::cout << vocab.getWordByCode(wordCode) << " ";
            }
            std::cout << std::endl;
         }
         std::cout << std::endl;
      }

   }

   // return OK
   return arrow::Status::OK();
};



   /*
   arrow::compute::ExecNode* scanNode2; 
   ARROW_RETURN_NOT_OK(buildArrowScanNodeFromDataset(scanNode2, dataset, plan));

   auto union_node_options = arrow::compute::ExecNodeOptions();
   arrow::compute::ExecNode* union_node;
   ARROW_ASSIGN_OR_RAISE(union_node, 
      MakeExecNode("union", 
         plan.get(), 
         {scanNode, scanNode2},
         union_node_options
      )
   );
   */






#endif // _LDAPLANBUILDER_H_