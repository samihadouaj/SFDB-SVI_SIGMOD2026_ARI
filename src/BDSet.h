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

#ifndef _BDSET_H_
#define _BDSET_H_

#include "utils/DebugHelper.h"

#include <algorithm> // std::fill
#include <vector>    // std::vector
#include <tuple>

template<class T>
using vec = std::vector<T>; 

template<int NUMVARS, int NUMOUTCOMES, int NUMOBS>
class BDSet
{
private:
   //int _obsMap[NUMOBS];                             // obsId -> valId
   vec<int> _obsMap;
   //int _varMap[NUMOBS];                             // obsId -> varId
   vec<int> _varMap;
   //int _obsCounts[NUMVARS];                         // varId -> obsCount
   vec<int> _obsCounts;
   //int _obsCountsByOutcome[NUMVARS][NUMOUTCOMES];   // (varId, valId) -> obsCount
   vec<int> _obsCountsByOutcome;
   //double _hyperParams[NUMVARS][NUMOUTCOMES];       // (varId, valId) -> dirichlet hyper-parameter ("alpha")
   vec<double> _hyperParams;
   //double _hyperParamsSum[NUMVARS];                 // varId -> sum over all the hyper-parameters
   vec<double> _hyperParamsSum;

public:
   static const int UNOBSERVED = -1;

   BDSet(const double symmetricPrior=1.0):
      _obsMap(NUMOBS, -1),
      _varMap(NUMOBS, -1),
      _obsCounts(NUMVARS, 0),
      _obsCountsByOutcome((NUMVARS*NUMOUTCOMES), 0),
      _hyperParams((NUMVARS*NUMOUTCOMES), symmetricPrior),
      _hyperParamsSum(NUMVARS, (symmetricPrior*NUMOUTCOMES))
   {}

   // *****************************
   // getters
   // *****************************

   static constexpr int getNumVars() 
   {
      return NUMVARS; 
   }

   static constexpr int getNumOutcomes() 
   {
      return NUMOUTCOMES; 
   }

   static constexpr int getMaxNumObservartions() 
   {
      return NUMOBS; 
   }

   int getObservedValue(const int varId, const int obsId) const
   {
      if (_varMap[obsId]!=varId) {
         return UNOBSERVED;
      } else {
         return _obsMap[obsId];
      }
   }

   int countObservations(const int varId) const
   {
      return _obsCounts[varId];
   }

   int countObservations(const int  varId, const int valueId) const
   {
      return _obsCountsByOutcome[(varId*NUMOUTCOMES)+valueId];
   }

   double getDirichletHyperParam(const int varId, const int valueId) const
   {
      return _hyperParams[(varId*NUMOUTCOMES)+valueId];
   }

   double getDirichletHyperParamSum(const int varId) const
   {
      return _hyperParamsSum[varId];
   }

   double getPosteriorPredictive(const int varId, const int valueId) const
   {
      return (getDirichletHyperParam(varId, valueId) + countObservations(varId, valueId))/(getDirichletHyperParamSum(varId)+countObservations(varId));
   }

   std::vector<std::pair<int,int>> getMostObservedDice(int resultSizeUpperBoundIncl) // returns vector<varId, obsCount>
   {
      std::vector<std::pair<int,int>> tmp;
      for (int varId=0; varId<NUMVARS; varId++) {
         int count = countObservations(varId);
         if (count>0) {
            tmp.push_back(std::make_pair(varId, count));
         }
      }
      if ((resultSizeUpperBoundIncl<0)||(tmp.size()<=resultSizeUpperBoundIncl)) 
      {
         // just sort tmp and return it
         auto cmp = [](const std::pair<int,int> &a, const std::pair<int,int> &b) { return (a.second > b.second); };
         std::sort(tmp.begin(),tmp.end(), cmp);
         return tmp;
      }
      else
      {
         // return only the top 'resultSizeUpperBoundIncl' entries in tmp
         auto cmp = [](const std::pair<int,int> &a, const std::pair<int,int> &b) { return (a.second < b.second); };
         std::make_heap (tmp.begin(),tmp.end(), cmp);
         std::vector<std::pair<int,int>> result;
         for (int i=0; i<resultSizeUpperBoundIncl; i++) 
         {
            result.push_back(tmp.front());
            std::pop_heap(tmp.begin(),tmp.end(), cmp);
            tmp.pop_back();
         }
         return result;
      }
   }

