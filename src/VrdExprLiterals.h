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

#ifndef _VRDEXPRLITERALS_H_
#define _VRDEXPRLITERALS_H_

#include "BDSet.h"
#include "SamplerExecutionContext.h"
#include "utils/DebugHelper.h"
#include "utils/RndUtils.h"


namespace  VrdExpr {

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

   throw std::runtime_error("VrdExprLiterals.h::rndChoice()::Unreachable");
};

template<int VGID, int VARID,int OBSID,int VALID> 
class CONST
{
public:

   template<typename M, typename E>
   static inline double ppSat(M &model, SamplerExecutionContext<E> &context) 
   { 
      SUPPRESS_UNUSED_PARAM_WARNING(context);
      return std::get<VGID>(model).getPosteriorPredictive(VARID, VALID);
   };

   template<typename M, typename E>
   static inline bool retract(M &model, SamplerExecutionContext<E> &context)
   {
      SUPPRESS_UNUSED_PARAM_WARNING(context);
      return std::get<VGID>(model).retractObservation(OBSID);
   }

   template<typename M, typename E>
   static inline bool sampleSat(M &model, SamplerExecutionContext<E> &context) 
   {
      SUPPRESS_UNUSED_PARAM_WARNING(context);
      std::get<VGID>(model).assertObservation(VARID, OBSID, VALID);
      return true;
   };

   template<typename M, typename E>
   static inline bool toString(std::ostream &os, M &model, SamplerExecutionContext<E> &context)
   {
      SUPPRESS_UNUSED_PARAM_WARNING(context);
      SUPPRESS_UNUSED_PARAM_WARNING(model);
      os << "(VG" << VGID << ".VARID" << VARID << "[" << OBSID << "]=" << VALID << ")"; 
      return true;
   }
};

template<int VGID, int VARID_ARG,int OBSID_ARG,int VALID_ARG> 
class VAR
{
public:

   template<typename M, typename E>
   static inline double ppSat(M &model, SamplerExecutionContext<E> &context) 
   { 
      return std::get<VGID>(model).getPosteriorPredictive(context.args[VARID_ARG], context.args[VALID_ARG]);
   };

   template<typename M, typename E>
   static inline bool retract(M &model, SamplerExecutionContext<E> &context)
   {
      return std::get<VGID>(model).retractObservation(context.args[OBSID_ARG]);
   };

   template<typename M, typename E>
   static inline bool sampleSat(M &model, SamplerExecutionContext<E> &context) 
   {
      std::get<VGID>(model).assertObservation(context.args[VARID_ARG], context.args[OBSID_ARG], context.args[VALID_ARG]);
      return true;
   };

   template<typename M, typename E>
   static inline bool toString(std::ostream &os, M &model, SamplerExecutionContext<E> &context)
   {
      SUPPRESS_UNUSED_PARAM_WARNING(model);
      os << "(VG" << VGID << ".VARID" << context.args[VARID_ARG] << "[" << context.args[OBSID_ARG] << "]=" << context.args[VALID_ARG] << ")"; 
      return true;
   };
};

template<int VGID, int VARID_ARG,int OBSID_ARG,int VALID> 
class VAR2
{
public:

   template<typename M, typename E>
   static inline double ppSat(M &model, SamplerExecutionContext<E> &context) 
   { 
      return std::get<VGID>(model).getPosteriorPredictive(context.args[VARID_ARG], VALID);
   };

   template<typename M, typename E>
   static inline bool retract(M &model, SamplerExecutionContext<E> &context)
   {
      return std::get<VGID>(model).retractObservation(context.args[OBSID_ARG]);
   };

   template<typename M, typename E>
   static inline bool sampleSat(M &model, SamplerExecutionContext<E> &context) 
   {
      std::get<VGID>(model).assertObservation(context.args[VARID_ARG], context.args[OBSID_ARG], VALID);
      return true;
   };

   template<typename M, typename E>
   static inline bool toString(std::ostream &os, M &model, SamplerExecutionContext<E> &context)
   {
      SUPPRESS_UNUSED_PARAM_WARNING(model);
      os << "(VG" << VGID << ".VARID" << context.args[VARID_ARG] << "[" << context.args[OBSID_ARG] <<"]=" << VALID << ")"; 
      return true;
   };
};

