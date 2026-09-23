#ifndef _BDSETP_H_
#define _BDSETP_H_

// #include "utils/DebugHelper.h"

#include <algorithm> // std::fill
#include <vector>    // std::vector
#include <tuple>
#include <memory>
#include <utility>
#include "utils/Atomwrapper.h"
#include <mutex>
#include "omp.h"
template<class T>
using vec = std::vector<T>; 


template<int NUMVARS, int NUMOUTCOMES, int NUMOBS>
class BDSetP
{
private:
   // std::atomic<int> _obsMap[10];                             // obsId -> valId
   // std::vector<atomwrapper<int>> _obsMap;
   vec<int> _obsMap;
   // std::atomic<int> _varMap[10];                             // obsId -> varId
   // std::vector<atomwrapper<int>> _varMap;
   vec<int> _varMap;
   // std::atomic<int> _obsCounts[10];                         // varId -> obsCount
   std::vector<atomwrapper<int>> _obsCounts;
   // std::atomic<int> _obsCountsByOutcome[10*10];   // (varId, valId) -> obsCount
   std::vector<atomwrapper<int>> _obsCountsByOutcome;
   // std::atomic<double> _hyperParams[10*10];       // (varId, valId) -> dirichlet hyper-parameter ("alpha")
   // std::vector<atomwrapper<double>> _hyperParams;
   vec<double> _hyperParams;

   // std::atomic<double> _hyperParamsSum[10];                 // varId -> sum over all the hyper-parameters
   // std::vector<atomwrapper<double>> _hyperParamsSum;
    vec<double> _hyperParamsSum;
   
public:
   static const int UNOBSERVED = -1;

   BDSetP(const double symmetricPrior=1.0):
      _obsMap(NUMOBS, -1),
      _varMap(NUMOBS, -1),
      _hyperParams((NUMVARS*NUMOUTCOMES), symmetricPrior),
      _hyperParamsSum(NUMVARS, (symmetricPrior*NUMOUTCOMES))
   {
      // for (int i = 0;i<NUMOBS;i++){std::atomic<int> a_i(-1);_obsMap.push_back(a_i);}
      // for (int i = 0;i<NUMOBS;i++){std::atomic<int> a_i(-1);_varMap.push_back(a_i);}
      for (int i = 0;i<NUMVARS;i++){std::atomic<int> a_i(0);_obsCounts.push_back(a_i);}
      for (int i = 0;i<NUMVARS*NUMOUTCOMES;i++){std::atomic<int> a_i(0);_obsCountsByOutcome.push_back(a_i);}
      // for (int i = 0;i<NUMVARS*NUMOUTCOMES;i++){std::atomic<double> a_i(symmetricPrior);_hyperParams.push_back(a_i);}
      // for (int i = 0;i<NUMVARS;i++){std::atomic<double> a_i(symmetricPrior*NUMOUTCOMES);_hyperParamsSum.push_back(a_i);}
   }

   // *****************************
   // getters
   // *****************************

// This is the number of die
   static constexpr int getNumVars() 
   {
      return NUMVARS; 
   }

// This is the number of faces
   static constexpr int getNumOutcomes() 
   {
      return NUMOUTCOMES; 
   }

// This is the number of throws
   static constexpr int getMaxNumObservartions() 
   {
      return NUMOBS; 
   }
// example, for KOS, for the red die, we have BDSet<3269,20,446323>, which means we have 3269 die (one per document)
// each one of these dice has 20 faces (outcomes) and we are going to throw that die 446323 times (the number of words) which is here referred to with NUMOBS.



   int getObservedValue(const int varId, const int obsId) const
   {
      if ((_varMap[obsId])!=varId) {
         return UNOBSERVED;
      } else {
         return _obsMap[obsId];
      }
   }


   int countObservations(const int varId) const
   {
      return _obsCounts[varId].get_a();
   }


   int countObservations(const int  varId, const int valueId) const
   {
      return _obsCountsByOutcome[(varId*NUMOUTCOMES)+valueId].get_a();
   }

// the Dirichlet hyperparam for the given die (varId) and a given face (ValueId)
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




// Still not sure what this does and why we need it!!
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

//    // *****************************
//    // setters
//    // *****************************