   std::vector<std::pair<int,int>> getMostCommonValues(int varId, int resultSizeUpperBoundIncl) // returns vector<valId, obsCount>
   {
      std::vector<std::pair<int,int>> tmp;
      for (int valId=0; valId<NUMOUTCOMES; valId++) {
         int count = countObservations(varId, valId);
         if (count>0) {
            tmp.push_back(std::make_pair(valId, count));
         }
      }
      if ((resultSizeUpperBoundIncl<0)||(tmp.size()<=resultSizeUpperBoundIncl)) 
      {
         // just sort tmp and return it
         auto cmp = [](const std::pair<int,int> &a, const std::pair<int,int> &b) { return (a.second > b.second); };
         std::sort(tmp.begin(),tmp.end(), cmp);
         return tmp;
      }
      else
      {
         // return only the top 'resultSizeUpperBoundIncl' entries in tmp
         auto cmp = [](const std::pair<int,int> &a, const std::pair<int,int> &b) { return (a.second < b.second); };
         std::make_heap (tmp.begin(),tmp.end(), cmp);
         std::vector<std::pair<int,int>> result;
         for (int i=0; i<resultSizeUpperBoundIncl; i++) 
         {
            result.push_back(tmp.front());
            std::pop_heap(tmp.begin(),tmp.end(), cmp);
            tmp.pop_back();
         }
         return result;
      }
   }

   std::vector<std::vector<int>> summarizePosterior(int numDice, int numValues)
   {
      std::vector<std::vector<int>> result;

      std::vector<std::pair<int,int>> mostObservedDice = getMostObservedDice(numDice);
      for (auto pair : mostObservedDice)
      {
         int varId = pair.first;
         std::vector<std::pair<int,int>> mostCommonValues = getMostCommonValues(varId, numValues);
         std::vector<int> vals;
         for (auto p2 : mostCommonValues)
         {
            int valId = p2.first;
            vals.push_back(valId);
         }
         result.push_back(vals);
      }

      return result;
   }

   // *****************************
   // setters
   // *****************************

   void setSymmetricDirichletHyperParam(const double param)
   {
      //std::cout << "setSymmetricDirichletHyperParam(" << param << ")" << std::endl;

      //std::fill(_hyperParams.begin(), _hyperParams.end(), param);
      for (int i=0; i<NUMVARS*NUMOUTCOMES; i++)
      {
         _hyperParams[i] = param;
      }

      std::fill(_hyperParamsSum.begin(), _hyperParamsSum.end(), (param*NUMOUTCOMES));
   }

   void setDirichletHyperParam(const int varId, const int valueId, const double param)
   {
      const int pos = (varId*NUMOUTCOMES)+valueId;
      _hyperParamsSum[varId] -= _hyperParams[pos];
      _hyperParams[pos] = param;
      _hyperParamsSum[varId] += param;
   }

   void assertObservation(const int varId, const int obsId, const int valueId)
   {
      #ifdef DEBUG
      //DebugHelper::verifyEqual(_obsMap[obsId], UNOBSERVED, "BayesianDiceSet_MutexObs_SymPrior::assertObservation(): assert _obsMap[obsId]==UNOBSERVED,"s);
      //DebugHelper::verifyEqual(_varMap[obsId], UNOBSERVED, "BayesianDiceSet_MutexObs_SymPrior::assertObservation(): assert _varMap[obsId]==UNOBSERVED,"s);
      #endif

     _varMap[obsId] = varId;
     _obsCounts[varId]++;
     _obsCountsByOutcome[(varId*NUMOUTCOMES)+valueId]++;
     _obsMap[obsId] = valueId;
   }

   void retractObservation(const int varId, const int obsId)
   {
      #ifdef DEBUG
      //DebugHelper::verifyNotEqual(_obsMap[obsId], UNOBSERVED, "BayesianDiceSet_MutexObs_SymPrior::assertObservation(): assert _obsMap[obsId]!=UNOBSERVED,"s);
      //DebugHelper::verifyEqual(_varMap[obsId], varId, "BayesianDiceSet_MutexObs_SymPrior::assertObservation(): assert _varMap[obsId]==varId,"s);
      #endif

      _varMap[obsId] = UNOBSERVED;
      _obsCounts[varId]--;
      const int valueId = _obsMap[obsId];
      _obsCountsByOutcome[(varId*NUMOUTCOMES)+valueId]--;
      _obsMap[obsId] = UNOBSERVED;

   }

   bool retractObservation(const int obsId)
   {
      const int varId = _varMap[obsId];

      if (varId!=UNOBSERVED) {
         retractObservation(varId, obsId);
         return true;
      } else {
         return false;
      }
      
   }

   void printModel(int bdSetId)
   {
      for (int obsId=0; obsId<NUMOBS; obsId++)
      {
         const int varId = _varMap[obsId];
         if (varId!=UNOBSERVED) {
            const int valId = _obsMap[obsId];
            std::cout << "SET" << bdSetId << "_D" << varId << "[OBS" << obsId << "] = VAL" << valId << std::endl;
         }
      }
   }
};

template <typename... Args>
using BDSets = std::tuple<Args...>;


#endif  // _BDSET_H_