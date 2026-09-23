// Standard Library Headers
#include <chrono>
#include <cstdio>
#include <cstring>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <map>
#include <random>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

// POSIX / System Headers
#include <signal.h> // signal()
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

// Local Headers
#include "clean.h"
#include "Utils/argparse.hpp"
#include "Utils/DebugHelper.h"
#include "Utils/TimeThis.h"
#include "Utils/RndUtils.h"
#include "Vocabulary.h"

std::string getCurrentTimeStampAsString()
{
   std::time_t currentTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
   std::string timeStr(std::ctime(&currentTime));
   return timeStr.substr(0, timeStr.size() - 1);
}

void convertWikiData(std::string &datasetName,
                     std::string &vocabFilePath,
                     std::string &datasetNameSuffix,
                     // int numberOfTestDocs,
                     std::string &outputDir,
                     std::mt19937 &rndGen,
                     int reportEvery,
                     std::string wikiDatasetsFile,
                     double holdOutFreq = 0.05)
{

   std::string trainingSetDirPathCSV2(outputDir + "/" + datasetName + datasetNameSuffix + "_train/csv2");
   std::string testSetDirPathCSV2(outputDir + "/" + datasetName + datasetNameSuffix + "_test/csv2");
   std::string trainingSetDirPathTSV(outputDir + "/" + datasetName + datasetNameSuffix + "_train/tsv");
   std::string testSetDirPathTSV(outputDir + "/" + datasetName + datasetNameSuffix + "_test/tsv");
   std::string cmd1("mkdir -p " + trainingSetDirPathCSV2);
   std::string cmd2("mkdir -p " + testSetDirPathCSV2);
   std::string cmd3("mkdir -p " + trainingSetDirPathTSV);
   std::string cmd4("mkdir -p " + testSetDirPathTSV);
   system(&*cmd1.begin());
   system(&*cmd2.begin());
   system(&*cmd3.begin());
   system(&*cmd4.begin());

   std::ifstream inFile(wikiDatasetsFile);
   Vocabulary vocab(vocabFilePath);

   std::cout << "Vocab size: " << vocab.getSize() << std::endl;
   std::vector<int> malletVocab(vocab.getSize(), -1);

   std::ofstream MalletVocabFile(trainingSetDirPathCSV2 + "/" + datasetName + datasetNameSuffix + "_vocab_mallet.csv");

   std::ofstream trainingSet(trainingSetDirPathTSV + "/training.tsv");
   std::ofstream testSet(testSetDirPathTSV + "/test.tsv");

   std::ofstream trainingSetCSV(trainingSetDirPathCSV2 + "/" + datasetName + datasetNameSuffix + "_train.csv");
   std::ofstream testSetCSV(testSetDirPathCSV2 + "/" + datasetName + datasetNameSuffix + "_test.csv");

   std::string line, word;
   int nextMalletWordCode = 0;

   int global_count = 0;
   int position = 0;
   // bool belongsToTrain = false;

   std::vector<std::string> trainingDocs;
   std::vector<std::string> testDocs;

   trainingSetCSV << "docId,pos,wordId" << std::endl;
   testSetCSV << "docId,pos,wordId" << std::endl;

   int progress=0;
   while (std::getline(inFile, line))
   {

      // Decide whether this is a training or test document
      if (RndUtils::flipCoin(1 - holdOutFreq, rndGen))
      {
         // doc belongs to the training set
         // belongsToTrain = true;
         // trainingSet << "doc" << intToStrWithLeadingZeroes(docId, 12) << '\t' << "x  " << line << std::endl;
         trainingDocs.push_back(line);
      }

      else
      {
         // doc belongs to the test set
         // belongsToTrain = false;
         testDocs.push_back(line);
      }
      if(progress % reportEvery == 0)
      {
         std::cout << "Reading and splitting into training/test set [" << getCurrentTimeStampAsString() << "] " << progress << " documents processed ..." << std::endl;
      }
      progress++;
   }

   std::cout<<"writing tsv and csv2..."<<std::endl;
   for (int i = 0; i < trainingDocs.size(); i++)
   {
      std::ostringstream ss;

      std::string& line = trainingDocs[i];

      // write in the tsv file
      trainingSet << "doc" << intToStrWithLeadingZeroes(i, 12) << '\t' << "x  " << line << std::endl;

      // Process each token individually to convert it to its correspondant word code
      std::istringstream iss(line);
      while (iss >> word)
      {

         int wordId = vocab.getWordCode(word);

         if (malletVocab[wordId] == -1)
         {
            malletVocab[wordId] = nextMalletWordCode;
            nextMalletWordCode++;
         }

         ss << i << "," << position << "," << wordId << std::endl;

         position++;
      }

      trainingSetCSV << ss.str();
      if(i%reportEvery == 0)
      {
         std::cout << "wrting training tsv and csv [" << getCurrentTimeStampAsString() << "] " << i << " documents processed ..." << std::endl;
      }
   }

   

   for (int i = 0; i < testDocs.size(); i++)

   {
      std::ostringstream ss;
      int offset = trainingDocs.size();
      // write in the tsv file
      std::string& line = testDocs[i];
      testSet << "doc" << intToStrWithLeadingZeroes(i + offset, 12) << '\t' << "x  " << line << std::endl;

      std::istringstream iss(line);
      while (iss >> word)
      {

         int wordId = vocab.getWordCode(word);
         ss << i + offset << "," << position << "," << wordId << std::endl;
         position++;
      }

      testSetCSV << ss.str();

      if(i%reportEvery == 0)
      {
         std::cout << "wrting test tsv and csv [" << getCurrentTimeStampAsString() << "] " << i << " documents processed ..." << std::endl;
      }
   }

   std::cout << "Total number of the trated documents: " << testDocs.size()+trainingDocs.size() << std::endl;

   // save Mallet vocabulary
   {
      MalletVocabFile << "malletWordCode wordStr" << std::endl;
      for (int i = 0; i < vocab.getSize(); i++)
      {
         // std::cout << "MalletVocab[" << i << "] = " << MalletVocab[i] << std::endl;
         MalletVocabFile << malletVocab[i] << " " << vocab.getWordByCode(i) << std::endl;
      }
      MalletVocabFile.close();
   }

   // convert the vocab file
   std::string vocabFile1 = trainingSetDirPathCSV2 + "/" + datasetName + datasetNameSuffix + "_vocab.csv";
   std::string vocabFile2 = testSetDirPathCSV2 + "/" + datasetName + datasetNameSuffix + "_vocab.csv";
   vocab.saveAsCsvFile(vocabFile1);
   vocab.saveAsCsvFile(vocabFile2);

   // std::string cmd6("cp " + trainingSetDirPathCSV2 + "/" + datasetName + datasetNameSuffix + "_vocab.csv " + testSetDirPathCSV2 + "/" + datasetName + datasetNameSuffix + "_vocab.csv");
   // system(&*cmd6.begin());
   std::cout << "Total number of treated tockens is = " << position << std::endl;
}

