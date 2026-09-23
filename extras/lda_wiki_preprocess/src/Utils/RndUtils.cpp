
#include "RndUtils.h"

bool RndUtils::flipCoin(const double bias, std::mt19937 &rnd)
{
   return (RndUtils::getRandomDouble(rnd) < bias);
}

double RndUtils::getRandomDouble(std::mt19937 &rnd)
{
   std::uniform_real_distribution<> dist(0, 1);
   return dist(rnd);
}



