#ifndef _BDEXPR3_H_
#define _BDEXPR3_H_

#include "BDSet.h"
#include "SamplerExecutionContext.h"
#include "utils/DebugHelper.h"
#include "utils/RndUtils.h"

// This is left here in case we need to improve the implementation of variadic expressions.
// This file can be otherwise ignored (use VrdExprOperators.h instead)

namespace BDExpr3 {

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

   throw std::runtime_error("BDExpr3.h::rndChoice()::Unreachable");
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
      std::get<VGID>(model).assertObservation(VARID, VARID, VALID);
      return true;
   };
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
   }

   template<typename M, typename E>
   static inline bool sampleSat(M &model, SamplerExecutionContext<E> &context) 
   {
      std::get<VGID>(model).assertObservation(context.args[VARID_ARG], context.args[OBSID_ARG], context.args[VALID_ARG]);
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
   }

   template<typename M, typename E>
   static inline bool sampleSat(M &model, SamplerExecutionContext<E> &context) 
   {
      std::get<VGID>(model).assertObservation(context.args[VARID_ARG], context.args[OBSID_ARG], VALID);
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
};






// -------------------------
// EXOR
// -------------------------

// -- prototypes --

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

// -- end of prototypes --


// -- struct def --

template <size_t I_, typename T_> 
struct EXORTerm{
   typedef T_ term;
};