int runMe(int argc, const char **argv)
{
   // default arguments

   int doCleaning = 0;
   double holdOutFreq = 0.05;

   std::string datasetName = "wiki08";
   std::string datasetNameSuffix = "";
   std::string vocabFile = "/gammapdb_arrow/data/raw/wiki08_clean_vocab.txt";
   int totalNbOfDocs = 5000000;
   int rndSeed = 12345;
   int reportEvery = 10000;
   std::string wikiDatasetFile = "/gammapdb_arrow/data/raw/wiki08_clean.txt";
   std::string outputDir = "/gammapdb_arrow/data";
   std::string wikiDatasetsDir = "/gammapdb_arrow/data/raw";
   // declare arguments
   const bool isOptionalArg = true;
   ArgumentParser argsParser;
   // declare arguments
   argsParser.addArgument("--dataset", 1, isOptionalArg);
   argsParser.addArgument("--datasetNameSuffix", 1, isOptionalArg);
   argsParser.addArgument("--totalNbOfDocs", 1, isOptionalArg);

   argsParser.addArgument("--rndSeed", 1, isOptionalArg);
   argsParser.addArgument("--reportEvery", 1, isOptionalArg);
   // mandatory args
   argsParser.addArgument("--outputDir", 1, isOptionalArg);
   argsParser.addArgument("--wikiDatasetsDir", 1, isOptionalArg);
   argsParser.addArgument("--doCleaning", 1, isOptionalArg);
   argsParser.addArgument("--holdOutFreq", 1, isOptionalArg);

   // parse arguments
   argsParser.parse(argc, argv);

   // read optional arguments
   if (argsParser.count("dataset"))
   {
      datasetName = argsParser.retrieve<std::string>("dataset");
   }
   if (argsParser.count("datasetNameSuffix"))
   {
      datasetNameSuffix = argsParser.retrieve<std::string>("datasetNameSuffix");
   }

   if (argsParser.count("totalNbOfDocs"))
   {
      totalNbOfDocs = std::stod(argsParser.retrieve<std::string>("totalNbOfDocs"));
   }
   if (argsParser.count("rndSeed"))
   {
      rndSeed = std::stoi(argsParser.retrieve<std::string>("rndSeed"));
   }
   if (argsParser.count("reportEvery"))
   {
      reportEvery = std::stoi(argsParser.retrieve<std::string>("reportEvery"));
   }
   if (argsParser.count("outputDir"))
   {
      outputDir = argsParser.retrieve<std::string>("outputDir");
   }
   if (argsParser.count("wikiDatasetsDir"))
   {
      wikiDatasetsDir = argsParser.retrieve<std::string>("wikiDatasetsDir");
   }
   if (argsParser.count("doCleaning"))
   {
      doCleaning = std::stoi(argsParser.retrieve<std::string>("doCleaning"));
   }
   if (argsParser.count("holdOutFreq"))
   {
      holdOutFreq = std::stod(argsParser.retrieve<std::string>("holdOutFreq"));
   }

   wikiDatasetFile = wikiDatasetsDir + "/" + datasetName + datasetNameSuffix + ".txt";
   vocabFile = wikiDatasetsDir + "/" + datasetName + datasetNameSuffix + "_clean_vocab.txt";

   // print arguments
   std::cout << "-----------------------------------------------------------" << std::endl;
   std::cout << "WIKI LDA PREPROCESS" << std::endl;
   std::cout << "-----------------------------------------------------------" << std::endl;
   std::cout << "current time         = " << getCurrentTimeStampAsString() << std::endl;
   std::cout << "dataset              = " << datasetName << datasetNameSuffix << std::endl;
   std::cout << "totalNbOfDocs =     " << totalNbOfDocs << std::endl;
   std::cout << "rndSeed              = " << rndSeed << std::endl;
   std::cout << "reportEvery          = " << reportEvery << std::endl;
   std::cout << "outputDir            = " << outputDir << std::endl;
   std::cout << "wikiDatasetsDir       = " << wikiDatasetsDir << std::endl;
   std::cout << "vocabFile            = " << vocabFile << std::endl;
   std::cout << "doCleaning       = " << doCleaning << std::endl;
   std::cout << "holdOutFreq          = " << holdOutFreq << std::endl;
   std::cout << "wikiDatasetFile       = " << wikiDatasetFile << std::endl;
   std::cout << "-----------------------------------------------------------" << std::endl;

   // init pseudo-random generator
   std::mt19937 rndGen(rndSeed);

   if (doCleaning == 1)
   {
      std::cout << "Cleaning the dataset ..." << std::endl;
      std::string cleanFile = outputDir + "/raw/" + datasetName + datasetNameSuffix + "_clean.txt";
      std::string stopwWordFile = outputDir + "/raw/stopwords.txt";
      clean(wikiDatasetFile, cleanFile, vocabFile, stopwWordFile, reportEvery, totalNbOfDocs);
      wikiDatasetFile = cleanFile;
      std::cout << "cleaned file path = : " << cleanFile << std::endl;
   }

   wikiDatasetFile = outputDir + "/raw/" + datasetName + datasetNameSuffix + "_clean.txt";
   std::cout << "wikiDatasetFile       = " << wikiDatasetFile << std::endl;
   convertWikiData(
       datasetName,
       vocabFile,
       datasetNameSuffix,
       outputDir,
       rndGen,
       reportEvery,
       wikiDatasetFile,
       holdOutFreq);

   return 0;
}

