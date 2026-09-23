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

#include "utils/argparse.hpp"
#include "utils/DebugHelper.h"
#include "utils/TimeThis.h"

#include <cmath>     // log2
#include <iostream>
#include <signal.h> 
#include <string>

#include "lda/LdaPlanBuilder.h"
#include "hmm/HmmPlanBuilder.h"
#include <arrow/status.h>

#include <tuple>
#include "lda_svi/MappingReader.h"

#include "ReadData.h"
#include "lda_svi/LdaSVIPlanBuilder.h"

// remove this
// #include "BDSet.h"
// #include "BDSetP.h"
// #include "BDExpr2.h"


//#include "FancyMacros.h"
//#include <climits>



int runMe(int argc, const char **argv);

void executeInMemoryLdaPlan(
   std::string &lda_variant,
   std::string &lda_datasetName,
   std::string &lda_datasetNameSuffix,
   std::string &lda_uciDatasetsDir,
   std::string &lda_malletStateDir,
   std::string &lda_outfileId,
   int lda_numDocuments,
   int lda_vocabSize,
   int lda_corpusSize,
   int lda_numTopics,
   int lda_numIterations,
   double lda_alpha,
   double lda_beta,
   int rndSeed,
   int reportEvery,
   int num_threads,
   int startSavingFromIteration
);

void executeHmmPlan(int rndSeed, int reportEvery);


int main(int argc, const char **argv)
{
    signal(SIGSEGV, signal_handler);
    std::set_terminate(uncaught_exception_handler);
    return runMe(argc, argv);
};