template <size_t... Is_, typename... Ts_>
struct EXORImpl<std::index_sequence<Is_...>, Ts_...>
        : public EXORTerm<Is_, Ts_>... 
{
public:

   static inline size_t numTerms()
   { return sizeof...(Ts_); }

   // ppSat

   template<typename M, typename E>
   static inline double ppSat(M &model, SamplerExecutionContext<E> &context)
   {
      double probs[] = { Ts_::ppSat(model, context)...};
      double result = 0;
      for (size_t i=0; i<sizeof...(Ts_); i++)
      { result += probs[i]; }
      return result;
   };

   // retract

   template <int TERMID, typename M, typename E>
   static typename std::enable_if<(TERMID > 0), bool>::type
   retractImpl(M &model, SamplerExecutionContext<E> &context)
   {
      if (EXORLookup<TERMID,EXOR<Ts_...>>::term::retract(model, context)) return true;
      return retractImpl<TERMID-1,M>(model, context);
   };

   template <int TERMID, typename M, typename E>
   static inline typename std::enable_if<TERMID == 0, bool>::type
   retractImpl(M &model, SamplerExecutionContext<E> &context)
   {
      if (EXORLookup<0,EXOR<Ts_...>>::term::retract(model, context)) return true;
      return false;
   };

   template<typename M, typename E>
   static inline bool retract(M &model, SamplerExecutionContext<E> &context)
   {
      return retractImpl<sizeof...(Ts_)-1,M>(model, context);
   };

   // sampleSat

   template <int NUMCHOICES, typename M, typename E>
   static inline typename std::enable_if<(NUMCHOICES > 8), bool>::type 
   sampleSatSwitch(int termId, M &model, SamplerExecutionContext<E> &context)
   {
      switch(termId)
      {
         case NUMCHOICES-1: return EXORLookup<NUMCHOICES-1,EXOR<Ts_...>>::term::sampleSat(model, context);
         case NUMCHOICES-2: return EXORLookup<NUMCHOICES-2,EXOR<Ts_...>>::term::sampleSat(model, context);
         case NUMCHOICES-3: return EXORLookup<NUMCHOICES-3,EXOR<Ts_...>>::term::sampleSat(model, context);
         case NUMCHOICES-4: return EXORLookup<NUMCHOICES-4,EXOR<Ts_...>>::term::sampleSat(model, context);
         case NUMCHOICES-5: return EXORLookup<NUMCHOICES-5,EXOR<Ts_...>>::term::sampleSat(model, context);
         case NUMCHOICES-6: return EXORLookup<NUMCHOICES-6,EXOR<Ts_...>>::term::sampleSat(model, context);
         case NUMCHOICES-7: return EXORLookup<NUMCHOICES-7,EXOR<Ts_...>>::term::sampleSat(model, context);
         case NUMCHOICES-8: return EXORLookup<NUMCHOICES-8,EXOR<Ts_...>>::term::sampleSat(model, context);

         default: return sampleSatSwitch<std::max((NUMCHOICES-8),0)>(termId, model, context);
      }
   };

   template <int NUMCHOICES, typename M, typename E>
   static inline typename std::enable_if<NUMCHOICES == 0, bool>::type 
   sampleSatSwitch(int termId, M &model, SamplerExecutionContext<E> &context)
   {
      throw std::runtime_error("sampleSatSwitch: invalid index");
      SUPPRESS_UNUSED_PARAM_WARNING(termId);
      SUPPRESS_UNUSED_PARAM_WARNING(model);
      SUPPRESS_UNUSED_PARAM_WARNING(context);
   };

   template <int NUMCHOICES, typename M, typename E>
   static inline typename std::enable_if<NUMCHOICES == 1, bool>::type 
   sampleSatSwitch(int termId, M &model, SamplerExecutionContext<E> &context)
   {
      switch(termId)
      {
         case 0: return EXORLookup<0,EXOR<Ts_...>>::term::sampleSat(model, context); 
         default: throw std::runtime_error("sampleSatSwitch: invalid index");
      }
   };

   template <int NUMCHOICES, typename M, typename E>
   static inline typename std::enable_if<NUMCHOICES == 2, bool>::type 
   sampleSatSwitch(int termId, M &model, SamplerExecutionContext<E> &context)
   {
      switch(termId)
      {
         case 1: return EXORLookup<1,EXOR<Ts_...>>::term::sampleSat(model, context); 
         case 0: return EXORLookup<0,EXOR<Ts_...>>::term::sampleSat(model, context); 
         default: throw std::runtime_error("sampleSatSwitch: invalid index");
      }
   };

   template <int NUMCHOICES, typename M, typename E>
   static inline typename std::enable_if<NUMCHOICES == 3, bool>::type 
   sampleSatSwitch(int termId, M &model, SamplerExecutionContext<E> &context)
   {
      switch(termId)
      {
         case 2: return EXORLookup<2,EXOR<Ts_...>>::term::sampleSat(model, context); 
         case 1: return EXORLookup<1,EXOR<Ts_...>>::term::sampleSat(model, context); 
         case 0: return EXORLookup<0,EXOR<Ts_...>>::term::sampleSat(model, context); 
         default: throw std::runtime_error("sampleSatSwitch: invalid index");
      }
   };

   template <int NUMCHOICES, typename M, typename E>
   static inline typename std::enable_if<NUMCHOICES == 4, bool>::type 
   sampleSatSwitch(int termId, M &model, SamplerExecutionContext<E> &context)
   {
      switch(termId)
      {
         case 3: return EXORLookup<3,EXOR<Ts_...>>::term::sampleSat(model, context); 
         case 2: return EXORLookup<2,EXOR<Ts_...>>::term::sampleSat(model, context); 
         case 1: return EXORLookup<1,EXOR<Ts_...>>::term::sampleSat(model, context); 
         case 0: return EXORLookup<0,EXOR<Ts_...>>::term::sampleSat(model, context); 
         default: throw std::runtime_error("sampleSatSwitch: invalid index");
      }
   };

   template <int NUMCHOICES, typename M, typename E>
   static inline typename std::enable_if<NUMCHOICES == 5, bool>::type 
   sampleSatSwitch(int termId, M &model, SamplerExecutionContext<E> &context)
   {
      switch(termId)
      {
         case 4: return EXORLookup<4,EXOR<Ts_...>>::term::sampleSat(model, context); 
         case 3: return EXORLookup<3,EXOR<Ts_...>>::term::sampleSat(model, context); 
         case 2: return EXORLookup<2,EXOR<Ts_...>>::term::sampleSat(model, context); 
         case 1: return EXORLookup<1,EXOR<Ts_...>>::term::sampleSat(model, context); 
         case 0: return EXORLookup<0,EXOR<Ts_...>>::term::sampleSat(model, context); 
         default: throw std::runtime_error("sampleSatSwitch: invalid index");
      }
   };

   template <int NUMCHOICES, typename M, typename E>
   static inline typename std::enable_if<NUMCHOICES == 6, bool>::type 
   sampleSatSwitch(int termId, M &model, SamplerExecutionContext<E> &context)
   {
      switch(termId)
      {
         case 5: return EXORLookup<5,EXOR<Ts_...>>::term::sampleSat(model, context); 
         case 4: return EXORLookup<4,EXOR<Ts_...>>::term::sampleSat(model, context); 
         case 3: return EXORLookup<3,EXOR<Ts_...>>::term::sampleSat(model, context); 
         case 2: return EXORLookup<2,EXOR<Ts_...>>::term::sampleSat(model, context); 
         case 1: return EXORLookup<1,EXOR<Ts_...>>::term::sampleSat(model, context); 
         case 0: return EXORLookup<0,EXOR<Ts_...>>::term::sampleSat(model, context); 
         default: throw std::runtime_error("sampleSatSwitch: invalid index");
      }
   };

   template <int NUMCHOICES, typename M, typename E>
   static inline typename std::enable_if<NUMCHOICES == 7, bool>::type 
   sampleSatSwitch(int termId, M &model, SamplerExecutionContext<E> &context)
   {
      switch(termId)
      {
         case 6: return EXORLookup<6,EXOR<Ts_...>>::term::sampleSat(model, context); 
         case 5: return EXORLookup<5,EXOR<Ts_...>>::term::sampleSat(model, context); 
         case 4: return EXORLookup<4,EXOR<Ts_...>>::term::sampleSat(model, context); 
         case 3: return EXORLookup<3,EXOR<Ts_...>>::term::sampleSat(model, context); 
         case 2: return EXORLookup<2,EXOR<Ts_...>>::term::sampleSat(model, context); 
         case 1: return EXORLookup<1,EXOR<Ts_...>>::term::sampleSat(model, context); 
         case 0: return EXORLookup<0,EXOR<Ts_...>>::term::sampleSat(model, context); 
         default: throw std::runtime_error("sampleSatSwitch: invalid index");
      }
   };

   template <int NUMCHOICES, typename M, typename E>
   static inline typename std::enable_if<NUMCHOICES == 8, bool>::type 
   sampleSatSwitch(int termId, M &model, SamplerExecutionContext<E> &context)
   {
      switch(termId)
      {
         case 7: return EXORLookup<7,EXOR<Ts_...>>::term::sampleSat(model, context); 
         case 6: return EXORLookup<6,EXOR<Ts_...>>::term::sampleSat(model, context); 
         case 5: return EXORLookup<5,EXOR<Ts_...>>::term::sampleSat(model, context); 
         case 4: return EXORLookup<4,EXOR<Ts_...>>::term::sampleSat(model, context); 
         case 3: return EXORLookup<3,EXOR<Ts_...>>::term::sampleSat(model, context); 
         case 2: return EXORLookup<2,EXOR<Ts_...>>::term::sampleSat(model, context); 
         case 1: return EXORLookup<1,EXOR<Ts_...>>::term::sampleSat(model, context); 
         case 0: return EXORLookup<0,EXOR<Ts_...>>::term::sampleSat(model, context); 
         default: throw std::runtime_error("sampleSatSwitch: invalid index");
      }
   };

   template<typename M, typename E>
   static inline bool sampleSat(M &model, SamplerExecutionContext<E> &context)
   {
      double probs[] = { Ts_::ppSat(model, context)...};
      int choice = rndChoice<sizeof...(Ts_)>(probs, context.rndSeed);
      return sampleSatSwitch<sizeof...(Ts_),M>(choice, model, context);
   };
}; // EXORImpl

