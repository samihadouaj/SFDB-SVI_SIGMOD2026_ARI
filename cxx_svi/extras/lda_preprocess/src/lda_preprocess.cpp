
#include <chrono>     // std::chrono
#include <exception>  // std::set_terminate()
#include <fstream>
#include <iostream>
#include <random>
#include <signal.h>   // signal()
#include <string>
#include <vector>

#include "Utils/argparse.hpp"
#include "Utils/DebugHelper.h"
#include "Utils/TimeThis.h"
#include "Utils/RndUtils.h"
#include "DocWordFileParser.h"
#include "Vocabulary.h"

std::string getCurrentTimeStampAsString()
{
   std::time_t currentTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
   std::string timeStr(std::ctime(&currentTime));
   return timeStr.substr(0, timeStr.size()-1);
}

void create_topic_def_csv_file(int numTopics, int vocabSize, std::string &csvFilePath)
{
   std::ofstream topicsDefCsvFile(csvFilePath);
   if (!topicsDefCsvFile.is_open())
   {
      std::cout << "Unable to create file " << csvFilePath << std::endl;
      throw std::runtime_error("Cannot create output file");
   }
   topicsDefCsvFile << "topicId,wordId" << std::endl;
   for (int topicId=0; topicId<numTopics; topicId++)
   {
      for (int wordId=0; wordId<vocabSize; wordId++)
      {
         topicsDefCsvFile << topicId << "," << wordId << std::endl;
      }
   }
   if (topicsDefCsvFile.is_open())
   {
      topicsDefCsvFile.close();
   }
}

void create_doc_composition_csv_file(int firstDocId, int lastDocId, int numTopics, std::string &csvFilePath)
{
   std::ofstream docDefCsvFile(csvFilePath);
   if (!docDefCsvFile.is_open())
   {
      std::cout << "Unable to create file " << csvFilePath << std::endl;
      throw std::runtime_error("Cannot create output file");
   }
   docDefCsvFile << "docId,topicId" << std::endl;
   for (int docId=firstDocId; docId<=lastDocId; docId++)
   {
      for (int topicId=0; topicId<numTopics; topicId++)
      {
         docDefCsvFile << docId << "," << topicId << std::endl;
      }
   }
   if (docDefCsvFile.is_open())
   {
      docDefCsvFile.close();
   }
}