int runMe(int argc, const char **argv)
{
   SUPPRESS_UNUSED_PARAM_WARNING(argc);
   SUPPRESS_UNUSED_PARAM_WARNING(argv);

   // default arguments
   std::string cmd="info";
   std::string lda_variant="pcexpr";
   std::string lda_datasetName="NYTIMES";
   std::string lda_datasetNameSuffix="";
   int lda_numDocuments = -1;
   int lda_vocabSize = -1;   // (cat /scratch/lda_datasets_2023/NYTIMES_train/csv2/NYTIMES_vocab.csv | wc -l) minus 1
   int lda_corpusSize = -1;  // (cat /scratch/lda_datasets_2023/NYTIMES_train/csv2/NYTIMES_train.csv | wc -l) minus 2
   double lda_alpha = 0.2;
   double lda_beta = 0.1;
   int lda_numTopics = 20;
   int lda_numIterations = 10;
   int rndSeed = 12345;
   int reportEvery = 5000;
   int num_threads=1;
   int batchSize = -1;
   int vi_iter_per_batch = 1;
   std::string lda_vi_dataset_dir = getexepath_parent()+"/data/NYTIMES_train/csv2";
   int total_num_iter=1;
   std::string outputDir = getexepath_parent()+"/benchmarks";
   std::string lda_uciDatasetsDir = getexepath_parent()+"/data";
   std::string lda_malletStateDir = outputDir+"/gammapdb_data/chain_states";
   int startSavingFromIteration = 0;

   // declare arguments
   const bool isOptionalArg = true;
   ArgumentParser argsParser;
   // declare arguments
   argsParser.addArgument("-c", "--command", 1, isOptionalArg);
   argsParser.addArgument("-d", "--lda_datasetName", 1, isOptionalArg);
   argsParser.addArgument("-s", "--lda_datasetNameSuffix", 1, isOptionalArg);
   argsParser.addArgument("--lda_numDocuments", 1, isOptionalArg);
   argsParser.addArgument("--lda_vocabSize", 1, isOptionalArg);
   argsParser.addArgument("--lda_corpusSize", 1, isOptionalArg);
   argsParser.addArgument("-a", "--lda_alpha", 1, isOptionalArg);
   argsParser.addArgument("-b", "--lda_beta", 1, isOptionalArg);
   argsParser.addArgument("--lda_numTopics", 1, isOptionalArg);
   argsParser.addArgument("-n", "--lda_numIterations", 1, isOptionalArg); 
   argsParser.addArgument("-r", "--rndSeed", 1, isOptionalArg);
   argsParser.addArgument("--reportEvery", 1, isOptionalArg);
   argsParser.addArgument("-o", "--outputDir", 1, isOptionalArg);
   argsParser.addArgument("--lda_uciDatasetsDir", 1, isOptionalArg);
   argsParser.addArgument("--lda_malletStateDir", 1, isOptionalArg);
   argsParser.addArgument("--lda_outfileId", 1, isOptionalArg);
   argsParser.addArgument("--numThreads", 1, isOptionalArg);

   // new arguments for svi
   argsParser.addArgument("--vi_iter_per_batch", 1, isOptionalArg);
   argsParser.addArgument("--batchSize", 1, isOptionalArg);
   argsParser.addArgument("--lda_vi_dataset_dir", 1, isOptionalArg);
   argsParser.addArgument("--total_num_iter", 1, isOptionalArg);

   // new argument for startSavingFromIteration
   argsParser.addArgument("--startSavingFromIteration", 1, isOptionalArg);


   // parse arguments
   argsParser.parse(argc, argv);

   if (argsParser.count("command")) {
      cmd = argsParser.retrieve<std::string>("command");
   }
   if (argsParser.count("lda_datasetName")) {
      lda_datasetName = argsParser.retrieve<std::string>("lda_datasetName");
   }
   if (argsParser.count("lda_datasetNameSuffix")) {
      lda_datasetNameSuffix = argsParser.retrieve<std::string>("lda_datasetNameSuffix");
   }
   if (argsParser.count("lda_numDocuments")) {
      lda_numDocuments = std::stoi(argsParser.retrieve<std::string>("lda_numDocuments"));
   }
   if (argsParser.count("lda_vocabSize")) {
      lda_vocabSize = std::stoi(argsParser.retrieve<std::string>("lda_vocabSize"));
   }
   if (argsParser.count("lda_corpusSize")) {
      lda_corpusSize = std::stoi(argsParser.retrieve<std::string>("lda_corpusSize"));
   }
   if (argsParser.count("lda_alpha")) {
      lda_alpha = std::stod(argsParser.retrieve<std::string>("lda_alpha"));
   }
   if (argsParser.count("lda_beta")) {
      lda_beta = std::stod(argsParser.retrieve<std::string>("lda_beta"));
   }
   if (argsParser.count("lda_numTopics")) {
      lda_numTopics = std::stoi(argsParser.retrieve<std::string>("lda_numTopics"));
   }
   if (argsParser.count("lda_numIterations")) {
      lda_numIterations = std::stoi(argsParser.retrieve<std::string>("lda_numIterations"));
   }
   if (argsParser.count("rndSeed")) {
      rndSeed = std::stoi(argsParser.retrieve<std::string>("rndSeed"));
   }
   if (argsParser.count("reportEvery")) {
      reportEvery = std::stoi(argsParser.retrieve<std::string>("reportEvery"));
   }
   if (argsParser.count("outputDir")) {
      outputDir = argsParser.retrieve<std::string>("outputDir");
   }
   if (argsParser.count("lda_uciDatasetsDir")) {
      lda_uciDatasetsDir = argsParser.retrieve<std::string>("lda_uciDatasetsDir");
   }
   if (argsParser.count("lda_malletStateDir")) {
      lda_malletStateDir = argsParser.retrieve<std::string>("lda_malletStateDir");
   }
      if (argsParser.count("numThreads")) {
      num_threads = std::stoi(argsParser.retrieve<std::string>("numThreads"));
   }

   if (argsParser.count("batchSize"))
   {
      batchSize = std::stoi(argsParser.retrieve<std::string>("batchSize"));
   }
   if (argsParser.count("vi_iter_per_batch"))
   {
      vi_iter_per_batch = std::stoi(argsParser.retrieve<std::string>("vi_iter_per_batch"));
   }
   if (argsParser.count("lda_vi_dataset_dir"))
   {
      lda_vi_dataset_dir = argsParser.retrieve<std::string>("lda_vi_dataset_dir");
   }
   if (argsParser.count("total_num_iter"))
   {
      total_num_iter = std::stoi(argsParser.retrieve<std::string>("total_num_iter"));
   }
   if (argsParser.count("startSavingFromIteration")) {
      startSavingFromIteration = std::stoi(argsParser.retrieve<std::string>("startSavingFromIteration"));
   }

   std::string lda_outfileId = lda_datasetName+lda_datasetNameSuffix+"_"+std::to_string(lda_numTopics)+"topics_A"+std::to_string(lda_alpha)+"_B"+std::to_string(lda_beta)+"_NI"+std::to_string(lda_numIterations)+"_NT_"+std::to_string(num_threads)+"_RND"+std::to_string(rndSeed);
   if (argsParser.count("lda_outfileId")) {
      lda_outfileId = argsParser.retrieve<std::string>("lda_outfileId");
   }

   // print arguments
   std::cout << "------------------------------------------------------------------------------------" << std::endl;
   std::cout << "GammaPDB Arrow Engine " << std::endl;
   std::cout << "------------------------------------------------------------------------------------" << std::endl;
   std::cout << "current time         = " << getCurrentTimeStampAsString() << std::endl;
   std::cout << "command              = " << cmd << std::endl;
   if (cmd=="hmm-inmemory-vrexpr")
   {
      std::cout << "rndSeed              = " << rndSeed << std::endl;
      std::cout << "reportEvery          = " << reportEvery << std::endl;
      std::cout << "------------------------------------------------------------------------------------" << std::endl;
   }
   else
   {
      std::cout << "lda_datasetName      = " << lda_datasetName << lda_datasetNameSuffix << std::endl;

      std::cout << "lda_numDocuments     = " << lda_numDocuments << std::endl;
      std::cout << "lda_vocabSize        = " << lda_vocabSize << std::endl;
      std::cout << "lda_corpusSize       = " << lda_corpusSize << std::endl;

      std::cout << "lda_alpha            = " << lda_alpha << std::endl;
      std::cout << "lda_beta             = " << lda_beta << std::endl;
      std::cout << "lda_numTopics        = " << lda_numTopics << std::endl;
      std::cout << "lda_numIterations    = " << lda_numIterations << std::endl;
      std::cout << "rndSeed              = " << rndSeed << std::endl;
      std::cout << "reportEvery          = " << reportEvery << std::endl;
      std::cout << "outputDir            = " << outputDir << std::endl;
      std::cout << "lda_uciDatasetsDir   = " << lda_uciDatasetsDir << std::endl;
      std::cout << "lda_malletStateDir   = " << lda_malletStateDir << std::endl;
      std::cout << "lda_outfileId        = " << lda_outfileId << std::endl;
      std::cout << "num_threads        = " << num_threads << std::endl;
      std::cout << "startSavingFromIteration = " << startSavingFromIteration << std::endl;
      if(cmd=="svi")
      {
         std::cout<<"lda_vi_dataset_dir   = " << lda_vi_dataset_dir << std::endl;
         std::cout<<"batchSize            = " << batchSize << std::endl;
         std::cout<<"vi_iter_per_batch    = " << vi_iter_per_batch << std::endl;
         std::cout<<"total_num_iter       = " << total_num_iter << std::endl;
      }
      std::cout << "------------------------------------------------------------------------------------" << std::endl;
   }
   if (cmd=="lda-inmemory-pcexpr") {

      std::string lda_variant = "pcexpr";
      executeInMemoryLdaPlan(
         lda_variant,
         lda_datasetName, 
         lda_datasetNameSuffix,
         lda_uciDatasetsDir,
         lda_malletStateDir,
         lda_outfileId,
         lda_numDocuments,
         lda_vocabSize,
         lda_corpusSize,
         lda_numTopics,
         lda_numIterations,
         lda_alpha,
         lda_beta,
         rndSeed,
         reportEvery,
         num_threads,
         startSavingFromIteration);

   } else if (cmd=="lda-inmemory-vrexpr") {

      std::string lda_variant = "vrexpr";
      executeInMemoryLdaPlan(
         lda_variant,
         lda_datasetName, 
         lda_datasetNameSuffix,
         lda_uciDatasetsDir,
         lda_malletStateDir,
         lda_outfileId,
         lda_numDocuments,
         lda_vocabSize,
         lda_corpusSize,
         lda_numTopics,
         lda_numIterations,
         lda_alpha,
         lda_beta,
         rndSeed,
         reportEvery,
         num_threads,
         startSavingFromIteration);

   } 
   else if (cmd=="lda-inmemory-vrexprP") {

      std::string lda_variant = "vrexprP";
      executeInMemoryLdaPlan(
         lda_variant,
         lda_datasetName, 
         lda_datasetNameSuffix,
         lda_uciDatasetsDir,
         lda_malletStateDir,
         lda_outfileId,
         lda_numDocuments,
         lda_vocabSize,
         lda_corpusSize,
         lda_numTopics,
         lda_numIterations,
         lda_alpha,
         lda_beta,
         rndSeed,
         reportEvery,
         num_threads,
         startSavingFromIteration);

   } else if (cmd=="hmm-inmemory-vrexpr")
   {
      executeHmmPlan(rndSeed, reportEvery);
   }

   else if(cmd=="svi")
   {
      int nb_sat_assig_per_constraint = lda_numTopics;

      std::string inputCsvUri = "file://" + lda_vi_dataset_dir + "/csv2/" + lda_datasetName + "_train_svi.csv";
      std::cout << "inputCsvUri = " << inputCsvUri << std::endl;

      float priors[] = {static_cast<float>(lda_alpha), static_cast<float>(lda_beta)};

      std::cout << "redPrior = " << priors[0] << std::endl;
      std::cout << "bluePrior = " << priors[1] << std::endl;

      auto data = parseFileToFlatVector(lda_vi_dataset_dir + "/csv2/" + lda_datasetName + "_train.csv");
      std::cout << "Size of data: " << data.size() << std::endl;

      std::string malletVocabFilePath = lda_vi_dataset_dir + "/csv2/" + lda_datasetName + "_vocab_mallet.csv";

      std::string mappingFilePath = lda_vi_dataset_dir + "/csv2/" + lda_datasetName + "_train_svi_mapping.csv";
      Vocabulary malletVocab(malletVocabFilePath);
      std::cout << "Finished reading the vocab" << std::endl;
      MappingReader mapping(mappingFilePath);
      std::cout<<"Finished reading the mapping"<<std::endl;
      std::string pattern = "";

      pattern = generateLDAPattern(lda_numTopics, lda_numDocuments, lda_numTopics, lda_numTopics, lda_vocabSize);

      std::cout << pattern << std::endl;

      setenv("OMP_PROC_BIND", "true", 1);
      setenv("OMP_PLACES", "cores", 1);
auto jit_start = std::chrono::high_resolution_clock::now();
      compileAndExecuteInMemorySviLDAPlan<pattern>(
            inputCsvUri,
            data, //this is not data for training, it's for eval later
            nb_sat_assig_per_constraint,
            total_num_iter,
            priors,
            malletVocab,
            lda_malletStateDir,
            lda_outfileId,
            batchSize,
            vi_iter_per_batch,
            reportEvery,
            num_threads,
            mapping);
            auto jit_end = std::chrono::high_resolution_clock::now();

auto total_ms = std::chrono::duration_cast<std::chrono::milliseconds>(jit_end - jit_start).count();

std::cout << "--------------------------------------------------" << std::endl;
std::cout << " [Profile][Main] Total Call Duration: " << total_ms << " ms" << std::endl;
std::cout << " (This includes JIT Compilation + Function Execution)" << std::endl;
std::cout << "--------------------------------------------------" << std::endl;
   }
   return 0;
};