template <typename... Ts_>
struct EXOR : public EXORImpl<std::make_index_sequence<sizeof...(Ts_)>, Ts_...> {};

template <size_t I_, typename T_>
struct  EXORLookup {};

template <size_t I_>
class EXORLookup<I_, EXOR<>> {
public:
    static_assert(I_ != I_, "EXORLookup: index out of range");
};

template <typename H_, typename... Ts_>
class EXORLookup<0, EXOR<H_, Ts_...>> {
public:
    typedef H_ term;
};

template <size_t I_, typename H_, typename... Ts_>
class EXORLookup<I_, EXOR<H_, Ts_...>> {
public:
    typedef typename EXORLookup<I_ - 1, EXOR<Ts_...>>::term term;
};

// -------------------------
// end of EXOR
// -------------------------



// -------------------------
// IAND
// -------------------------

// -- prototypes --

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

// IANDTerm: a term inside an IAND expression
// usage: IANDTerm<2,VAR3<..>> --> the third term inside an IAND expression
template <size_t I_, typename T_> 
struct IANDTerm;

// -- end of prototypes --


// -- struct def --

template <size_t I_, typename T_> 
struct IANDTerm{
   typedef T_ term;
};

template <size_t... Is_, typename... Ts_>
struct IANDImpl<std::index_sequence<Is_...>, Ts_...>
        : public IANDTerm<Is_, Ts_>... 
{
public:

   static inline size_t numTerms()
   { return sizeof...(Ts_); }

   // ppSat

   template<typename M, typename E>
   static inline double ppSat(M &model, SamplerExecutionContext<E> &context)
   {
      double probs[] = { Ts_::ppSat(model, context)...};
      double result = 1.0;
      for (size_t i=0; i<sizeof...(Ts_); i++)
      {
         result *= probs[i];
      }
      return result;
   };

   template<typename M, typename E>
   static inline bool retract(M &model, SamplerExecutionContext<E> &context)
   {
      bool res = false;
      bool results[] = { Ts_::retract(model, context)... };
      for (size_t i=0; i<sizeof...(Ts_); i++)
      {
         res = (res||results[i]);
      }
      return res;
   };

   template<typename M, typename E>
   static inline bool sampleSat(M &model, SamplerExecutionContext<E> &context)
   {
      bool results[] = { Ts_::sampleSat(model, context)... };
      SUPPRESS_UNUSED_PARAM_WARNING(results);
      return true;
   };

}; // IANDImpl


template <typename... Ts_>
struct IAND : public IANDImpl<std::make_index_sequence<sizeof...(Ts_)>, Ts_...> {};

template <size_t I_, typename T_>
struct  IANDLookup {};

template <size_t I_>
class IANDLookup<I_, IAND<>> {
public:
   static_assert(I_ != I_, "IANDLookup: index out of range");
};

template <typename H_, typename... Ts_>
class IANDLookup<0, IAND<H_, Ts_...>> {
public:
   typedef H_ term;
};

template <size_t I_, typename H_, typename... Ts_>
class IANDLookup<I_, IAND<H_, Ts_...>> {
public:
   typedef typename IANDLookup<I_ - 1, IAND<Ts_...>>::term term;
};

// -------------------------
// end of IAND
// -------------------------


} //namespace BDExpr3

#endif  // _BDEXPR3_H_

