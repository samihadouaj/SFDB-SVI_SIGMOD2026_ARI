#ifndef _VRDEXPROPERATORS_H_
#define _VRDEXPROPERATORS_H_

#include "VrdExprLiterals.h"

// generated on Thu Sep 19 11:36:54 2024

namespace  VrdExpr {

// -------------------------
// EXOR
// -------------------------

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

   template<typename M, typename E>
   static inline bool toString(std::ostream &os, M &model, SamplerExecutionContext<E> &context)
   {
      os << "EXOR(";
      bool results[] = { Ts_::toString(os, model, context)... };
      SUPPRESS_UNUSED_PARAM_WARNING(results);
      os << ")";
      return true;
   }

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
// IAND
// -------------------------

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

   template<typename M, typename E>
   static inline bool toString(std::ostream &os, M &model, SamplerExecutionContext<E> &context)
   {
      os << "IAND(";
      bool results[] = { Ts_::toString(os, model, context)... };
      SUPPRESS_UNUSED_PARAM_WARNING(results);
      os << ")";
      return true;
   }

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



} // namespace  VrdExpr 

#endif  // _VRDEXPROPERATORS_H_

