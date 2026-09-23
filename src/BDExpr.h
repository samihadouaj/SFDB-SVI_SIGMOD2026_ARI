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

#ifndef _BDEXPR_H_
#define _BDEXPR_H_

#include "BDSet.h"
#include "SamplerExecutionContext.h"
#include "utils/DebugHelper.h"
#include "utils/RndUtils.h"


#include <tuple>

template <int N>
static inline int rndChoice(double (&probs)[N], uint64_t *rndSeed)
{

   for (int i=1; i<N; i++) {
      probs[i] += probs[i-1];
   }
   double x = FastRNG::wy2u01(FastRNG::wyhash64(rndSeed)) * probs[N-1];
   for (int i=0; i<N; i++) {
      if (x <= probs[i]) {
         return i;
      }
   }

   throw std::runtime_error("BDExpr.h::rndChoice()::Unreachable");
};

template<class L, class R>
class EXOR
{
public:

   template<typename M, typename E>
   static double ppSat(M &model, SamplerExecutionContext<E> &context) 
   { 
      return L::ppSat(model, context)+R::ppSat(model, context);
   };

   template<typename M, typename E>
   static bool retract(M &model, SamplerExecutionContext<E> &context)
   {
      if (!L::retract(model, context))
      {
         return R::retract(model, context);
      }
      return true;
   }

   
   template<typename M, typename E>
   static void sampleSat(M &model, SamplerExecutionContext<E> &context) 
   {
      double probs[2] = { L::ppSat(model, context), R::ppSat(model, context) };
      int choice = rndChoice<2>(probs, context.rndSeed);
      switch (choice)
      {
         case 0: L::sampleSat(model, context); break;
         case 1: R::sampleSat(model, context); break;
      }
   };
   
};

template<class L, class R>
class IAND
{
public:

   template<typename M, typename E>
   static double ppSat(M &model, SamplerExecutionContext<E> &context) 
   { 
      return L::ppSat(model, context)*R::ppSat(model, context);
   };

   template<typename M, typename E>
   static bool retract(M &model, SamplerExecutionContext<E> &context)
   {
      bool res = false;
      bool tmp = false;
      tmp = L::retract(model, context); res = (res||tmp); 
      tmp = R::retract(model, context); res = (res||tmp); 
      return (res);
   };

   template<typename M, typename E>
   static void sampleSat(M &model, SamplerExecutionContext<E> &context) 
   {
      L::sampleSat(model, context);
      R::sampleSat(model, context);
   };
};

template<int VGID, int VARID,int OBSID,int VALID> 
class CONST
{
public:

   template<typename M, typename E>
   static double ppSat(M &model, SamplerExecutionContext<E> &context) 
   { 
      SUPPRESS_UNUSED_PARAM_WARNING(context);
      return std::get<VGID>(model).getPosteriorPredictive(VARID, VALID);
   };

   template<typename M, typename E>
   static bool retract(M &model, SamplerExecutionContext<E> &context)
   {
      SUPPRESS_UNUSED_PARAM_WARNING(context);
      //std::cout << "Retract(OBS" << OBSID << ")" << std::endl;
      return std::get<VGID>(model).retractObservation(OBSID);
   }

   template<typename M, typename E>
   static void sampleSat(M &model, SamplerExecutionContext<E> &context) 
   {
      SUPPRESS_UNUSED_PARAM_WARNING(context);
      std::get<VGID>(model).assertObservation(VARID, VARID, VALID);
   };
};

template<int VGID, int VARID_ARG,int OBSID_ARG,int VALID_ARG> 
class VAR
{
public:

   template<typename M, typename E>
   static double ppSat(M &model, SamplerExecutionContext<E> &context) 
   { 
      return std::get<VGID>(model).getPosteriorPredictive(context.args[VARID_ARG], context.args[VALID_ARG]);
   };

   template<typename M, typename E>
   static bool retract(M &model, SamplerExecutionContext<E> &context)
   {
      //std::cout << "Retract(OBS" << args[OBSID_ARG] << ")" << std::endl;
      return std::get<VGID>(model).retractObservation(context.args[OBSID_ARG]);
   }

   template<typename M, typename E>
   static void sampleSat(M &model, SamplerExecutionContext<E> &context) 
   {
      return std::get<VGID>(model).assertObservation(context.args[VARID_ARG], context.args[OBSID_ARG], context.args[VALID_ARG]);
   };
};

template<int VGID, int VARID_ARG,int OBSID_ARG,int VALID> 
class VAR2
{
public:

   template<typename M, typename E>
   static double ppSat(M &model, SamplerExecutionContext<E> &context) 
   { 
      return std::get<VGID>(model).getPosteriorPredictive(context.args[VARID_ARG], VALID);
   };

   template<typename M, typename E>
   static bool retract(M &model, SamplerExecutionContext<E> &context)
   {
      //std::cout << "Retract(OBS" << args[OBSID_ARG] << ")" << std::endl;
      return std::get<VGID>(model).retractObservation(context.args[OBSID_ARG]);
   }

   template<typename M, typename E>
   static void sampleSat(M &model, SamplerExecutionContext<E> &context) 
   {
      //std::cout << "SAT(SET" << VGID << "_D" << args[VARID_ARG] << "[OBS" <<  args[OBSID_ARG] << "] = " << VALID << ")" << std::endl;
      return std::get<VGID>(model).assertObservation(context.args[VARID_ARG], context.args[OBSID_ARG], VALID);
   };
};

template<int VGID, int VARID, int OBSID_ARG, int VALID_ARG> 
class VAR3
{
public:

   template<typename M, typename E>
   static double ppSat(M &model, SamplerExecutionContext<E> &context) 
   { 
      return std::get<VGID>(model).getPosteriorPredictive(VARID, context.args[VALID_ARG]);
   };

   template<typename M, typename E>
   static bool retract(M &model, SamplerExecutionContext<E> &context)
   {
      //std::cout << "VAR3::Retract(OBS" << args[OBSID_ARG] << ")" << std::endl;
      return std::get<VGID>(model).retractObservation(context.args[OBSID_ARG]);
   }

   template<typename M, typename E>
   static void sampleSat(M &model, SamplerExecutionContext<E> &context) 
   {
      //std::cout << "SAT(SET" << VGID << "_D" << VARID << "[OBS" <<  args[OBSID_ARG] << "] = " << args[VALID_ARG] << ")" << std::endl;
      return std::get<VGID>(model).assertObservation(VARID, context.args[OBSID_ARG], context.args[VALID_ARG]);
   };
};


#endif  // _BDEXPR_H_