void convert_uci_dataset(
   std::string& datasetName,
   std::string& datasetNameSuffix,
   std::string& docwordFilePath,
   std::string& vocabFilePath,
   std::string& outputDir,
   std::string& outputFormat,
   double holdOutFreq,
   int numTopics,
   std::mt19937 &rndGen,
   int reportEvery
)
{
   // read the vocab file
   Vocabulary vocabulary(vocabFilePath);
   

   // read the header
   DocWordFileParser parser(docwordFilePath);
   int a[3];
   if (!parser.readThreeIntegers(a))
   {
      throw std::runtime_error("Cannot read DocWord file header");
   }
   int totalNumberOfDocs = a[0], numberOfWords = a[1], totalNumberOfRecords = a[2];

   if (vocabulary.getSize() != numberOfWords) {
      throw std::runtime_error("vocabulary.getSize() != numberOfWords");
   }

   // copy the whole file into an array (put test documents at the tail)
   int numberOfTestDocs = 0;
   int numberOfTrainingDocs = 0;

   std::unique_ptr<int[]> buf1Ptr = std::make_unique<int[]>(3 * totalNumberOfRecords);
   {
      {
         std::time_t currentTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
         std::string timeStr(std::ctime(&currentTime));
         std::cout << "[" << timeStr.substr(0, timeStr.size()-1) << "] Reading UCI dataset ..." << std::endl;
      }

      int *fwCursor = buf1Ptr.get();
      int *bwCursor = buf1Ptr.get()+(3 * (totalNumberOfRecords-1));

      int copiedRecords = 0;
      int currentTrainingDocId = -1;
      int currentTestDocId = totalNumberOfDocs;
      int lastSeenRecordDocId = -1;
      int lastSeenTestDocId = -1;

      while (copiedRecords<totalNumberOfRecords)
      {
         if (!parser.readThreeIntegers(fwCursor)) 
         {
            throw std::runtime_error("Cannot read DocWord file entry");
         }
         copiedRecords++;
         fwCursor[0]--; // make RecordDocIDs zero-based 
         fwCursor[1]--; // make WordIDs zero-based   

         if (fwCursor[0]!=lastSeenRecordDocId)
         {
            // previously unseen doc
            lastSeenRecordDocId = fwCursor[0];
            if (RndUtils::flipCoin(holdOutFreq, rndGen))
            {
               // This new document belongs to the test set, copy the record to the tail of the buffer
               numberOfTestDocs++;
               currentTestDocId--;
               lastSeenTestDocId = fwCursor[0];
               bwCursor[0] = currentTestDocId;
               bwCursor[1] = fwCursor[1];
               bwCursor[2] = fwCursor[2];
               bwCursor -= 3;
            }
            else
            {
               // This new document belongs to the training set, keep the record at the head of the buffer
               currentTrainingDocId++;
               numberOfTrainingDocs++;
               fwCursor[0] = currentTrainingDocId;
               fwCursor += 3;
            }
         }
         else
         {
            // previously seen doc
            if (fwCursor[0] == lastSeenTestDocId) 
            {
               // This record refers to a test document, copy it to the tail of the buffer
               bwCursor[0] = currentTestDocId;
               bwCursor[1] = fwCursor[1];
               bwCursor[2] = fwCursor[2];
               bwCursor -= 3;
            } 
            else
            {
               // This record refers to a training document, keep it at the head of the buffer
               fwCursor[0] = currentTrainingDocId;
               fwCursor += 3;
            }
         }
      }
   }

   //int numberOfEmptyDocs = totalNumberOfDocs - (numberOfTrainingDocs+numberOfTestDocs);
   //int numberOfValidDocs = numberOfTrainingDocs+numberOfTestDocs;
   int firstTestDocId = totalNumberOfDocs - numberOfTestDocs;
   /*

   _numberOfTrainingDocs = numberOfTrainingDocs;
   _numberOfTestDocs = numberOfTestDocs;
   _numberOfEmptyDocs = totalNumberOfDocs - (numberOfTrainingDocs+numberOfTestDocs);
   _numberOfValidDocs = numberOfTrainingDocs+numberOfTestDocs;
   */

   if (outputFormat=="CSV" || outputFormat=="CSV2"  || outputFormat=="ALL")
   {
      std::cout << "[" << getCurrentTimeStampAsString() << "] Converting to " << ((outputFormat=="CSV") ? "CSV" : "CSV2") << " format ..." << std::endl;
      
      // create output directories (unix-only)
      std::string trainingSetDirPath(outputDir+"/"+datasetName+datasetNameSuffix+"_train/"+((outputFormat=="CSV") ? "csv" : "csv2"));
      std::string testSetDirPath(outputDir+"/"+datasetName+datasetNameSuffix+"_test/"+((outputFormat=="CSV") ? "csv" : "csv2"));
      std::string cmd1("mkdir -p "+trainingSetDirPath);
      std::string cmd2("mkdir -p "+testSetDirPath);
      system(&*cmd1.begin());
      system(&*cmd2.begin());

      // convert the vocab file
      std::string vocabFile1 = trainingSetDirPath+"/"+datasetName+datasetNameSuffix+"_vocab.csv";
      std::string vocabFile2 = testSetDirPath+"/"+datasetName+datasetNameSuffix+"_vocab.csv";
      vocabulary.saveAsCsvFile(vocabFile1);
      vocabulary.saveAsCsvFile(vocabFile2);

      // define the delta-tuples for topics & documents
      // format CSV2 will skip this step
      if (outputFormat=="CSV") {
         std::string topicFile1 = trainingSetDirPath+"/"+datasetName+datasetNameSuffix+"_topics.csv";
         create_topic_def_csv_file(numTopics, vocabulary.getSize(), topicFile1);
         
         std::string docFile1 = trainingSetDirPath+"/"+datasetName+datasetNameSuffix+"_docs.csv";
         create_doc_composition_csv_file(0, numberOfTrainingDocs-1, numTopics, docFile1);
      }

      // create output files
      std::string trainingCsvFilePath = trainingSetDirPath+"/"+datasetName+datasetNameSuffix+"_train.csv";
      std::string testCsvFilePath = testSetDirPath+"/"+datasetName+datasetNameSuffix+"_test.csv";
      std::ofstream trainingDataFile(trainingCsvFilePath);
      std::ofstream testDataFile(testCsvFilePath);
      if (!trainingDataFile.is_open())
      {
         std::cout << "[" << getCurrentTimeStampAsString() << "] Unable to create file " << trainingCsvFilePath << std::endl;
         throw std::runtime_error("Cannot create output file");
      }
      if (!testDataFile.is_open())
      {
         std::cout << "[" << getCurrentTimeStampAsString() << "] Unable to create file " << testCsvFilePath << std::endl;
         throw std::runtime_error("Cannot create output file");
      }

      // print headers
      trainingDataFile << "docId,pos,wordId" << std::endl;
      testDataFile << "docId,pos,wordId" << std::endl;

      // allocate map for Mallet wordcodes
      std::vector<int> MalletVocab;
      MalletVocab.reserve(vocabulary.getSize());
      int nextMalletWordCode = 0;
      for (int i=0; i<vocabulary.getSize(); i++) {
         MalletVocab[i]=-1;
      }

      // iterate over all the docword data
      {
         int previousDocId=-1;
         int pos=-1;
         const int *start = buf1Ptr.get();
         const int *end = start+(3 * totalNumberOfRecords);
         for (auto i=start; i<end; i+=3)
         {
            int docId = i[0];
            int wordId = i[1];
            int wcount = i[2];

            if (docId!=previousDocId)
            {
               previousDocId=docId;
               if ((outputFormat=="CSV")||(pos==-1))  {
                  pos=0; // format "CSV2" uses globally-unique position identifiers
               }
               
               if ((docId % reportEvery)==0)
               {
                  std::cout << "[" << getCurrentTimeStampAsString() << "] " << docId << " documents processed ..." << std::endl;
               }
            }
            for (int j=pos; j<(pos+wcount); ++j)
            {
               if (docId<numberOfTrainingDocs)
               {
                  // this document belongs to the training set
                  trainingDataFile << docId << "," << j << "," << wordId << std::endl;

                  // update Mallet vocabulary
                  if (MalletVocab[wordId]==-1)
                  {
                     MalletVocab[wordId] = nextMalletWordCode;
                     nextMalletWordCode++;
                  }

               }
               else if (docId>=firstTestDocId)
               {
                  // this document belongs to the test set
                  testDataFile << docId << "," << j << "," << wordId << std::endl;
               }
            }
            pos += wcount;
         }
      }

      // save Mallet vocabulary
      std::string malletVocabFilePath = trainingSetDirPath+"/"+datasetName+datasetNameSuffix+"_vocab_mallet.csv";
      {
         std::ofstream MalletVocabFile(malletVocabFilePath);
         MalletVocabFile << "malletWordCode wordStr" << std::endl;
         for (int i=0; i<vocabulary.getSize(); i++) {
            //std::cout << "MalletVocab[" << i << "] = " << MalletVocab[i] << std::endl;
            MalletVocabFile << MalletVocab[i] << " " << vocabulary.getWordByCode(i) << std::endl;
         }
         MalletVocabFile.close();
      }
      

      // close all open files
      if (trainingDataFile.is_open())
      {
         trainingDataFile.close();
      }
      if (testDataFile.is_open())
      {
         testDataFile.close();
      }
   }

   if (outputFormat=="PLAIN" || outputFormat=="ALL")
   {
      std::string trainingSetDirPath(outputDir+"/"+datasetName+datasetNameSuffix+"_train/plain");
      std::string testSetDirPath(outputDir+"/"+datasetName+datasetNameSuffix+"_test/plain");
      // &*str.begin()
      std::string cmd1("mkdir -p "+trainingSetDirPath);
      std::string cmd2("mkdir -p "+testSetDirPath);
      system(&*cmd1.begin());
      system(&*cmd2.begin());

      // iterate over all the docword data
      {
         std::ofstream docFile;
         int previousDocId=-1;
         int pos=-1;
         const int *start = buf1Ptr.get();
         const int *end = start+(3 * totalNumberOfRecords);
         for (auto i=start; i<end; i+=3)
         {
            int docId = i[0];
            int wordId = i[1];
            int wcount = i[2];
            if (docId!=previousDocId)
            {
               previousDocId=docId;
               pos=0;
               if ((docId % reportEvery)==0)
               {
                  std::cout << "[" << getCurrentTimeStampAsString() << "] " << docId << " documents processed ..." << std::endl;
               }
               if (docFile.is_open())
               {
                  docFile.close();
               }

               if (docId<numberOfTrainingDocs)
               {
                  // this document belongs to the training set
                  std::string docFilePath = trainingSetDirPath+"/"+"doc"+intToStrWithLeadingZeroes(docId, 12)+".txt";
                  docFile.open(docFilePath);
               }
               else if (docId>=firstTestDocId)
               {
                  // this document belongs to the test set
                  std::string docFilePath = testSetDirPath+"/"+"doc"+intToStrWithLeadingZeroes(docId, 12)+".txt";
                  docFile.open(docFilePath);
               } 
            }
            for (int j=pos; j<(pos+wcount); ++j)
            {
               docFile << vocabulary.getWordByCode(wordId) << " "; //<< std::endl;
            }
            pos += wcount;
         } // for
         if (docFile.is_open())
         {
            docFile.close();
         }
      }
   }

   if (outputFormat=="TSV" || outputFormat=="ALL")
   {
      std::string trainingSetDirPath(outputDir+"/"+datasetName+datasetNameSuffix+"_train/tsv");
      std::string testSetDirPath(outputDir+"/"+datasetName+datasetNameSuffix+"_test/tsv");
      // &*str.begin()
      std::string cmd1("mkdir -p "+trainingSetDirPath);
      std::string cmd2("mkdir -p "+testSetDirPath);
      system(&*cmd1.begin());
      system(&*cmd2.begin());

      // iterate over all the docword data
      {
         std::ofstream trainingDataFile;
         std::string trainingDataFilePath = trainingSetDirPath+"/training.tsv";
         trainingDataFile.open(trainingDataFilePath);

         std::ofstream testDataFile;
         std::string testDataFilePath = testSetDirPath+"/test.tsv";
         testDataFile.open(testDataFilePath);

         int previousDocId=-1;
         int pos=-1;
         const int *start = buf1Ptr.get();
         const int *end = start+(3 * totalNumberOfRecords);
         for (auto i=start; i<end; i+=3)
         {
            int docId = i[0];
            int wordId = i[1];
            int wcount = i[2];
            if (docId!=previousDocId)
            {
               if (previousDocId<numberOfTrainingDocs)
               {
                  // previous document belongs to the training set
                  trainingDataFile << std::endl;
               }
               else if (previousDocId>=firstTestDocId)
               {
                  // previous document belongs to the test set
                  testDataFile << std::endl;
               }

               previousDocId=docId;
               pos=0;
               if ((docId % reportEvery)==0)
               {
                  std::cout << "[" << getCurrentTimeStampAsString() << "] " << docId << " documents processed ..." << std::endl;
               }

               if (docId<numberOfTrainingDocs)
               {
                  // this document belongs to the training set
                  trainingDataFile << "doc" << intToStrWithLeadingZeroes(docId, 12) << '\t' << "x  "; 
               } 
               else if (docId>=firstTestDocId) 
               {
                  // this document belongs to the test set
                  testDataFile << "doc" << intToStrWithLeadingZeroes(docId, 12) << '\t' << "x  "; 
               }
            }
            if (docId<numberOfTrainingDocs)
            {
               // this document belongs to the training set
               for (int j=pos; j<(pos+wcount); ++j)
               {
                  trainingDataFile << vocabulary.getWordByCode(wordId) << " "; //<< std::endl;
               }
               pos += wcount;
            }
            else if (docId>=firstTestDocId) 
            {
               // this document belongs to the test set
               for (int j=pos; j<(pos+wcount); ++j)
               {
                  testDataFile << vocabulary.getWordByCode(wordId) << " "; //<< std::endl;
               }
               pos += wcount;
            }
         } // for

         if (trainingDataFile.is_open())
         {
            trainingDataFile.close();
         }

         if (testDataFile.is_open())
         {
            testDataFile.close();
         }
      }
   }

}