int main(int argc, const char **argv)
{
   signal(SIGSEGV, signal_handler);
   std::set_terminate(uncaught_exception_handler);
   return runMe(argc, argv);

   // std::string datasetName = "wiki08";
   // std::string datasetNameSuffix = "";
   // std::string vocabFile = "/gammapdb_arrow/data/raw/wiki08_clean_vocab.txt";
   // int numberOfTrainingDocs = 6000000;
   // int numberOfTestDocs = 300000;
   // int rndSeed = 12345;
   // int reportEvery = 10000;
   // std::string wikiDatasetFile = "/gammapdb_arrow/data/raw/wiki08_clean.txt";
   // std::string outputDir = "/gammapdb_arrow/data";

   // // std::string datasetName = "test01";
   // // std::string datasetNameSuffix = "";
   // // std::string vocabFile = "/gammapdb_arrow/data/Test_data/vocab.test01.txt";
   // // int rndSeed = 12345;
   // // int reportEvery = 1;
   // // std::string wikiDatasetFile ="/gammapdb_arrow/data/Test_data/test01raw.txt";
   // // std::string outputDir = "/gammapdb_arrow/data";

   // std::mt19937 rndGen(rndSeed);
   // double holdOutFreq = 0.05;
   // convertWikiData(
   //     datasetName,
   //     vocabFile,
   //     datasetNameSuffix,
   //     outputDir,
   //     rndGen,
   //     reportEvery,
   //     wikiDatasetFile,
   //     holdOutFreq);
}