template<int VGID, int VARID, int OBSID_ARG, int VALID_ARG> 
class VAR3
{
public:

   template<typename M, typename E>
   static inline double ppSat(M &model, SamplerExecutionContext<E> &context) 
   { 
      return std::get<VGID>(model).getPosteriorPredictive(VARID, context.args[VALID_ARG]);
   };

   template<typename M, typename E>
   static inline bool retract(M &model, SamplerExecutionContext<E> &context)
   {
      return std::get<VGID>(model).retractObservation(context.args[OBSID_ARG]);
   }

   template<typename M, typename E>
   static inline bool sampleSat(M &model, SamplerExecutionContext<E> &context) 
   {
      std::get<VGID>(model).assertObservation(VARID, context.args[OBSID_ARG], context.args[VALID_ARG]);
      return true;
   };

   template<typename M, typename E>
   static inline bool toString(std::ostream &os, M &model, SamplerExecutionContext<E> &context)
   {
      SUPPRESS_UNUSED_PARAM_WARNING(model);
      os << "(VG" << VGID << ".VARID" << VARID << "[" << context.args[OBSID_ARG] <<"]=" << context.args[VALID_ARG] << ")"; 
      return true;
   };
};

template<int VGID, int VARID, int OBSID_ARG, int VALID_ARG> 
class VAR3_DT
{
public:

   template<typename M, typename E>
   static inline double ppSat(M &model, SamplerExecutionContext<E> &context) 
   {
      if (context.args[VALID_ARG]==-1)
         return 1.0;
      else 
         return std::get<VGID>(model).getPosteriorPredictive(VARID, context.args[VALID_ARG]);
   };

   template<typename M, typename E>
   static inline bool retract(M &model, SamplerExecutionContext<E> &context)
   {
      if (context.args[VALID_ARG]==-1)
         return false;
      else
         return std::get<VGID>(model).retractObservation(context.args[OBSID_ARG]);
   }

   template<typename M, typename E>
   static inline bool sampleSat(M &model, SamplerExecutionContext<E> &context) 
   {
      if (context.args[VALID_ARG]==-1)
         return true;
      std::get<VGID>(model).assertObservation(VARID, context.args[OBSID_ARG], context.args[VALID_ARG]);
      return true;
   };

   template<typename M, typename E>
   static inline bool toString(std::ostream &os, M &model, SamplerExecutionContext<E> &context)
   {
      SUPPRESS_UNUSED_PARAM_WARNING(model);
      if (context.args[VALID_ARG]==-1)
      {
         os << "(TRUE)"; 
      }
      else
      {
         os << "(VG" << VGID << ".VARID" << VARID << "[" << context.args[OBSID_ARG] <<"]=" << context.args[VALID_ARG] << ")"; 
      }
      return true;
   };
};

template<int VGID, int VARID, int OBSID_ARG, int VALID_ARG> 
class VAR3_DF
{
public:

   template<typename M, typename E>
   static inline double ppSat(M &model, SamplerExecutionContext<E> &context) 
   {
      if (context.args[VALID_ARG]==-1)
         return 0.0;
      else 
         return std::get<VGID>(model).getPosteriorPredictive(VARID, context.args[VALID_ARG]);
   };

   template<typename M, typename E>
   static inline bool retract(M &model, SamplerExecutionContext<E> &context)
   {
      if (context.args[VALID_ARG]==-1)
         return false;
      else
         return std::get<VGID>(model).retractObservation(context.args[OBSID_ARG]);
   }

   template<typename M, typename E>
   static inline bool sampleSat(M &model, SamplerExecutionContext<E> &context) 
   {
      if (context.args[VALID_ARG]==-1)
         return true;
      std::get<VGID>(model).assertObservation(VARID, context.args[OBSID_ARG], context.args[VALID_ARG]);
      return true;
   };

   template<typename M, typename E>
   static inline bool toString(std::ostream &os, M &model, SamplerExecutionContext<E> &context)
   {
      SUPPRESS_UNUSED_PARAM_WARNING(model);
      if (context.args[VALID_ARG]==-1)
      {
         os << "(FALSE)"; 
      }
      else
      {
         os << "(VG" << VGID << ".VARID" << VARID << "[" << context.args[OBSID_ARG] << "]=" << context.args[VALID_ARG] << ")"; 
      }
      return true;
   };
};

