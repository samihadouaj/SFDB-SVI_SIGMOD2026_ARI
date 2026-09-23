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

#ifndef _RNDUTILS_H_
#define _RNDUTILS_H_

#include <random>
#include <stdexcept>
#include <set>

// Example: intToStrWithLeadingZeroes(32, 4) should return "0032"
static inline std::string intToStrWithLeadingZeroes(int n, int targetStrLength)
{
   std::string nStr = std::to_string(n);
   int nStrLength = nStr.size();
   return std::string(targetStrLength - std::min(targetStrLength, nStrLength), '0') + nStr;
}


namespace FastRNG
{

// from https://github.com/lemire/testingRNG
static inline uint64_t wyhash64(uint64_t *seed) {  
   *seed += UINT64_C(0x60bee2bee120fc15);
   __uint128_t tmp;
   tmp = (__uint128_t)*seed * UINT64_C(0xa3b195354a39b70d);
   uint64_t m1 = (tmp >> 64) ^ tmp;
   tmp = (__uint128_t)m1 * UINT64_C(0x1b03738712fad5c9);
   uint64_t m2 = (tmp >> 64) ^ tmp;
   return m2;
};

static inline uint32_t wyhash32(uint64_t *seed) {
   return (uint32_t) wyhash64(seed); 
};

static inline double wy2u01(uint64_t r) { 
   const double _wynorm=1.0/(1ull<<52); 
   return (r>>12)*_wynorm;
};

static inline uint32_t bounded_wyhash32(uint64_t *seed, uint32_t range) {
   uint32_t mask = ~uint32_t(0);
   --range;
   mask >>= __builtin_clz(range|1);
   uint32_t x;
   do {
       x = wyhash32(seed) & mask;
   } while (x > range);
   return x;
};

static inline int randomChoiceWyash(const double *cumulativeWeightsArray, int arrayLength, uint64_t *seed)
{
   double x = wy2u01(wyhash64(seed)) * cumulativeWeightsArray[arrayLength-1];
   for (int i=0; i<arrayLength; i++) {
      if (x <= cumulativeWeightsArray[i]) {
         return i;
      }
   }
   throw std::runtime_error("RndUtils.h::FastRNG::randomChoiceWyash()::Unreachable");
};

}


namespace RndUtils
{

bool flipCoin(const double bias, std::mt19937 &rnd); 

int randomChoice(const double *cumulativeWeightsArray, int arrayLength, std::mt19937 &rnd);

int randomChoice(const int *cumulativeWeightsArray, int arrayLength, std::mt19937 &rnd);

void pickRandomInts(std::set<int> &dst, const int &n, const int &minValueIncl, const int &maxValueExcl, std::mt19937 &rnd);

double getRandomDouble(std::mt19937 &rnd);

double getRandomDouble(const double upperBoundExcl, std::mt19937 &rnd);

double getRandomDouble(const double lowerBoundIncl, const double upperBoundExcl, std::mt19937 &rnd);

int getRandomInt(const int upperBoundExcl, std::mt19937 &rnd);

int getRandomInt(const int lowerBoundIncl, const int upperBoundExcl, std::mt19937 &rnd);

}


#endif // _RNDUTILS_H_
