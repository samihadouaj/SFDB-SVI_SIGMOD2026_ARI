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


#include "RndUtils.h"

bool RndUtils::flipCoin(const double bias, std::mt19937 &rnd)
{
   return (RndUtils::getRandomDouble(rnd) < bias);
}

int RndUtils::randomChoice(const double *cumulativeWeightsArray, int arrayLength, std::mt19937 &rnd)
{
   std::uniform_real_distribution<double> dist(0, cumulativeWeightsArray[arrayLength-1]);
   double x = dist(rnd);
   for (int i=0; i<arrayLength; i++) {
      if (x <= cumulativeWeightsArray[i]) {
         return i;
      }
   }
   throw std::runtime_error("RndUtils::randomChoice::Unreachable");
}

int RndUtils::randomChoice(const int *cumulativeWeightsArray, int arrayLength, std::mt19937 &rnd)
{
   std::uniform_int_distribution<> dist(0, cumulativeWeightsArray[arrayLength-1]);
   int x = dist(rnd);
   for (int i=0; i<arrayLength; i++) {
      if (x <= cumulativeWeightsArray[i]) {
         return i;
      }
   }
   throw std::runtime_error("RndUtils::randomChoice::Unreachable");
}

void RndUtils::pickRandomInts(std::set<int> &dst, const int &n, const int &minValueIncl, const int &maxValueExcl, std::mt19937 &rnd)
{
   #ifdef DEBUG
   if (n<0) {
      throw std::out_of_range("n<0");
   }
   if (minValueIncl>=maxValueExcl) {
      throw std::out_of_range("minValueIncl>=maxValueExcl");
   }
   #endif
   std::uniform_int_distribution<> dist(minValueIncl, maxValueExcl-1);
   while (dst.size()<n) 
   {
      int x = dist(rnd);
      if (dst.count(x)==0) {
         dst.insert(x);
      }
   }
}

double RndUtils::getRandomDouble(std::mt19937 &rnd)
{
   std::uniform_real_distribution<> dist(0, 1);
   return dist(rnd);
}

double RndUtils::getRandomDouble(const double upperBoundExcl, std::mt19937 &rnd)
{
   #ifdef DEBUG
   if (upperBoundExcl<0) {
      throw std::out_of_range("upperBoundExcl<0");
   }
   #endif
   std::uniform_real_distribution<> dist(0, upperBoundExcl);
   return dist(rnd);
}

double RndUtils::getRandomDouble(const double lowerBoundIncl, const double upperBoundExcl, std::mt19937 &rnd)
{
   #ifdef DEBUG
   if (upperBoundExcl<0) {
      throw std::out_of_range("upperBoundExcl<0");
   }
   if (lowerBoundIncl>upperBoundExcl) {
      throw std::out_of_range("lowerBoundIncl>upperBoundExcl");
   }
   #endif
   std::uniform_real_distribution<> dist(lowerBoundIncl, upperBoundExcl);
   return dist(rnd);
}

int RndUtils::getRandomInt(const int upperBoundExcl, std::mt19937 &rnd)
{
   #ifdef DEBUG
   if (upperBoundExcl<0) {
      throw std::out_of_range("upperBoundExcl<0");
   }
   #endif
   std::uniform_int_distribution<> dist(0, upperBoundExcl-1);
   return dist(rnd);
}

int RndUtils::getRandomInt(const int lowerBoundIncl, const int upperBoundExcl, std::mt19937 &rnd)
{
   #ifdef DEBUG
   if (upperBoundExcl<0) {
      throw std::out_of_range("upperBoundExcl<0");
   }
   if (lowerBoundIncl>upperBoundExcl) {
      throw std::out_of_range("lowerBoundIncl>upperBoundExcl");
   }
   #endif
   std::uniform_int_distribution<> dist(lowerBoundIncl, upperBoundExcl-1);
   return dist(rnd);  
}