   void setSymmetricDirichletHyperParam(const double param)
   {
      //std::cout << "setSymmetricDirichletHyperParam(" << param << ")" << std::endl;

      //std::fill(_hyperParams.begin(), _hyperParams.end(), param);
      for (int i=0; i<NUMVARS*NUMOUTCOMES; i++)
      {
         // std::atomic<double> temp(param);
         // atomwrapper<double> temp((std::atomic<double>(param)));
         // atomwrapper<double> temp2 = temp;
         //  _hyperParams[i].storeWrapper(param);
         _hyperParams[i] = param;
      }

      std::fill(_hyperParamsSum.begin(), _hyperParamsSum.end(), (param*NUMOUTCOMES));
      // for (int i = 0;i<NUMVARS;i++){ _hyperParamsSum[i].storeWrapper(param*NUMOUTCOMES);}

   }

   // void setDirichletHyperParam(const int varId, const int valueId, const double param)
   // {
   //    const int pos = (varId*NUMOUTCOMES)+valueId;
   //    // _hyperParamsSum[varId] -= _hyperParams[pos];
   //    _hyperParamsSum[varId].storeWrapper(_hyperParamsSum[varId].get_a() -  _hyperParams[pos].get_a());
   //    // _hyperParams[pos] = param;
   //     _hyperParams[pos].storeWrapper(param);
   //    // _hyperParamsSum[varId] += param;
   //    _hyperParamsSum[varId].storeWrapper( _hyperParamsSum[varId].get_a()+param);
   // }

   void assertObservation(const int varId, const int obsId, const int valueId)
   {
      #ifdef DEBUG
      //DebugHelper::verifyEqual(_obsMap[obsId], UNOBSERVED, "BayesianDiceSet_MutexObs_SymPrior::assertObservation(): assert _obsMap[obsId]==UNOBSERVED,"s);
      //DebugHelper::verifyEqual(_varMap[obsId], UNOBSERVED, "BayesianDiceSet_MutexObs_SymPrior::assertObservation(): assert _varMap[obsId]==UNOBSERVED,"s);
      #endif

   _varMap[obsId] = (varId);
   _obsCounts[varId].fetch_add(1);

   _obsCountsByOutcome[(varId*NUMOUTCOMES)+valueId].fetch_add(1);
   _obsMap[obsId] = valueId;
   }


   void retractObservation(const int varId, const int obsId)
   {
             

      _varMap[obsId] = UNOBSERVED;
      _obsCounts[varId].fetch_add(-1);
      const int valueId = _obsMap[obsId];
      _obsCountsByOutcome[(varId*NUMOUTCOMES)+valueId].fetch_add(-1);
       

      // int snapshot1;
      //    do 
      //    {
      //       snapshot1 =  _obsCounts[varId].get_a();
      //       if (snapshot1 <= 0)
      //             return;  // There's no need to continue because count is not positive.
      //       else
      //       {
      //          _obsCounts[varId].fetch_add(-1);
      //          _varMap[obsId] = UNOBSERVED;
      //          return;
      //       }
      //    } while (!_obsCounts[varId].compare_exchange_weak(snapshot1, snapshot1 - 1));

      //    _varMap[obsId] =  UNOBSERVED ;


      // const int valueId = _obsMap[obsId];
      // int snapshot2;
      // do {
      //    snapshot2 = _obsCountsByOutcome[(varId*NUMOUTCOMES)+valueId].get_a();
      //    if (snapshot2 <= 0)
      //          return; 
      //    else{
      //       _obsCountsByOutcome[(varId*NUMOUTCOMES)+valueId].fetch_add(-1);
      //       _obsMap[obsId] = UNOBSERVED;
      //       return;
      //    }
      // } while (!_obsCountsByOutcome[(varId*NUMOUTCOMES)+valueId].compare_exchange_weak(snapshot2, snapshot2 - 1));
      
       
      _obsMap[obsId] = UNOBSERVED;




      //     if(_obsCountsByOutcome[(varId*NUMOUTCOMES)+valueId].get_a()<0)
      // {
      //    std::cout<<"--- retractObservation is wrong, _obsCountsByOutcome["<<(varId*NUMOUTCOMES)+valueId<<"] is: "<<_obsCountsByOutcome[(varId*NUMOUTCOMES)+valueId].get_a()<<" valueId is" <<valueId<<" varId is "<<varId<<" and obsId is:  "<<obsId<<std::endl;
      // }
     
      

      

      // _obsMap[obsId] = UNOBSERVED;
  
   }

   bool retractObservation(const int obsId)
   {  
     const int varId = _varMap[obsId];
      if (varId!=UNOBSERVED) {
         // int tid = omp_get_thread_num();
         // printf("The thread %d  trying to retract = %d\n", tid, varId);
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
using BDSetsP = std::tuple<Args...>;




#endif  // _BDSETP_H_