void executeHmmPlan(int rndSeed, int reportEvery)
{
   std::string dataDir = getexepath_parent()+"/data";

   std::string inputCsvUri1 = "file:" + dataDir + "/HmmSyn01_train.csv";
   std::string inputCsvUri2 = "file:" + dataDir + "/HmmSyn02_train.csv";
   std::string inputCsvUri3 = "file:" + dataDir + "/HmmSyn03_train.csv";

   std::string modelStr = buildHmmModelExpression_variadic();
   int numIterations = 20;
   
   //std::cout << "modelStr=" << modelStr << std::endl;
   {
      std::string outfileId = getexepath_parent()+"/benchmarks/gammapdb_hmm_model_01_";
      arrow::Status finalState = compileAndExecuteInMemoryHmmPlan<modelStr>(inputCsvUri1, outfileId, numIterations, rndSeed+9, reportEvery);

      if (finalState!=arrow::Status::OK()) {
         std::cout << "Something went wrong.. :(" << std::endl;
      }
   }

   {
      std::string outfileId = getexepath_parent()+"/benchmarks/gammapdb_hmm_model_02_";
      arrow::Status finalState = compileAndExecuteInMemoryHmmPlan<modelStr>(inputCsvUri2, outfileId, numIterations, rndSeed+99, reportEvery);

      if (finalState!=arrow::Status::OK()) {
         std::cout << "Something went wrong.. :(" << std::endl;
      }
   }

   {
      std::string outfileId = getexepath_parent()+"/benchmarks/gammapdb_hmm_model_03_";
      arrow::Status finalState = compileAndExecuteInMemoryHmmPlan<modelStr>(inputCsvUri3, outfileId, numIterations, rndSeed+999, reportEvery);

      if (finalState!=arrow::Status::OK()) {
         std::cout << "Something went wrong.. :(" << std::endl;
      }
   }

};

