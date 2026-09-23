#include "utils/DebugHelper.h"
#include "utils/RndUtils.h"


#include <chrono>     // std::chrono
#include <exception>  // std::set_terminate()
#include <signal.h>   // signal()


#include <fstream>
#include <iostream>
#include <string>

#include <stdlib.h> 
#include <stdio.h> 
#include <linux/limits.h>
#include <sys/stat.h>

int runMe(int argc, const char **argv);



template <int NUMROWS, int NUMCOLS>
void printMatrix(double m[NUMROWS][NUMCOLS])
{
   for (int row=0; row<NUMROWS; row++)
   {
      for (int col=0; col<NUMCOLS; col++)
      {
         std::cout << m[row][col] << "   ";
      }
      std::cout << std::endl;
   }
};

template <int NUMELEMENTS>
void generateCumulativeVector(double m[NUMELEMENTS])
{
   for (int i=1; i<NUMELEMENTS; i++)
   {
      m[i] += m[i-1];
   }
};

template <int NUMROWS, int NUMCOLS>
void generateCumulativeMatrix(double m[NUMROWS][NUMCOLS])
{
   for (int row=0; row<NUMROWS; row++)
   {
      for (int col=1; col<NUMCOLS; col++)
      {
         m[row][col] += m[row][col-1];
      }
   }
};

void printSeq(int *seq, int seqLength, std::ofstream &outputFile)
{
   for (int i=0; i<seqLength; i++)
   {
      char c = (char) (seq[i]+65);
      outputFile << c; 
   }
   outputFile << std::endl;
}

template <int NUMSTATES, int NUMSYMBOLS>
class hmmModel
{
public:
   double iMatrixCumulative[NUMSTATES];
   double tMatrixCumulative[NUMSTATES][NUMSTATES];
   double eMatrixCumulative[NUMSTATES][NUMSYMBOLS]; 

   hmmModel(const double (&iMatrix)[NUMSTATES], const double (&tMatrix)[NUMSTATES][NUMSTATES], const double (&eMatrix)[NUMSTATES][NUMSYMBOLS])
   {
      for (int i=0; i<NUMSTATES; i++)
      {
         iMatrixCumulative[i] = iMatrix[i];
      }
      generateCumulativeVector<NUMSTATES>(iMatrixCumulative);

      for (int i=0; i<NUMSTATES; i++)
      {
         for (int j=0; j<NUMSTATES; j++)
         {
            tMatrixCumulative[i][j] = tMatrix[i][j];
         }
      }
      generateCumulativeMatrix<NUMSTATES,NUMSTATES>(tMatrixCumulative);

      for (int i=0; i<NUMSTATES; i++)
      {
         for (int j=0; j<NUMSYMBOLS; j++)
         {
            eMatrixCumulative[i][j] = eMatrix[i][j];
         }
      }
      generateCumulativeMatrix<NUMSTATES,NUMSYMBOLS>(eMatrixCumulative);
   }
};

template <int NUMSTATES, int NUMSYMBOLS>
void generateHmmSequence(
   //double iMatrixCumulative[NUMSTATES],
   //double tMatrixCumulative[NUMSTATES][NUMSTATES], 
   //double eMatrixCumulative[NUMSTATES][NUMSYMBOLS], 
   hmmModel<NUMSTATES, NUMSYMBOLS> &model,
   int seqLength,
   int *target,
   uint64_t *seed)
{
   int currentHState = FastRNG::randomChoiceWyash( (double*) (&(model.iMatrixCumulative[0])), NUMSTATES, seed);
   for (int pos=0; pos<seqLength; pos++)
   {
      int symb = FastRNG::randomChoiceWyash( (double*) (&(model.eMatrixCumulative[currentHState][0])), NUMSYMBOLS, seed);
      target[pos] = symb;
      currentHState = FastRNG::randomChoiceWyash(  (double*) (&(model.tMatrixCumulative[currentHState][0])),  NUMSYMBOLS, seed);
   }
};