// TODO
template<int REF_ARG>
class REF_DT
{
public:

   template<typename M, typename E>
   static inline double ppSat(M &model, SamplerExecutionContext<E> &context) 
   {
      int refTgt = context.args[REF_ARG];

      // If NULL-REF return prob=1.0
      if (refTgt==-1)
         return 1.0;

      // shift context
      context.shift(refTgt);

      // do we need to recompute?
      double prob = -1.0;
      if (context.probs[0]<0)
      {
         // yes
         context.probs[0] = E::ppSat(model, context);
      }
      prob = context.probs[0];

      // realign context
      context.shift(-refTgt);

      return prob;
   };

   template<typename M, typename E>
   static inline bool retract(M &model, SamplerExecutionContext<E> &context)
   {
      int refTgt = context.args[REF_ARG];

      // If NULL-REF return false
      if (refTgt==-1)
         return false;
      
      // shift context
      context.shift(refTgt);

      bool res = false;

      // do we need to retract?
      if (context.retracted[0]==0)
      {
         // yes
         res = E::retract(model, context);
         context.retracted[0] = 1;
         context.probs[0] = -1.0;
      }

      // realign context
      context.shift(-refTgt);
      
      return res;
   }

   template<typename M, typename E>
   static inline bool sampleSat(M &model, SamplerExecutionContext<E> &context) 
   {
      int refTgt = context.args[REF_ARG];

      // If NULL-REF return true
      if (refTgt==-1)
         return true;

      // shift context
      context.shift(refTgt);

      // do we need to sample?
      if (context.retracted[0]==1)
      {
         E::sampleSat(model, context);
         context.retracted[0] = 0;
         // context.probs[0] = -1.0;
      }

      // realign context
      context.shift(-refTgt);

      return true;

      //throw std::runtime_error("VrdExprLiterals.h::REF_DT::NOT_IMPLEMENTED");
      //std::get<VGID>(model).assertObservation(VARID, context.args[OBSID_ARG], context.args[VALID_ARG]);
      //return true;
   };

   template<typename M, typename E>
   static inline bool toString(std::ostream &os, M &model, SamplerExecutionContext<E> &context)
   {
      SUPPRESS_UNUSED_PARAM_WARNING(model);
      os << "(REF[" << context.args[REF_ARG] << "])" ; 
      return true;
   };
};

// -------------------------
// EXOR prototypes
// -------------------------

// EXOR: a variadic exor expression
// usage: EXOR<IAND<..,..>,VAR3<..,..,..,..>,...>
template <typename... Ts_>
struct EXOR;

// EXORLookup: helper type to access a specific term within an EXOR expression
// usage: EXORLookup<1,EXOR<IAND<..>,VAR2<..>,VAR3<..>>>::type --> returns VAR2<..>
template <size_t I_, typename T_>
struct  EXORLookup;

// EXORImpl: utility type to implement EXOR
// usage: EXORImpl<std::integer_sequence<0,1,2>,IAND<..>,VAR2<..>,VAR3<..>>
// the initial integer sequence indexes the terms of the EXOR
template <typename INDEX_SEQUENCE, typename... Ts_>
struct EXORImpl;

// EXORTerm: a term inside an EXOR expression
// usage: EXORTerm<2,VAR3<..>> --> the third term inside an EXOR expression
template <size_t I_, typename T_> 
struct EXORTerm;





// -------------------------
// IAND prototypes
// -------------------------

// IAND: a variadic iand expression
// usage: IAND<EXOR<..,..>,VAR3<..,..,..,..>,...>
template <typename... Ts_>
struct IAND;

// IANDLookup: helper type to access a specific term within a IAND expression
// usage: IANDLookup<1,IAND<EXOR<..>,VAR2<..>,VAR3<..>>>::type --> returns VAR2<..>
template <size_t I_, typename T_>
struct  IANDLookup;

// IANDImpl: utility type to implement IAND
// usage: IANDImpl<std::integer_sequence<0,1,2>,EXOR<..>,VAR2<..>,VAR3<..>>
// the initial integer sequence indexes the terms of the IAND
template <typename INDEX_SEQUENCE, typename... Ts_>
struct IANDImpl;





} // namespace  VrdExpr 


#endif  // _VRDEXPRLITERALS_H_