void executeInMemoryLdaPlan(
   std::string &lda_variant,
   std::string &lda_datasetName,
   std::string &lda_datasetNameSuffix,
   std::string &lda_uciDatasetsDir,
   std::string &lda_malletStateDir,
   std::string &lda_outfileId,
   int lda_numDocuments,
   int lda_vocabSize,
   int lda_corpusSize,
   int lda_numTopics,
   int lda_numIterations,
   double lda_alpha,
   double lda_beta,
   int rndSeed,
   int reportEvery,
   int num_threads,
   int startSavingFromIteration
)
{
   std::string inputCsvUri = "file:" + lda_uciDatasetsDir + "/" + lda_datasetName + lda_datasetNameSuffix + "_train.csv";

   std::string vocabFilePath = lda_uciDatasetsDir + "/" + lda_datasetName + "_vocab.csv";

   std::string malletVocabFilePath = lda_uciDatasetsDir + "/" + lda_datasetName + "_vocab_mallet.csv";

   std::string modelStr;

   if (lda_variant=="vrexpr")
   {
      // use variadic expressions
      modelStr = buildLDAmodelExpression_variadic(lda_numTopics, lda_numDocuments, lda_vocabSize, lda_corpusSize);
   } 
   else if (lda_variant=="pcexpr")
   {
      // use pre-compiled expressions
      modelStr = buildLDAmodelExpression_pcexpr(lda_numTopics, lda_numDocuments, lda_vocabSize, lda_corpusSize);
   }
   else if (lda_variant=="bdexpr3")
   {
      modelStr = buildLDAmodelExpression_bdexpr3(lda_numTopics, lda_numDocuments, lda_vocabSize, lda_corpusSize);
   }
   else if (lda_variant=="vrexprP")
   {
      // use variadic expressions
      modelStr = buildLDAmodelExpression_variadicP(lda_numTopics, lda_numDocuments, lda_vocabSize, lda_corpusSize);
   } 
   else
   {
      throw new std::runtime_error("invalid lda implementation");
   }
   std::cout << modelStr << std::endl;

   if (lda_variant=="vrexprP")
   {
         arrow::Status finalState = compileAndExecuteInMemoryLdaPlanP<modelStr>(inputCsvUri, vocabFilePath, malletVocabFilePath, lda_malletStateDir, lda_outfileId, lda_numIterations, lda_alpha, lda_beta, rndSeed, reportEvery,num_threads,startSavingFromIteration);
            if (finalState!=arrow::Status::OK()) {
               std::cout << "Something went wrong.. :(" << std::endl;
            }
   }
   else {
   arrow::Status finalState = compileAndExecuteInMemoryLdaPlan<modelStr>(inputCsvUri, vocabFilePath, malletVocabFilePath, lda_malletStateDir, lda_outfileId, lda_numIterations, lda_alpha, lda_beta, rndSeed, reportEvery,startSavingFromIteration);

   if (finalState!=arrow::Status::OK()) {
      std::cout << "Something went wrong.. :(" << std::endl;
   }
   }

};