void generateSynDataset01()
{
   uint64_t rndSeed = 723445;
   const int seqLength = 10;
   int seq[seqLength];
   int numTrainingSamples = 1000;
   int numTestSamples = 100;

   // create directory structure
   char resolved_path[PATH_MAX]; 
   realpath("../../../data/HmmSyn01_train", resolved_path); 
   mkdir(resolved_path,0777);
   realpath("../../../data/HmmSyn01_train/txt", resolved_path); 
   mkdir(resolved_path,0777);
   realpath("../../../data/HmmSyn01_test", resolved_path); 
   mkdir(resolved_path,0777);
   realpath("../../../data/HmmSyn01_test/txt", resolved_path); 
   mkdir(resolved_path,0777);

   hmmModel<3,3> model1(
   {1, 98, 1}, 
   {{10, 75, 15}, {75, 10, 15}, {1, 1, 98}}, 
   {{1000, 1, 1}, {1, 1000, 1}, {1, 1, 1000}}
   );

   {
      std::ofstream outputFile("../../../data/HmmSyn01_train/txt/class01.txt");
      for (int i=0; i<numTrainingSamples; i++)
      {
         generateHmmSequence<3,3>(model1, seqLength, &(seq[0]), &rndSeed);
         printSeq(&(seq[0]), seqLength, outputFile);
      }
      if (outputFile.is_open()) {
         outputFile.close();
      }
   }

   {
      std::ofstream outputFile("../../../data/HmmSyn01_test/txt/class01.txt");
      for (int i=0; i<numTestSamples; i++)
      {
         generateHmmSequence<3,3>(model1, seqLength, &(seq[0]), &rndSeed);
         printSeq(&(seq[0]), seqLength, outputFile);
      }
      if (outputFile.is_open()) {
         outputFile.close();
      }
   }

   hmmModel<3,3> model2(
   {1, 1, 1}, 
   {{10, 85, 5}, {5, 70, 25}, {85, 10, 5}}, 
   {{1000, 1, 1}, {1, 1000, 1}, {1, 1, 1000}}
   );

   {
      std::ofstream outputFile("../../../data/HmmSyn01_train/txt/class02.txt");
      for (int i=0; i<numTrainingSamples; i++)
      {
         generateHmmSequence<3,3>(model2, seqLength, &(seq[0]), &rndSeed);
         printSeq(&(seq[0]), seqLength, outputFile);
      }
      if (outputFile.is_open()) {
         outputFile.close();
      }
   }

   {
      std::ofstream outputFile("../../../data/HmmSyn01_test/txt/class02.txt");
      for (int i=0; i<numTestSamples; i++)
      {
         generateHmmSequence<3,3>(model2, seqLength, &(seq[0]), &rndSeed);
         printSeq(&(seq[0]), seqLength, outputFile);
      }
      if (outputFile.is_open()) {
         outputFile.close();
      }
   }


   hmmModel<3,3> model3(
   {1, 1, 98}, 
   {{10, 5, 85}, {85, 10, 5}, {5, 85, 10}}, 
   {{1000, 1, 1}, {1, 1000, 1}, {1, 1, 1000}}
   );

   {
      std::ofstream outputFile("../../../data/HmmSyn01_train/txt/class03.txt");
      for (int i=0; i<numTrainingSamples; i++)
      {
         generateHmmSequence<3,3>(model3, seqLength, &(seq[0]), &rndSeed);
         printSeq(&(seq[0]), seqLength, outputFile);
      }
      if (outputFile.is_open()) {
         outputFile.close();
      }
   }

   {
      std::ofstream outputFile("../../../data/HmmSyn01_test/txt/class03.txt");
      for (int i=0; i<numTestSamples; i++)
      {
         generateHmmSequence<3,3>(model3, seqLength, &(seq[0]), &rndSeed);
         printSeq(&(seq[0]), seqLength, outputFile);
      }
      if (outputFile.is_open()) {
         outputFile.close();
      }
   }


   
}

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

   std::cout << "hmmgen " << getCurrentTimeStampAsString() << std::endl;
   
   generateSynDataset01();
   std::cout << std::endl;


   return 0;
};