int runMe(int argc, const char **argv)
{
   // default arguments
   std::string datasetName="NYTIMES";
   std::string datasetNameSuffix="";
   std::string outputFormat="ALL";
   double holdOutFreq = 0.05; // 5%
   int numTopics = 20;
   int rndSeed = 12345;
   int reportEvery = 5000;
   std::string outputDir = "/scratch/lda_datasets";
   std::string uciDatasetsDir = "/scratch/uci_datasets_raw";

   // declare arguments
   const bool isOptionalArg = true;
   ArgumentParser argsParser;
   // declare arguments
   argsParser.addArgument("-d", "--dataset", 1, isOptionalArg);
   argsParser.addArgument("-s", "--datasetNameSuffix", 1, isOptionalArg);
   argsParser.addArgument("-f", "--format", 1, isOptionalArg);
   argsParser.addArgument("-h", "--holdOutFreq", 1, isOptionalArg);
   argsParser.addArgument("--numTopics", 1, isOptionalArg);
   argsParser.addArgument("-r", "--rndSeed", 1, isOptionalArg);
   argsParser.addArgument("--reportEvery", 1, isOptionalArg);
   // mandatory args
   argsParser.addArgument("-o", "--outputDir", 1, !isOptionalArg);
   argsParser.addArgument("--uciDatasetsDir", 1, !isOptionalArg);
   
   // parse arguments
   argsParser.parse(argc, argv);

   // read optional arguments
   if (argsParser.count("dataset")) {
      datasetName = argsParser.retrieve<std::string>("dataset");
   }
   if (argsParser.count("datasetNameSuffix")) {
      datasetNameSuffix = argsParser.retrieve<std::string>("datasetNameSuffix");
   }
   if (argsParser.count("format")) {
      outputFormat = argsParser.retrieve<std::string>("format");
   }
   if (argsParser.count("holdOutFreq")) {
      holdOutFreq = std::stod(argsParser.retrieve<std::string>("holdOutFreq"));
   }
   if (argsParser.count("numTopics")) {
      numTopics = std::stod(argsParser.retrieve<std::string>("numTopics"));
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
   if (argsParser.count("uciDatasetsDir")) {
      uciDatasetsDir = argsParser.retrieve<std::string>("uciDatasetsDir");
   }

   // print arguments
   std::cout << "-----------------------------------------------------------" << std::endl;
   std::cout << "LDA PREPROCESS" << std::endl;
   std::cout << "-----------------------------------------------------------" << std::endl;
   std::cout << "current time         = " << getCurrentTimeStampAsString() << std::endl;
   std::cout << "dataset              = " << datasetName << datasetNameSuffix << std::endl;
   std::cout << "outputFormat         = " << outputFormat << std::endl;
   std::cout << "holdOutFreq          = " << holdOutFreq << std::endl;
   std::cout << "numTopics            = " << numTopics << std::endl;
   std::cout << "rndSeed              = " << rndSeed << std::endl;
   std::cout << "reportEvery          = " << reportEvery << std::endl;
   std::cout << "outputDir            = " << outputDir << std::endl;
   std::cout << "uciDatasetsDir       = " << uciDatasetsDir << std::endl;
   std::cout << "-----------------------------------------------------------" << std::endl;

   // init UCI datasets paths
   std::string kosDocwordFile = uciDatasetsDir+"/docword.kos.txt";
   std::string kosVocabFile = uciDatasetsDir+"/vocab.kos.txt";
   std::string nytimesDocwordFile = uciDatasetsDir+"/docword.nytimes.txt";
   std::string nytimesVocabFile = uciDatasetsDir+"/vocab.nytimes.txt";
   std::string pubmedDocwordFile = uciDatasetsDir+"/docword.pubmed.txt";
   std::string pubmedVocabFile = uciDatasetsDir+"/vocab.pubmed.txt";

   // init pseudo-random generator
   std::mt19937 rndGen(rndSeed);
   
   if (datasetName=="KOS")
   {
      convert_uci_dataset(
         datasetName, 
         datasetNameSuffix,
         kosDocwordFile, 
         kosVocabFile,
         outputDir,
         outputFormat,
         holdOutFreq,
         numTopics,
         rndGen,
         reportEvery);
   }
   else if (datasetName=="NYTIMES")
   {
      convert_uci_dataset(
         datasetName, 
         datasetNameSuffix,
         nytimesDocwordFile, 
         nytimesVocabFile,
         outputDir,
         outputFormat,
         holdOutFreq,
         numTopics,
         rndGen,
         reportEvery);
   }
   else if (datasetName=="PUBMED")
   {
      convert_uci_dataset(
         datasetName, 
         datasetNameSuffix,
         pubmedDocwordFile, 
         pubmedVocabFile,
         outputDir,
         outputFormat,
         holdOutFreq,
         numTopics,
         rndGen,
         reportEvery);
   }

   return 0;
}

int main(int argc, const char **argv)
{
   signal(SIGSEGV, signal_handler);
   std::set_terminate(uncaught_exception_handler);
   return runMe(argc, argv);
}
