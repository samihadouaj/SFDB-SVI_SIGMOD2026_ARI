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

namespace RndUtils
{

bool flipCoin(const double bias, std::mt19937 &rnd); 

double getRandomDouble(std::mt19937 &rnd);

}


#endif // _RNDUTILS_H_
