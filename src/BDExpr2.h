#ifndef _BDEXPR2_H_
#define _BDEXPR2_H_

// generated on Thu Sep 19 11:36:54 2024

#include "BDSet.h"
#include "SamplerExecutionContext.h"
#include "utils/DebugHelper.h"
#include "utils/RndUtils.h"

#include <tuple>

template<class C0, class C1>
class IAND2
{
public:

   template<typename M, typename E>
   static double ppSat(M &model, SamplerExecutionContext<E> &context)
   {
      return C0::ppSat(model, context)
      * C1::ppSat(model, context);
   };

   template<typename M, typename E>
   static bool retract(M &model, SamplerExecutionContext<E> &context)
   {
      bool res = false;
      bool tmp = false;
      tmp = C0::retract(model, context); res = (res||tmp);
      tmp = C1::retract(model, context); res = (res||tmp);
      return res;
   };

   template<typename M, typename E>
   static void sampleSat(M &model, SamplerExecutionContext<E> &context)
   {
      C0::sampleSat(model, context);
      C1::sampleSat(model, context);
   };

};

template<class C0, class C1>
class EXOR2
{
public:

   template<typename M, typename E>
   static double ppSat(M &model, SamplerExecutionContext<E> &context)
   {
      return C0::ppSat(model, context)
      + C1::ppSat(model, context);
   };

   template<typename M, typename E>
   static bool retract(M &model, SamplerExecutionContext<E> &context)
   {
      if (C0::retract(model, context)) return true;
      if (C1::retract(model, context)) return true;
      return false;
   };

   template<typename M, typename E>
   static void sampleSat(M &model, SamplerExecutionContext<E> &context)
   {
      double probs[2] = {
      C0::ppSat(model, context),
      C1::ppSat(model, context)
      };
      int choice = rndChoice<2>(probs, context.rndSeed);
      switch (choice)
      {
         case 0 : C0::sampleSat(model, context); break;
         case 1 : C1::sampleSat(model, context); break;
      }
   };

};

template<class C0, class C1, class C2>
class IAND3
{
public:

   template<typename M, typename E>
   static double ppSat(M &model, SamplerExecutionContext<E> &context)
   {
      return C0::ppSat(model, context)
      * C1::ppSat(model, context)
      * C2::ppSat(model, context);
   };

   template<typename M, typename E>
   static bool retract(M &model, SamplerExecutionContext<E> &context)
   {
      bool res = false;
      bool tmp = false;
      tmp = C0::retract(model, context); res = (res||tmp);
      tmp = C1::retract(model, context); res = (res||tmp);
      tmp = C2::retract(model, context); res = (res||tmp);
      return res;
   };

   template<typename M, typename E>
   static void sampleSat(M &model, SamplerExecutionContext<E> &context)
   {
      C0::sampleSat(model, context);
      C1::sampleSat(model, context);
      C2::sampleSat(model, context);
   };

};

template<class C0, class C1, class C2>
class EXOR3
{
public:

   template<typename M, typename E>
   static double ppSat(M &model, SamplerExecutionContext<E> &context)
   {
      return C0::ppSat(model, context)
      + C1::ppSat(model, context)
      + C2::ppSat(model, context);
   };

   template<typename M, typename E>
   static bool retract(M &model, SamplerExecutionContext<E> &context)
   {
      if (C0::retract(model, context)) return true;
      if (C1::retract(model, context)) return true;
      if (C2::retract(model, context)) return true;
      return false;
   };

   template<typename M, typename E>
   static void sampleSat(M &model, SamplerExecutionContext<E> &context)
   {
      double probs[3] = {
      C0::ppSat(model, context),
      C1::ppSat(model, context),
      C2::ppSat(model, context)
      };
      int choice = rndChoice<3>(probs, context.rndSeed);
      switch (choice)
      {
         case 0 : C0::sampleSat(model, context); break;
         case 1 : C1::sampleSat(model, context); break;
         case 2 : C2::sampleSat(model, context); break;
      }
   };

};

template<class C0, class C1, class C2, class C3>
class IAND4
{
public:

   template<typename M, typename E>
   static double ppSat(M &model, SamplerExecutionContext<E> &context)
   {
      return C0::ppSat(model, context)
      * C1::ppSat(model, context)
      * C2::ppSat(model, context)
      * C3::ppSat(model, context);
   };

   template<typename M, typename E>
   static bool retract(M &model, SamplerExecutionContext<E> &context)
   {
      bool res = false;
      bool tmp = false;
      tmp = C0::retract(model, context); res = (res||tmp);
      tmp = C1::retract(model, context); res = (res||tmp);
      tmp = C2::retract(model, context); res = (res||tmp);
      tmp = C3::retract(model, context); res = (res||tmp);
      return res;
   };

   template<typename M, typename E>
   static void sampleSat(M &model, SamplerExecutionContext<E> &context)
   {
      C0::sampleSat(model, context);
      C1::sampleSat(model, context);
      C2::sampleSat(model, context);
      C3::sampleSat(model, context);
   };

};

template<class C0, class C1, class C2, class C3>
class EXOR4
{
public:

   template<typename M, typename E>
   static double ppSat(M &model, SamplerExecutionContext<E> &context)
   {
      return C0::ppSat(model, context)
      + C1::ppSat(model, context)
      + C2::ppSat(model, context)
      + C3::ppSat(model, context);
   };

   template<typename M, typename E>
   static bool retract(M &model, SamplerExecutionContext<E> &context)
   {
      if (C0::retract(model, context)) return true;
      if (C1::retract(model, context)) return true;
      if (C2::retract(model, context)) return true;
      if (C3::retract(model, context)) return true;
      return false;
   };

   template<typename M, typename E>
   static void sampleSat(M &model, SamplerExecutionContext<E> &context)
   {
      double probs[4] = {
      C0::ppSat(model, context),
      C1::ppSat(model, context),
      C2::ppSat(model, context),
      C3::ppSat(model, context)
      };
      int choice = rndChoice<4>(probs, context.rndSeed);
      switch (choice)
      {
         case 0 : C0::sampleSat(model, context); break;
         case 1 : C1::sampleSat(model, context); break;
         case 2 : C2::sampleSat(model, context); break;
         case 3 : C3::sampleSat(model, context); break;
      }
   };

};

template<class C0, class C1, class C2, class C3, class C4>
class IAND5
{
public:

   template<typename M, typename E>
   static double ppSat(M &model, SamplerExecutionContext<E> &context)
   {
      return C0::ppSat(model, context)
      * C1::ppSat(model, context)
      * C2::ppSat(model, context)
      * C3::ppSat(model, context)
      * C4::ppSat(model, context);
   };

   template<typename M, typename E>
   static bool retract(M &model, SamplerExecutionContext<E> &context)
   {
      bool res = false;
      bool tmp = false;
      tmp = C0::retract(model, context); res = (res||tmp);
      tmp = C1::retract(model, context); res = (res||tmp);
      tmp = C2::retract(model, context); res = (res||tmp);
      tmp = C3::retract(model, context); res = (res||tmp);
      tmp = C4::retract(model, context); res = (res||tmp);
      return res;
   };

   template<typename M, typename E>
   static void sampleSat(M &model, SamplerExecutionContext<E> &context)
   {
      C0::sampleSat(model, context);
      C1::sampleSat(model, context);
      C2::sampleSat(model, context);
      C3::sampleSat(model, context);
      C4::sampleSat(model, context);
   };

};

template<class C0, class C1, class C2, class C3, class C4>
class EXOR5
{
public:

   template<typename M, typename E>
   static double ppSat(M &model, SamplerExecutionContext<E> &context)
   {
      return C0::ppSat(model, context)
      + C1::ppSat(model, context)
      + C2::ppSat(model, context)
      + C3::ppSat(model, context)
      + C4::ppSat(model, context);
   };

   template<typename M, typename E>
   static bool retract(M &model, SamplerExecutionContext<E> &context)
   {
      if (C0::retract(model, context)) return true;
      if (C1::retract(model, context)) return true;
      if (C2::retract(model, context)) return true;
      if (C3::retract(model, context)) return true;
      if (C4::retract(model, context)) return true;
      return false;
   };

   template<typename M, typename E>
   static void sampleSat(M &model, SamplerExecutionContext<E> &context)
   {
      double probs[5] = {
      C0::ppSat(model, context),
      C1::ppSat(model, context),
      C2::ppSat(model, context),
      C3::ppSat(model, context),
      C4::ppSat(model, context)
      };
      int choice = rndChoice<5>(probs, context.rndSeed);
      switch (choice)
      {
         case 0 : C0::sampleSat(model, context); break;
         case 1 : C1::sampleSat(model, context); break;
         case 2 : C2::sampleSat(model, context); break;
         case 3 : C3::sampleSat(model, context); break;
         case 4 : C4::sampleSat(model, context); break;
      }
   };

};

template<class C0, class C1, class C2, class C3, class C4, class C5>
class IAND6
{
public:

   template<typename M, typename E>
   static double ppSat(M &model, SamplerExecutionContext<E> &context)
   {
      return C0::ppSat(model, context)
      * C1::ppSat(model, context)
      * C2::ppSat(model, context)
      * C3::ppSat(model, context)
      * C4::ppSat(model, context)
      * C5::ppSat(model, context);
   };

   template<typename M, typename E>
   static bool retract(M &model, SamplerExecutionContext<E> &context)
   {
      bool res = false;
      bool tmp = false;
      tmp = C0::retract(model, context); res = (res||tmp);
      tmp = C1::retract(model, context); res = (res||tmp);
      tmp = C2::retract(model, context); res = (res||tmp);
      tmp = C3::retract(model, context); res = (res||tmp);
      tmp = C4::retract(model, context); res = (res||tmp);
      tmp = C5::retract(model, context); res = (res||tmp);
      return res;
   };

   template<typename M, typename E>
   static void sampleSat(M &model, SamplerExecutionContext<E> &context)
   {
      C0::sampleSat(model, context);
      C1::sampleSat(model, context);
      C2::sampleSat(model, context);
      C3::sampleSat(model, context);
      C4::sampleSat(model, context);
      C5::sampleSat(model, context);
   };

};

template<class C0, class C1, class C2, class C3, class C4, class C5>
class EXOR6
{
public:

   template<typename M, typename E>
   static double ppSat(M &model, SamplerExecutionContext<E> &context)
   {
      return C0::ppSat(model, context)
      + C1::ppSat(model, context)
      + C2::ppSat(model, context)
      + C3::ppSat(model, context)
      + C4::ppSat(model, context)
      + C5::ppSat(model, context);
   };

   template<typename M, typename E>
   static bool retract(M &model, SamplerExecutionContext<E> &context)
   {
      if (C0::retract(model, context)) return true;
      if (C1::retract(model, context)) return true;
      if (C2::retract(model, context)) return true;
      if (C3::retract(model, context)) return true;
      if (C4::retract(model, context)) return true;
      if (C5::retract(model, context)) return true;
      return false;
   };

   template<typename M, typename E>
   static void sampleSat(M &model, SamplerExecutionContext<E> &context)
   {
      double probs[6] = {
      C0::ppSat(model, context),
      C1::ppSat(model, context),
      C2::ppSat(model, context),
      C3::ppSat(model, context),
      C4::ppSat(model, context),
      C5::ppSat(model, context)
      };
      int choice = rndChoice<6>(probs, context.rndSeed);
      switch (choice)
      {
         case 0 : C0::sampleSat(model, context); break;
         case 1 : C1::sampleSat(model, context); break;
         case 2 : C2::sampleSat(model, context); break;
         case 3 : C3::sampleSat(model, context); break;
         case 4 : C4::sampleSat(model, context); break;
         case 5 : C5::sampleSat(model, context); break;
      }
   };

};

template<class C0, class C1, class C2, class C3, class C4, class C5, class C6>
class IAND7
{
public:

   template<typename M, typename E>
   static double ppSat(M &model, SamplerExecutionContext<E> &context)
   {
      return C0::ppSat(model, context)
      * C1::ppSat(model, context)
      * C2::ppSat(model, context)
      * C3::ppSat(model, context)
      * C4::ppSat(model, context)
      * C5::ppSat(model, context)
      * C6::ppSat(model, context);
   };

   template<typename M, typename E>
   static bool retract(M &model, SamplerExecutionContext<E> &context)
   {
      bool res = false;
      bool tmp = false;
      tmp = C0::retract(model, context); res = (res||tmp);
      tmp = C1::retract(model, context); res = (res||tmp);
      tmp = C2::retract(model, context); res = (res||tmp);
      tmp = C3::retract(model, context); res = (res||tmp);
      tmp = C4::retract(model, context); res = (res||tmp);
      tmp = C5::retract(model, context); res = (res||tmp);
      tmp = C6::retract(model, context); res = (res||tmp);
      return res;
   };

   template<typename M, typename E>
   static void sampleSat(M &model, SamplerExecutionContext<E> &context)
   {
      C0::sampleSat(model, context);
      C1::sampleSat(model, context);
      C2::sampleSat(model, context);
      C3::sampleSat(model, context);
      C4::sampleSat(model, context);
      C5::sampleSat(model, context);
      C6::sampleSat(model, context);
   };

};

template<class C0, class C1, class C2, class C3, class C4, class C5, class C6>
class EXOR7
{
public:

   template<typename M, typename E>
   static double ppSat(M &model, SamplerExecutionContext<E> &context)
   {
      return C0::ppSat(model, context)
      + C1::ppSat(model, context)
      + C2::ppSat(model, context)
      + C3::ppSat(model, context)
      + C4::ppSat(model, context)
      + C5::ppSat(model, context)
      + C6::ppSat(model, context);
   };

   template<typename M, typename E>
   static bool retract(M &model, SamplerExecutionContext<E> &context)
   {
      if (C0::retract(model, context)) return true;
      if (C1::retract(model, context)) return true;
      if (C2::retract(model, context)) return true;
      if (C3::retract(model, context)) return true;
      if (C4::retract(model, context)) return true;
      if (C5::retract(model, context)) return true;
      if (C6::retract(model, context)) return true;
      return false;
   };

   template<typename M, typename E>
   static void sampleSat(M &model, SamplerExecutionContext<E> &context)
   {
      double probs[7] = {
      C0::ppSat(model, context),
      C1::ppSat(model, context),
      C2::ppSat(model, context),
      C3::ppSat(model, context),
      C4::ppSat(model, context),
      C5::ppSat(model, context),
      C6::ppSat(model, context)
      };
      int choice = rndChoice<7>(probs, context.rndSeed);
      switch (choice)
      {
         case 0 : C0::sampleSat(model, context); break;
         case 1 : C1::sampleSat(model, context); break;
         case 2 : C2::sampleSat(model, context); break;
         case 3 : C3::sampleSat(model, context); break;
         case 4 : C4::sampleSat(model, context); break;
         case 5 : C5::sampleSat(model, context); break;
         case 6 : C6::sampleSat(model, context); break;
      }
   };

};

template<class C0, class C1, class C2, class C3, class C4, class C5, class C6, class C7>
class IAND8
{
public:

   template<typename M, typename E>
   static double ppSat(M &model, SamplerExecutionContext<E> &context)
   {
      return C0::ppSat(model, context)
      * C1::ppSat(model, context)
      * C2::ppSat(model, context)
      * C3::ppSat(model, context)
      * C4::ppSat(model, context)
      * C5::ppSat(model, context)
      * C6::ppSat(model, context)
      * C7::ppSat(model, context);
   };

   template<typename M, typename E>
   static bool retract(M &model, SamplerExecutionContext<E> &context)
   {
      bool res = false;
      bool tmp = false;
      tmp = C0::retract(model, context); res = (res||tmp);
      tmp = C1::retract(model, context); res = (res||tmp);
      tmp = C2::retract(model, context); res = (res||tmp);
      tmp = C3::retract(model, context); res = (res||tmp);
      tmp = C4::retract(model, context); res = (res||tmp);
      tmp = C5::retract(model, context); res = (res||tmp);
      tmp = C6::retract(model, context); res = (res||tmp);
      tmp = C7::retract(model, context); res = (res||tmp);
      return res;
   };

   template<typename M, typename E>
   static void sampleSat(M &model, SamplerExecutionContext<E> &context)
   {
      C0::sampleSat(model, context);
      C1::sampleSat(model, context);
      C2::sampleSat(model, context);
      C3::sampleSat(model, context);
      C4::sampleSat(model, context);
      C5::sampleSat(model, context);
      C6::sampleSat(model, context);
      C7::sampleSat(model, context);
   };

};

template<class C0, class C1, class C2, class C3, class C4, class C5, class C6, class C7>
class EXOR8
{
public:

   template<typename M, typename E>
   static double ppSat(M &model, SamplerExecutionContext<E> &context)
   {
      return C0::ppSat(model, context)
      + C1::ppSat(model, context)
      + C2::ppSat(model, context)
      + C3::ppSat(model, context)
      + C4::ppSat(model, context)
      + C5::ppSat(model, context)
      + C6::ppSat(model, context)
      + C7::ppSat(model, context);
   };

   template<typename M, typename E>
   static bool retract(M &model, SamplerExecutionContext<E> &context)
   {
      if (C0::retract(model, context)) return true;
      if (C1::retract(model, context)) return true;
      if (C2::retract(model, context)) return true;
      if (C3::retract(model, context)) return true;
      if (C4::retract(model, context)) return true;
      if (C5::retract(model, context)) return true;
      if (C6::retract(model, context)) return true;
      if (C7::retract(model, context)) return true;
      return false;
   };

   template<typename M, typename E>
   static void sampleSat(M &model, SamplerExecutionContext<E> &context)
   {
      double probs[8] = {
      C0::ppSat(model, context),
      C1::ppSat(model, context),
      C2::ppSat(model, context),
      C3::ppSat(model, context),
      C4::ppSat(model, context),
      C5::ppSat(model, context),
      C6::ppSat(model, context),
      C7::ppSat(model, context)
      };
      int choice = rndChoice<8>(probs, context.rndSeed);
      switch (choice)
      {
         case 0 : C0::sampleSat(model, context); break;
         case 1 : C1::sampleSat(model, context); break;
         case 2 : C2::sampleSat(model, context); break;
         case 3 : C3::sampleSat(model, context); break;
         case 4 : C4::sampleSat(model, context); break;
         case 5 : C5::sampleSat(model, context); break;
         case 6 : C6::sampleSat(model, context); break;
         case 7 : C7::sampleSat(model, context); break;
      }
   };

};

template<class C0, class C1, class C2, class C3, class C4, class C5, class C6, class C7, class C8>
class IAND9
{
public:

   template<typename M, typename E>
   static double ppSat(M &model, SamplerExecutionContext<E> &context)
   {
      return C0::ppSat(model, context)
      * C1::ppSat(model, context)
      * C2::ppSat(model, context)
      * C3::ppSat(model, context)
      * C4::ppSat(model, context)
      * C5::ppSat(model, context)
      * C6::ppSat(model, context)
      * C7::ppSat(model, context)
      * C8::ppSat(model, context);
   };

   template<typename M, typename E>
   static bool retract(M &model, SamplerExecutionContext<E> &context)
   {
      bool res = false;
      bool tmp = false;
      tmp = C0::retract(model, context); res = (res||tmp);
      tmp = C1::retract(model, context); res = (res||tmp);
      tmp = C2::retract(model, context); res = (res||tmp);
      tmp = C3::retract(model, context); res = (res||tmp);
      tmp = C4::retract(model, context); res = (res||tmp);
      tmp = C5::retract(model, context); res = (res||tmp);
      tmp = C6::retract(model, context); res = (res||tmp);
      tmp = C7::retract(model, context); res = (res||tmp);
      tmp = C8::retract(model, context); res = (res||tmp);
      return res;
   };

   template<typename M, typename E>
   static void sampleSat(M &model, SamplerExecutionContext<E> &context)
   {
      C0::sampleSat(model, context);
      C1::sampleSat(model, context);
      C2::sampleSat(model, context);
      C3::sampleSat(model, context);
      C4::sampleSat(model, context);
      C5::sampleSat(model, context);
      C6::sampleSat(model, context);
      C7::sampleSat(model, context);
      C8::sampleSat(model, context);
   };

};

template<class C0, class C1, class C2, class C3, class C4, class C5, class C6, class C7, class C8>
class EXOR9
{
public:

   template<typename M, typename E>
   static double ppSat(M &model, SamplerExecutionContext<E> &context)
   {
      return C0::ppSat(model, context)
      + C1::ppSat(model, context)
      + C2::ppSat(model, context)
      + C3::ppSat(model, context)
      + C4::ppSat(model, context)
      + C5::ppSat(model, context)
      + C6::ppSat(model, context)
      + C7::ppSat(model, context)
      + C8::ppSat(model, context);
   };

   template<typename M, typename E>
   static bool retract(M &model, SamplerExecutionContext<E> &context)
   {
      if (C0::retract(model, context)) return true;
      if (C1::retract(model, context)) return true;
      if (C2::retract(model, context)) return true;
      if (C3::retract(model, context)) return true;
      if (C4::retract(model, context)) return true;
      if (C5::retract(model, context)) return true;
      if (C6::retract(model, context)) return true;
      if (C7::retract(model, context)) return true;
      if (C8::retract(model, context)) return true;
      return false;
   };

   template<typename M, typename E>
   static void sampleSat(M &model, SamplerExecutionContext<E> &context)
   {
      double probs[9] = {
      C0::ppSat(model, context),
      C1::ppSat(model, context),
      C2::ppSat(model, context),
      C3::ppSat(model, context),
      C4::ppSat(model, context),
      C5::ppSat(model, context),
      C6::ppSat(model, context),
      C7::ppSat(model, context),
      C8::ppSat(model, context)
      };
      int choice = rndChoice<9>(probs, context.rndSeed);
      switch (choice)
      {
         case 0 : C0::sampleSat(model, context); break;
         case 1 : C1::sampleSat(model, context); break;
         case 2 : C2::sampleSat(model, context); break;
         case 3 : C3::sampleSat(model, context); break;
         case 4 : C4::sampleSat(model, context); break;
         case 5 : C5::sampleSat(model, context); break;
         case 6 : C6::sampleSat(model, context); break;
         case 7 : C7::sampleSat(model, context); break;
         case 8 : C8::sampleSat(model, context); break;
      }
   };

};

template<class C0, class C1, class C2, class C3, class C4, class C5, class C6, class C7, class C8, class C9>
class IAND10
{
public:

   template<typename M, typename E>
   static double ppSat(M &model, SamplerExecutionContext<E> &context)
   {
      return C0::ppSat(model, context)
      * C1::ppSat(model, context)
      * C2::ppSat(model, context)
      * C3::ppSat(model, context)
      * C4::ppSat(model, context)
      * C5::ppSat(model, context)
      * C6::ppSat(model, context)
      * C7::ppSat(model, context)
      * C8::ppSat(model, context)
      * C9::ppSat(model, context);
   };

   template<typename M, typename E>
   static bool retract(M &model, SamplerExecutionContext<E> &context)
   {
      bool res = false;
      bool tmp = false;
      tmp = C0::retract(model, context); res = (res||tmp);
      tmp = C1::retract(model, context); res = (res||tmp);
      tmp = C2::retract(model, context); res = (res||tmp);
      tmp = C3::retract(model, context); res = (res||tmp);
      tmp = C4::retract(model, context); res = (res||tmp);
      tmp = C5::retract(model, context); res = (res||tmp);
      tmp = C6::retract(model, context); res = (res||tmp);
      tmp = C7::retract(model, context); res = (res||tmp);
      tmp = C8::retract(model, context); res = (res||tmp);
      tmp = C9::retract(model, context); res = (res||tmp);
      return res;
   };

   template<typename M, typename E>
   static void sampleSat(M &model, SamplerExecutionContext<E> &context)
   {
      C0::sampleSat(model, context);
      C1::sampleSat(model, context);
      C2::sampleSat(model, context);
      C3::sampleSat(model, context);
      C4::sampleSat(model, context);
      C5::sampleSat(model, context);
      C6::sampleSat(model, context);
      C7::sampleSat(model, context);
      C8::sampleSat(model, context);
      C9::sampleSat(model, context);
   };

};

template<class C0, class C1, class C2, class C3, class C4, class C5, class C6, class C7, class C8, class C9>
class EXOR10
{
public:

   template<typename M, typename E>
   static double ppSat(M &model, SamplerExecutionContext<E> &context)
   {
      return C0::ppSat(model, context)
      + C1::ppSat(model, context)
      + C2::ppSat(model, context)
      + C3::ppSat(model, context)
      + C4::ppSat(model, context)
      + C5::ppSat(model, context)
      + C6::ppSat(model, context)
      + C7::ppSat(model, context)
      + C8::ppSat(model, context)
      + C9::ppSat(model, context);
   };

   template<typename M, typename E>
   static bool retract(M &model, SamplerExecutionContext<E> &context)
   {
      if (C0::retract(model, context)) return true;
      if (C1::retract(model, context)) return true;
      if (C2::retract(model, context)) return true;
      if (C3::retract(model, context)) return true;
      if (C4::retract(model, context)) return true;
      if (C5::retract(model, context)) return true;
      if (C6::retract(model, context)) return true;
      if (C7::retract(model, context)) return true;
      if (C8::retract(model, context)) return true;
      if (C9::retract(model, context)) return true;
      return false;
   };

   template<typename M, typename E>
   static void sampleSat(M &model, SamplerExecutionContext<E> &context)
   {
      double probs[10] = {
      C0::ppSat(model, context),
      C1::ppSat(model, context),
      C2::ppSat(model, context),
      C3::ppSat(model, context),
      C4::ppSat(model, context),
      C5::ppSat(model, context),
      C6::ppSat(model, context),
      C7::ppSat(model, context),
      C8::ppSat(model, context),
      C9::ppSat(model, context)
      };
      int choice = rndChoice<10>(probs, context.rndSeed);
      switch (choice)
      {
         case 0 : C0::sampleSat(model, context); break;
         case 1 : C1::sampleSat(model, context); break;
         case 2 : C2::sampleSat(model, context); break;
         case 3 : C3::sampleSat(model, context); break;
         case 4 : C4::sampleSat(model, context); break;
         case 5 : C5::sampleSat(model, context); break;
         case 6 : C6::sampleSat(model, context); break;
         case 7 : C7::sampleSat(model, context); break;
         case 8 : C8::sampleSat(model, context); break;
         case 9 : C9::sampleSat(model, context); break;
      }
   };

};

template<class C0, class C1, class C2, class C3, class C4, class C5, class C6, class C7, class C8, class C9, class C10>
class IAND11
{
public:

   template<typename M, typename E>
   static double ppSat(M &model, SamplerExecutionContext<E> &context)
   {
      return C0::ppSat(model, context)
      * C1::ppSat(model, context)
      * C2::ppSat(model, context)
      * C3::ppSat(model, context)
      * C4::ppSat(model, context)
      * C5::ppSat(model, context)
      * C6::ppSat(model, context)
      * C7::ppSat(model, context)
      * C8::ppSat(model, context)
      * C9::ppSat(model, context)
      * C10::ppSat(model, context);
   };

   template<typename M, typename E>
   static bool retract(M &model, SamplerExecutionContext<E> &context)
   {
      bool res = false;
      bool tmp = false;
      tmp = C0::retract(model, context); res = (res||tmp);
      tmp = C1::retract(model, context); res = (res||tmp);
      tmp = C2::retract(model, context); res = (res||tmp);
      tmp = C3::retract(model, context); res = (res||tmp);
      tmp = C4::retract(model, context); res = (res||tmp);
      tmp = C5::retract(model, context); res = (res||tmp);
      tmp = C6::retract(model, context); res = (res||tmp);
      tmp = C7::retract(model, context); res = (res||tmp);
      tmp = C8::retract(model, context); res = (res||tmp);
      tmp = C9::retract(model, context); res = (res||tmp);
      tmp = C10::retract(model, context); res = (res||tmp);
      return res;
   };

   template<typename M, typename E>
   static void sampleSat(M &model, SamplerExecutionContext<E> &context)
   {
      C0::sampleSat(model, context);
      C1::sampleSat(model, context);
      C2::sampleSat(model, context);
      C3::sampleSat(model, context);
      C4::sampleSat(model, context);
      C5::sampleSat(model, context);
      C6::sampleSat(model, context);
      C7::sampleSat(model, context);
      C8::sampleSat(model, context);
      C9::sampleSat(model, context);
      C10::sampleSat(model, context);
   };

};

template<class C0, class C1, class C2, class C3, class C4, class C5, class C6, class C7, class C8, class C9, class C10>
class EXOR11
{
public:

   template<typename M, typename E>
   static double ppSat(M &model, SamplerExecutionContext<E> &context)
   {
      return C0::ppSat(model, context)
      + C1::ppSat(model, context)
      + C2::ppSat(model, context)
      + C3::ppSat(model, context)
      + C4::ppSat(model, context)
      + C5::ppSat(model, context)
      + C6::ppSat(model, context)
      + C7::ppSat(model, context)
      + C8::ppSat(model, context)
      + C9::ppSat(model, context)
      + C10::ppSat(model, context);
   };

   template<typename M, typename E>
   static bool retract(M &model, SamplerExecutionContext<E> &context)
   {
      if (C0::retract(model, context)) return true;
      if (C1::retract(model, context)) return true;
      if (C2::retract(model, context)) return true;
      if (C3::retract(model, context)) return true;
      if (C4::retract(model, context)) return true;
      if (C5::retract(model, context)) return true;
      if (C6::retract(model, context)) return true;
      if (C7::retract(model, context)) return true;
      if (C8::retract(model, context)) return true;
      if (C9::retract(model, context)) return true;
      if (C10::retract(model, context)) return true;
      return false;
   };

   template<typename M, typename E>
   static void sampleSat(M &model, SamplerExecutionContext<E> &context)
   {
      double probs[11] = {
      C0::ppSat(model, context),
      C1::ppSat(model, context),
      C2::ppSat(model, context),
      C3::ppSat(model, context),
      C4::ppSat(model, context),
      C5::ppSat(model, context),
      C6::ppSat(model, context),
      C7::ppSat(model, context),
      C8::ppSat(model, context),
      C9::ppSat(model, context),
      C10::ppSat(model, context)
      };
      int choice = rndChoice<11>(probs, context.rndSeed);
      switch (choice)
      {
         case 0 : C0::sampleSat(model, context); break;
         case 1 : C1::sampleSat(model, context); break;
         case 2 : C2::sampleSat(model, context); break;
         case 3 : C3::sampleSat(model, context); break;
         case 4 : C4::sampleSat(model, context); break;
         case 5 : C5::sampleSat(model, context); break;
         case 6 : C6::sampleSat(model, context); break;
         case 7 : C7::sampleSat(model, context); break;
         case 8 : C8::sampleSat(model, context); break;
         case 9 : C9::sampleSat(model, context); break;
         case 10 : C10::sampleSat(model, context); break;
      }
   };

};

template<class C0, class C1, class C2, class C3, class C4, class C5, class C6, class C7, class C8, class C9, class C10, class C11>
class IAND12
{
public:

   template<typename M, typename E>
   static double ppSat(M &model, SamplerExecutionContext<E> &context)
   {
      return C0::ppSat(model, context)
      * C1::ppSat(model, context)
      * C2::ppSat(model, context)
      * C3::ppSat(model, context)
      * C4::ppSat(model, context)
      * C5::ppSat(model, context)
      * C6::ppSat(model, context)
      * C7::ppSat(model, context)
      * C8::ppSat(model, context)
      * C9::ppSat(model, context)
      * C10::ppSat(model, context)
      * C11::ppSat(model, context);
   };

   template<typename M, typename E>
   static bool retract(M &model, SamplerExecutionContext<E> &context)
   {
      bool res = false;
      bool tmp = false;
      tmp = C0::retract(model, context); res = (res||tmp);
      tmp = C1::retract(model, context); res = (res||tmp);
      tmp = C2::retract(model, context); res = (res||tmp);
      tmp = C3::retract(model, context); res = (res||tmp);
      tmp = C4::retract(model, context); res = (res||tmp);
      tmp = C5::retract(model, context); res = (res||tmp);
      tmp = C6::retract(model, context); res = (res||tmp);
      tmp = C7::retract(model, context); res = (res||tmp);
      tmp = C8::retract(model, context); res = (res||tmp);
      tmp = C9::retract(model, context); res = (res||tmp);
      tmp = C10::retract(model, context); res = (res||tmp);
      tmp = C11::retract(model, context); res = (res||tmp);
      return res;
   };

   template<typename M, typename E>
   static void sampleSat(M &model, SamplerExecutionContext<E> &context)
   {
      C0::sampleSat(model, context);
      C1::sampleSat(model, context);
      C2::sampleSat(model, context);
      C3::sampleSat(model, context);
      C4::sampleSat(model, context);
      C5::sampleSat(model, context);
      C6::sampleSat(model, context);
      C7::sampleSat(model, context);
      C8::sampleSat(model, context);
      C9::sampleSat(model, context);
      C10::sampleSat(model, context);
      C11::sampleSat(model, context);
   };

};

template<class C0, class C1, class C2, class C3, class C4, class C5, class C6, class C7, class C8, class C9, class C10, class C11>
class EXOR12
{
public:

   template<typename M, typename E>
   static double ppSat(M &model, SamplerExecutionContext<E> &context)
   {
      return C0::ppSat(model, context)
      + C1::ppSat(model, context)
      + C2::ppSat(model, context)
      + C3::ppSat(model, context)
      + C4::ppSat(model, context)
      + C5::ppSat(model, context)
      + C6::ppSat(model, context)
      + C7::ppSat(model, context)
      + C8::ppSat(model, context)
      + C9::ppSat(model, context)
      + C10::ppSat(model, context)
      + C11::ppSat(model, context);
   };

   template<typename M, typename E>
   static bool retract(M &model, SamplerExecutionContext<E> &context)
   {
      if (C0::retract(model, context)) return true;
      if (C1::retract(model, context)) return true;
      if (C2::retract(model, context)) return true;
      if (C3::retract(model, context)) return true;
      if (C4::retract(model, context)) return true;
      if (C5::retract(model, context)) return true;
      if (C6::retract(model, context)) return true;
      if (C7::retract(model, context)) return true;
      if (C8::retract(model, context)) return true;
      if (C9::retract(model, context)) return true;
      if (C10::retract(model, context)) return true;
      if (C11::retract(model, context)) return true;
      return false;
   };

   template<typename M, typename E>
   static void sampleSat(M &model, SamplerExecutionContext<E> &context)
   {
      double probs[12] = {
      C0::ppSat(model, context),
      C1::ppSat(model, context),
      C2::ppSat(model, context),
      C3::ppSat(model, context),
      C4::ppSat(model, context),
      C5::ppSat(model, context),
      C6::ppSat(model, context),
      C7::ppSat(model, context),
      C8::ppSat(model, context),
      C9::ppSat(model, context),
      C10::ppSat(model, context),
      C11::ppSat(model, context)
      };
      int choice = rndChoice<12>(probs, context.rndSeed);
      switch (choice)
      {
         case 0 : C0::sampleSat(model, context); break;
         case 1 : C1::sampleSat(model, context); break;
         case 2 : C2::sampleSat(model, context); break;
         case 3 : C3::sampleSat(model, context); break;
         case 4 : C4::sampleSat(model, context); break;
         case 5 : C5::sampleSat(model, context); break;
         case 6 : C6::sampleSat(model, context); break;
         case 7 : C7::sampleSat(model, context); break;
         case 8 : C8::sampleSat(model, context); break;
         case 9 : C9::sampleSat(model, context); break;
         case 10 : C10::sampleSat(model, context); break;
         case 11 : C11::sampleSat(model, context); break;
      }
   };

};

template<class C0, class C1, class C2, class C3, class C4, class C5, class C6, class C7, class C8, class C9, class C10, class C11, class C12>
class IAND13
{
public:

   template<typename M, typename E>
   static double ppSat(M &model, SamplerExecutionContext<E> &context)
   {
      return C0::ppSat(model, context)
      * C1::ppSat(model, context)
      * C2::ppSat(model, context)
      * C3::ppSat(model, context)
      * C4::ppSat(model, context)
      * C5::ppSat(model, context)
      * C6::ppSat(model, context)
      * C7::ppSat(model, context)
      * C8::ppSat(model, context)
      * C9::ppSat(model, context)
      * C10::ppSat(model, context)
      * C11::ppSat(model, context)
      * C12::ppSat(model, context);
   };

   template<typename M, typename E>
   static bool retract(M &model, SamplerExecutionContext<E> &context)
   {
      bool res = false;
      bool tmp = false;
      tmp = C0::retract(model, context); res = (res||tmp);
      tmp = C1::retract(model, context); res = (res||tmp);
      tmp = C2::retract(model, context); res = (res||tmp);
      tmp = C3::retract(model, context); res = (res||tmp);
      tmp = C4::retract(model, context); res = (res||tmp);
      tmp = C5::retract(model, context); res = (res||tmp);
      tmp = C6::retract(model, context); res = (res||tmp);
      tmp = C7::retract(model, context); res = (res||tmp);
      tmp = C8::retract(model, context); res = (res||tmp);
      tmp = C9::retract(model, context); res = (res||tmp);
      tmp = C10::retract(model, context); res = (res||tmp);
      tmp = C11::retract(model, context); res = (res||tmp);
      tmp = C12::retract(model, context); res = (res||tmp);
      return res;
   };

   template<typename M, typename E>
   static void sampleSat(M &model, SamplerExecutionContext<E> &context)
   {
      C0::sampleSat(model, context);
      C1::sampleSat(model, context);
      C2::sampleSat(model, context);
      C3::sampleSat(model, context);
      C4::sampleSat(model, context);
      C5::sampleSat(model, context);
      C6::sampleSat(model, context);
      C7::sampleSat(model, context);
      C8::sampleSat(model, context);
      C9::sampleSat(model, context);
      C10::sampleSat(model, context);
      C11::sampleSat(model, context);
      C12::sampleSat(model, context);
   };

};

template<class C0, class C1, class C2, class C3, class C4, class C5, class C6, class C7, class C8, class C9, class C10, class C11, class C12>
class EXOR13
{
public:

   template<typename M, typename E>
   static double ppSat(M &model, SamplerExecutionContext<E> &context)
   {
      return C0::ppSat(model, context)
      + C1::ppSat(model, context)
      + C2::ppSat(model, context)
      + C3::ppSat(model, context)
      + C4::ppSat(model, context)
      + C5::ppSat(model, context)
      + C6::ppSat(model, context)
      + C7::ppSat(model, context)
      + C8::ppSat(model, context)
      + C9::ppSat(model, context)
      + C10::ppSat(model, context)
      + C11::ppSat(model, context)
      + C12::ppSat(model, context);
   };

   template<typename M, typename E>
   static bool retract(M &model, SamplerExecutionContext<E> &context)
   {
      if (C0::retract(model, context)) return true;
      if (C1::retract(model, context)) return true;
      if (C2::retract(model, context)) return true;
      if (C3::retract(model, context)) return true;
      if (C4::retract(model, context)) return true;
      if (C5::retract(model, context)) return true;
      if (C6::retract(model, context)) return true;
      if (C7::retract(model, context)) return true;
      if (C8::retract(model, context)) return true;
      if (C9::retract(model, context)) return true;
      if (C10::retract(model, context)) return true;
      if (C11::retract(model, context)) return true;
      if (C12::retract(model, context)) return true;
      return false;
   };

   template<typename M, typename E>
   static void sampleSat(M &model, SamplerExecutionContext<E> &context)
   {
      double probs[13] = {
      C0::ppSat(model, context),
      C1::ppSat(model, context),
      C2::ppSat(model, context),
      C3::ppSat(model, context),
      C4::ppSat(model, context),
      C5::ppSat(model, context),
      C6::ppSat(model, context),
      C7::ppSat(model, context),
      C8::ppSat(model, context),
      C9::ppSat(model, context),
      C10::ppSat(model, context),
      C11::ppSat(model, context),
      C12::ppSat(model, context)
      };
      int choice = rndChoice<13>(probs, context.rndSeed);
      switch (choice)
      {
         case 0 : C0::sampleSat(model, context); break;
         case 1 : C1::sampleSat(model, context); break;
         case 2 : C2::sampleSat(model, context); break;
         case 3 : C3::sampleSat(model, context); break;
         case 4 : C4::sampleSat(model, context); break;
         case 5 : C5::sampleSat(model, context); break;
         case 6 : C6::sampleSat(model, context); break;
         case 7 : C7::sampleSat(model, context); break;
         case 8 : C8::sampleSat(model, context); break;
         case 9 : C9::sampleSat(model, context); break;
         case 10 : C10::sampleSat(model, context); break;
         case 11 : C11::sampleSat(model, context); break;
         case 12 : C12::sampleSat(model, context); break;
      }
   };

};

template<class C0, class C1, class C2, class C3, class C4, class C5, class C6, class C7, class C8, class C9, class C10, class C11, class C12, class C13>
class IAND14
{
public:

   template<typename M, typename E>
   static double ppSat(M &model, SamplerExecutionContext<E> &context)
   {
      return C0::ppSat(model, context)
      * C1::ppSat(model, context)
      * C2::ppSat(model, context)
      * C3::ppSat(model, context)
      * C4::ppSat(model, context)
      * C5::ppSat(model, context)
      * C6::ppSat(model, context)
      * C7::ppSat(model, context)
      * C8::ppSat(model, context)
      * C9::ppSat(model, context)
      * C10::ppSat(model, context)
      * C11::ppSat(model, context)
      * C12::ppSat(model, context)
      * C13::ppSat(model, context);
   };

   template<typename M, typename E>
   static bool retract(M &model, SamplerExecutionContext<E> &context)
   {
      bool res = false;
      bool tmp = false;
      tmp = C0::retract(model, context); res = (res||tmp);
      tmp = C1::retract(model, context); res = (res||tmp);
      tmp = C2::retract(model, context); res = (res||tmp);
      tmp = C3::retract(model, context); res = (res||tmp);
      tmp = C4::retract(model, context); res = (res||tmp);
      tmp = C5::retract(model, context); res = (res||tmp);
      tmp = C6::retract(model, context); res = (res||tmp);
      tmp = C7::retract(model, context); res = (res||tmp);
      tmp = C8::retract(model, context); res = (res||tmp);
      tmp = C9::retract(model, context); res = (res||tmp);
      tmp = C10::retract(model, context); res = (res||tmp);
      tmp = C11::retract(model, context); res = (res||tmp);
      tmp = C12::retract(model, context); res = (res||tmp);
      tmp = C13::retract(model, context); res = (res||tmp);
      return res;
   };

   template<typename M, typename E>
   static void sampleSat(M &model, SamplerExecutionContext<E> &context)
   {
      C0::sampleSat(model, context);
      C1::sampleSat(model, context);
      C2::sampleSat(model, context);
      C3::sampleSat(model, context);
      C4::sampleSat(model, context);
      C5::sampleSat(model, context);
      C6::sampleSat(model, context);
      C7::sampleSat(model, context);
      C8::sampleSat(model, context);
      C9::sampleSat(model, context);
      C10::sampleSat(model, context);
      C11::sampleSat(model, context);
      C12::sampleSat(model, context);
      C13::sampleSat(model, context);
   };

};

template<class C0, class C1, class C2, class C3, class C4, class C5, class C6, class C7, class C8, class C9, class C10, class C11, class C12, class C13>
class EXOR14
{
public:

   template<typename M, typename E>
   static double ppSat(M &model, SamplerExecutionContext<E> &context)
   {
      return C0::ppSat(model, context)
      + C1::ppSat(model, context)
      + C2::ppSat(model, context)
      + C3::ppSat(model, context)
      + C4::ppSat(model, context)
      + C5::ppSat(model, context)
      + C6::ppSat(model, context)
      + C7::ppSat(model, context)
      + C8::ppSat(model, context)
      + C9::ppSat(model, context)
      + C10::ppSat(model, context)
      + C11::ppSat(model, context)
      + C12::ppSat(model, context)
      + C13::ppSat(model, context);
   };

   template<typename M, typename E>
   static bool retract(M &model, SamplerExecutionContext<E> &context)
   {
      if (C0::retract(model, context)) return true;
      if (C1::retract(model, context)) return true;
      if (C2::retract(model, context)) return true;
      if (C3::retract(model, context)) return true;
      if (C4::retract(model, context)) return true;
      if (C5::retract(model, context)) return true;
      if (C6::retract(model, context)) return true;
      if (C7::retract(model, context)) return true;
      if (C8::retract(model, context)) return true;
      if (C9::retract(model, context)) return true;
      if (C10::retract(model, context)) return true;
      if (C11::retract(model, context)) return true;
      if (C12::retract(model, context)) return true;
      if (C13::retract(model, context)) return true;
      return false;
   };

   template<typename M, typename E>
   static void sampleSat(M &model, SamplerExecutionContext<E> &context)
   {
      double probs[14] = {
      C0::ppSat(model, context),
      C1::ppSat(model, context),
      C2::ppSat(model, context),
      C3::ppSat(model, context),
      C4::ppSat(model, context),
      C5::ppSat(model, context),
      C6::ppSat(model, context),
      C7::ppSat(model, context),
      C8::ppSat(model, context),
      C9::ppSat(model, context),
      C10::ppSat(model, context),
      C11::ppSat(model, context),
      C12::ppSat(model, context),
      C13::ppSat(model, context)
      };
      int choice = rndChoice<14>(probs, context.rndSeed);
      switch (choice)
      {
         case 0 : C0::sampleSat(model, context); break;
         case 1 : C1::sampleSat(model, context); break;
         case 2 : C2::sampleSat(model, context); break;
         case 3 : C3::sampleSat(model, context); break;
         case 4 : C4::sampleSat(model, context); break;
         case 5 : C5::sampleSat(model, context); break;
         case 6 : C6::sampleSat(model, context); break;
         case 7 : C7::sampleSat(model, context); break;
         case 8 : C8::sampleSat(model, context); break;
         case 9 : C9::sampleSat(model, context); break;
         case 10 : C10::sampleSat(model, context); break;
         case 11 : C11::sampleSat(model, context); break;
         case 12 : C12::sampleSat(model, context); break;
         case 13 : C13::sampleSat(model, context); break;
      }
   };

};

template<class C0, class C1, class C2, class C3, class C4, class C5, class C6, class C7, class C8, class C9, class C10, class C11, class C12, class C13, class C14>
class IAND15
{
public:

   template<typename M, typename E>
   static double ppSat(M &model, SamplerExecutionContext<E> &context)
   {
      return C0::ppSat(model, context)
      * C1::ppSat(model, context)
      * C2::ppSat(model, context)
      * C3::ppSat(model, context)
      * C4::ppSat(model, context)
      * C5::ppSat(model, context)
      * C6::ppSat(model, context)
      * C7::ppSat(model, context)
      * C8::ppSat(model, context)
      * C9::ppSat(model, context)
      * C10::ppSat(model, context)
      * C11::ppSat(model, context)
      * C12::ppSat(model, context)
      * C13::ppSat(model, context)
      * C14::ppSat(model, context);
   };

   template<typename M, typename E>
   static bool retract(M &model, SamplerExecutionContext<E> &context)
   {
      bool res = false;
      bool tmp = false;
      tmp = C0::retract(model, context); res = (res||tmp);
      tmp = C1::retract(model, context); res = (res||tmp);
      tmp = C2::retract(model, context); res = (res||tmp);
      tmp = C3::retract(model, context); res = (res||tmp);
      tmp = C4::retract(model, context); res = (res||tmp);
      tmp = C5::retract(model, context); res = (res||tmp);
      tmp = C6::retract(model, context); res = (res||tmp);
      tmp = C7::retract(model, context); res = (res||tmp);
      tmp = C8::retract(model, context); res = (res||tmp);
      tmp = C9::retract(model, context); res = (res||tmp);
      tmp = C10::retract(model, context); res = (res||tmp);
      tmp = C11::retract(model, context); res = (res||tmp);
      tmp = C12::retract(model, context); res = (res||tmp);
      tmp = C13::retract(model, context); res = (res||tmp);
      tmp = C14::retract(model, context); res = (res||tmp);
      return res;
   };

   template<typename M, typename E>
   static void sampleSat(M &model, SamplerExecutionContext<E> &context)
   {
      C0::sampleSat(model, context);
      C1::sampleSat(model, context);
      C2::sampleSat(model, context);
      C3::sampleSat(model, context);
      C4::sampleSat(model, context);
      C5::sampleSat(model, context);
      C6::sampleSat(model, context);
      C7::sampleSat(model, context);
      C8::sampleSat(model, context);
      C9::sampleSat(model, context);
      C10::sampleSat(model, context);
      C11::sampleSat(model, context);
      C12::sampleSat(model, context);
      C13::sampleSat(model, context);
      C14::sampleSat(model, context);
   };

};

template<class C0, class C1, class C2, class C3, class C4, class C5, class C6, class C7, class C8, class C9, class C10, class C11, class C12, class C13, class C14>
class EXOR15
{
public:

   template<typename M, typename E>
   static double ppSat(M &model, SamplerExecutionContext<E> &context)
   {
      return C0::ppSat(model, context)
      + C1::ppSat(model, context)
      + C2::ppSat(model, context)
      + C3::ppSat(model, context)
      + C4::ppSat(model, context)
      + C5::ppSat(model, context)
      + C6::ppSat(model, context)
      + C7::ppSat(model, context)
      + C8::ppSat(model, context)
      + C9::ppSat(model, context)
      + C10::ppSat(model, context)
      + C11::ppSat(model, context)
      + C12::ppSat(model, context)
      + C13::ppSat(model, context)
      + C14::ppSat(model, context);
   };

   template<typename M, typename E>
   static bool retract(M &model, SamplerExecutionContext<E> &context)
   {
      if (C0::retract(model, context)) return true;
      if (C1::retract(model, context)) return true;
      if (C2::retract(model, context)) return true;
      if (C3::retract(model, context)) return true;
      if (C4::retract(model, context)) return true;
      if (C5::retract(model, context)) return true;
      if (C6::retract(model, context)) return true;
      if (C7::retract(model, context)) return true;
      if (C8::retract(model, context)) return true;
      if (C9::retract(model, context)) return true;
      if (C10::retract(model, context)) return true;
      if (C11::retract(model, context)) return true;
      if (C12::retract(model, context)) return true;
      if (C13::retract(model, context)) return true;
      if (C14::retract(model, context)) return true;
      return false;
   };

   template<typename M, typename E>
   static void sampleSat(M &model, SamplerExecutionContext<E> &context)
   {
      double probs[15] = {
      C0::ppSat(model, context),
      C1::ppSat(model, context),
      C2::ppSat(model, context),
      C3::ppSat(model, context),
      C4::ppSat(model, context),
      C5::ppSat(model, context),
      C6::ppSat(model, context),
      C7::ppSat(model, context),
      C8::ppSat(model, context),
      C9::ppSat(model, context),
      C10::ppSat(model, context),
      C11::ppSat(model, context),
      C12::ppSat(model, context),
      C13::ppSat(model, context),
      C14::ppSat(model, context)
      };
      int choice = rndChoice<15>(probs, context.rndSeed);
      switch (choice)
      {
         case 0 : C0::sampleSat(model, context); break;
         case 1 : C1::sampleSat(model, context); break;
         case 2 : C2::sampleSat(model, context); break;
         case 3 : C3::sampleSat(model, context); break;
         case 4 : C4::sampleSat(model, context); break;
         case 5 : C5::sampleSat(model, context); break;
         case 6 : C6::sampleSat(model, context); break;
         case 7 : C7::sampleSat(model, context); break;
         case 8 : C8::sampleSat(model, context); break;
         case 9 : C9::sampleSat(model, context); break;
         case 10 : C10::sampleSat(model, context); break;
         case 11 : C11::sampleSat(model, context); break;
         case 12 : C12::sampleSat(model, context); break;
         case 13 : C13::sampleSat(model, context); break;
         case 14 : C14::sampleSat(model, context); break;
      }
   };

};

template<class C0, class C1, class C2, class C3, class C4, class C5, class C6, class C7, class C8, class C9, class C10, class C11, class C12, class C13, class C14, class C15>
class IAND16
{
public:

   template<typename M, typename E>
   static double ppSat(M &model, SamplerExecutionContext<E> &context)
   {
      return C0::ppSat(model, context)
      * C1::ppSat(model, context)
      * C2::ppSat(model, context)
      * C3::ppSat(model, context)
      * C4::ppSat(model, context)
      * C5::ppSat(model, context)
      * C6::ppSat(model, context)
      * C7::ppSat(model, context)
      * C8::ppSat(model, context)
      * C9::ppSat(model, context)
      * C10::ppSat(model, context)
      * C11::ppSat(model, context)
      * C12::ppSat(model, context)
      * C13::ppSat(model, context)
      * C14::ppSat(model, context)
      * C15::ppSat(model, context);
   };

   template<typename M, typename E>
   static bool retract(M &model, SamplerExecutionContext<E> &context)
   {
      bool res = false;
      bool tmp = false;
      tmp = C0::retract(model, context); res = (res||tmp);
      tmp = C1::retract(model, context); res = (res||tmp);
      tmp = C2::retract(model, context); res = (res||tmp);
      tmp = C3::retract(model, context); res = (res||tmp);
      tmp = C4::retract(model, context); res = (res||tmp);
      tmp = C5::retract(model, context); res = (res||tmp);
      tmp = C6::retract(model, context); res = (res||tmp);
      tmp = C7::retract(model, context); res = (res||tmp);
      tmp = C8::retract(model, context); res = (res||tmp);
      tmp = C9::retract(model, context); res = (res||tmp);
      tmp = C10::retract(model, context); res = (res||tmp);
      tmp = C11::retract(model, context); res = (res||tmp);
      tmp = C12::retract(model, context); res = (res||tmp);
      tmp = C13::retract(model, context); res = (res||tmp);
      tmp = C14::retract(model, context); res = (res||tmp);
      tmp = C15::retract(model, context); res = (res||tmp);
      return res;
   };

   template<typename M, typename E>
   static void sampleSat(M &model, SamplerExecutionContext<E> &context)
   {
      C0::sampleSat(model, context);
      C1::sampleSat(model, context);
      C2::sampleSat(model, context);
      C3::sampleSat(model, context);
      C4::sampleSat(model, context);
      C5::sampleSat(model, context);
      C6::sampleSat(model, context);
      C7::sampleSat(model, context);
      C8::sampleSat(model, context);
      C9::sampleSat(model, context);
      C10::sampleSat(model, context);
      C11::sampleSat(model, context);
      C12::sampleSat(model, context);
      C13::sampleSat(model, context);
      C14::sampleSat(model, context);
      C15::sampleSat(model, context);
   };

};

template<class C0, class C1, class C2, class C3, class C4, class C5, class C6, class C7, class C8, class C9, class C10, class C11, class C12, class C13, class C14, class C15>
class EXOR16
{
public:

   template<typename M, typename E>
   static double ppSat(M &model, SamplerExecutionContext<E> &context)
   {
      return C0::ppSat(model, context)
      + C1::ppSat(model, context)
      + C2::ppSat(model, context)
      + C3::ppSat(model, context)
      + C4::ppSat(model, context)
      + C5::ppSat(model, context)
      + C6::ppSat(model, context)
      + C7::ppSat(model, context)
      + C8::ppSat(model, context)
      + C9::ppSat(model, context)
      + C10::ppSat(model, context)
      + C11::ppSat(model, context)
      + C12::ppSat(model, context)
      + C13::ppSat(model, context)
      + C14::ppSat(model, context)
      + C15::ppSat(model, context);
   };

   template<typename M, typename E>
   static bool retract(M &model, SamplerExecutionContext<E> &context)
   {
      if (C0::retract(model, context)) return true;
      if (C1::retract(model, context)) return true;
      if (C2::retract(model, context)) return true;
      if (C3::retract(model, context)) return true;
      if (C4::retract(model, context)) return true;
      if (C5::retract(model, context)) return true;
      if (C6::retract(model, context)) return true;
      if (C7::retract(model, context)) return true;
      if (C8::retract(model, context)) return true;
      if (C9::retract(model, context)) return true;
      if (C10::retract(model, context)) return true;
      if (C11::retract(model, context)) return true;
      if (C12::retract(model, context)) return true;
      if (C13::retract(model, context)) return true;
      if (C14::retract(model, context)) return true;
      if (C15::retract(model, context)) return true;
      return false;
   };

   template<typename M, typename E>
   static void sampleSat(M &model, SamplerExecutionContext<E> &context)
   {
      double probs[16] = {
      C0::ppSat(model, context),
      C1::ppSat(model, context),
      C2::ppSat(model, context),
      C3::ppSat(model, context),
      C4::ppSat(model, context),
      C5::ppSat(model, context),
      C6::ppSat(model, context),
      C7::ppSat(model, context),
      C8::ppSat(model, context),
      C9::ppSat(model, context),
      C10::ppSat(model, context),
      C11::ppSat(model, context),
      C12::ppSat(model, context),
      C13::ppSat(model, context),
      C14::ppSat(model, context),
      C15::ppSat(model, context)
      };
      int choice = rndChoice<16>(probs, context.rndSeed);
      switch (choice)
      {
         case 0 : C0::sampleSat(model, context); break;
         case 1 : C1::sampleSat(model, context); break;
         case 2 : C2::sampleSat(model, context); break;
         case 3 : C3::sampleSat(model, context); break;
         case 4 : C4::sampleSat(model, context); break;
         case 5 : C5::sampleSat(model, context); break;
         case 6 : C6::sampleSat(model, context); break;
         case 7 : C7::sampleSat(model, context); break;
         case 8 : C8::sampleSat(model, context); break;
         case 9 : C9::sampleSat(model, context); break;
         case 10 : C10::sampleSat(model, context); break;
         case 11 : C11::sampleSat(model, context); break;
         case 12 : C12::sampleSat(model, context); break;
         case 13 : C13::sampleSat(model, context); break;
         case 14 : C14::sampleSat(model, context); break;
         case 15 : C15::sampleSat(model, context); break;
      }
   };

};

template<class C0, class C1, class C2, class C3, class C4, class C5, class C6, class C7, class C8, class C9, class C10, class C11, class C12, class C13, class C14, class C15, class C16>
class IAND17
{
public:

   template<typename M, typename E>
   static double ppSat(M &model, SamplerExecutionContext<E> &context)
   {
      return C0::ppSat(model, context)
      * C1::ppSat(model, context)
      * C2::ppSat(model, context)
      * C3::ppSat(model, context)
      * C4::ppSat(model, context)
      * C5::ppSat(model, context)
      * C6::ppSat(model, context)
      * C7::ppSat(model, context)
      * C8::ppSat(model, context)
      * C9::ppSat(model, context)
      * C10::ppSat(model, context)
      * C11::ppSat(model, context)
      * C12::ppSat(model, context)
      * C13::ppSat(model, context)
      * C14::ppSat(model, context)
      * C15::ppSat(model, context)
      * C16::ppSat(model, context);
   };

   template<typename M, typename E>
   static bool retract(M &model, SamplerExecutionContext<E> &context)
   {
      bool res = false;
      bool tmp = false;
      tmp = C0::retract(model, context); res = (res||tmp);
      tmp = C1::retract(model, context); res = (res||tmp);
      tmp = C2::retract(model, context); res = (res||tmp);
      tmp = C3::retract(model, context); res = (res||tmp);
      tmp = C4::retract(model, context); res = (res||tmp);
      tmp = C5::retract(model, context); res = (res||tmp);
      tmp = C6::retract(model, context); res = (res||tmp);
      tmp = C7::retract(model, context); res = (res||tmp);
      tmp = C8::retract(model, context); res = (res||tmp);
      tmp = C9::retract(model, context); res = (res||tmp);
      tmp = C10::retract(model, context); res = (res||tmp);
      tmp = C11::retract(model, context); res = (res||tmp);
      tmp = C12::retract(model, context); res = (res||tmp);
      tmp = C13::retract(model, context); res = (res||tmp);
      tmp = C14::retract(model, context); res = (res||tmp);
      tmp = C15::retract(model, context); res = (res||tmp);
      tmp = C16::retract(model, context); res = (res||tmp);
      return res;
   };

   template<typename M, typename E>
   static void sampleSat(M &model, SamplerExecutionContext<E> &context)
   {
      C0::sampleSat(model, context);
      C1::sampleSat(model, context);
      C2::sampleSat(model, context);
      C3::sampleSat(model, context);
      C4::sampleSat(model, context);
      C5::sampleSat(model, context);
      C6::sampleSat(model, context);
      C7::sampleSat(model, context);
      C8::sampleSat(model, context);
      C9::sampleSat(model, context);
      C10::sampleSat(model, context);
      C11::sampleSat(model, context);
      C12::sampleSat(model, context);
      C13::sampleSat(model, context);
      C14::sampleSat(model, context);
      C15::sampleSat(model, context);
      C16::sampleSat(model, context);
   };

};

template<class C0, class C1, class C2, class C3, class C4, class C5, class C6, class C7, class C8, class C9, class C10, class C11, class C12, class C13, class C14, class C15, class C16>
class EXOR17
{
public:

   template<typename M, typename E>
   static double ppSat(M &model, SamplerExecutionContext<E> &context)
   {
      return C0::ppSat(model, context)
      + C1::ppSat(model, context)
      + C2::ppSat(model, context)
      + C3::ppSat(model, context)
      + C4::ppSat(model, context)
      + C5::ppSat(model, context)
      + C6::ppSat(model, context)
      + C7::ppSat(model, context)
      + C8::ppSat(model, context)
      + C9::ppSat(model, context)
      + C10::ppSat(model, context)
      + C11::ppSat(model, context)
      + C12::ppSat(model, context)
      + C13::ppSat(model, context)
      + C14::ppSat(model, context)
      + C15::ppSat(model, context)
      + C16::ppSat(model, context);
   };

   template<typename M, typename E>
   static bool retract(M &model, SamplerExecutionContext<E> &context)
   {
      if (C0::retract(model, context)) return true;
      if (C1::retract(model, context)) return true;
      if (C2::retract(model, context)) return true;
      if (C3::retract(model, context)) return true;
      if (C4::retract(model, context)) return true;
      if (C5::retract(model, context)) return true;
      if (C6::retract(model, context)) return true;
      if (C7::retract(model, context)) return true;
      if (C8::retract(model, context)) return true;
      if (C9::retract(model, context)) return true;
      if (C10::retract(model, context)) return true;
      if (C11::retract(model, context)) return true;
      if (C12::retract(model, context)) return true;
      if (C13::retract(model, context)) return true;
      if (C14::retract(model, context)) return true;
      if (C15::retract(model, context)) return true;
      if (C16::retract(model, context)) return true;
      return false;
   };

   template<typename M, typename E>
   static void sampleSat(M &model, SamplerExecutionContext<E> &context)
   {
      double probs[17] = {
      C0::ppSat(model, context),
      C1::ppSat(model, context),
      C2::ppSat(model, context),
      C3::ppSat(model, context),
      C4::ppSat(model, context),
      C5::ppSat(model, context),
      C6::ppSat(model, context),
      C7::ppSat(model, context),
      C8::ppSat(model, context),
      C9::ppSat(model, context),
      C10::ppSat(model, context),
      C11::ppSat(model, context),
      C12::ppSat(model, context),
      C13::ppSat(model, context),
      C14::ppSat(model, context),
      C15::ppSat(model, context),
      C16::ppSat(model, context)
      };
      int choice = rndChoice<17>(probs, context.rndSeed);
      switch (choice)
      {
         case 0 : C0::sampleSat(model, context); break;
         case 1 : C1::sampleSat(model, context); break;
         case 2 : C2::sampleSat(model, context); break;
         case 3 : C3::sampleSat(model, context); break;
         case 4 : C4::sampleSat(model, context); break;
         case 5 : C5::sampleSat(model, context); break;
         case 6 : C6::sampleSat(model, context); break;
         case 7 : C7::sampleSat(model, context); break;
         case 8 : C8::sampleSat(model, context); break;
         case 9 : C9::sampleSat(model, context); break;
         case 10 : C10::sampleSat(model, context); break;
         case 11 : C11::sampleSat(model, context); break;
         case 12 : C12::sampleSat(model, context); break;
         case 13 : C13::sampleSat(model, context); break;
         case 14 : C14::sampleSat(model, context); break;
         case 15 : C15::sampleSat(model, context); break;
         case 16 : C16::sampleSat(model, context); break;
      }
   };

};

template<class C0, class C1, class C2, class C3, class C4, class C5, class C6, class C7, class C8, class C9, class C10, class C11, class C12, class C13, class C14, class C15, class C16, class C17>
class IAND18
{
public:

   template<typename M, typename E>
   static double ppSat(M &model, SamplerExecutionContext<E> &context)
   {
      return C0::ppSat(model, context)
      * C1::ppSat(model, context)
      * C2::ppSat(model, context)
      * C3::ppSat(model, context)
      * C4::ppSat(model, context)
      * C5::ppSat(model, context)
      * C6::ppSat(model, context)
      * C7::ppSat(model, context)
      * C8::ppSat(model, context)
      * C9::ppSat(model, context)
      * C10::ppSat(model, context)
      * C11::ppSat(model, context)
      * C12::ppSat(model, context)
      * C13::ppSat(model, context)
      * C14::ppSat(model, context)
      * C15::ppSat(model, context)
      * C16::ppSat(model, context)
      * C17::ppSat(model, context);
   };

   template<typename M, typename E>
   static bool retract(M &model, SamplerExecutionContext<E> &context)
   {
      bool res = false;
      bool tmp = false;
      tmp = C0::retract(model, context); res = (res||tmp);
      tmp = C1::retract(model, context); res = (res||tmp);
      tmp = C2::retract(model, context); res = (res||tmp);
      tmp = C3::retract(model, context); res = (res||tmp);
      tmp = C4::retract(model, context); res = (res||tmp);
      tmp = C5::retract(model, context); res = (res||tmp);
      tmp = C6::retract(model, context); res = (res||tmp);
      tmp = C7::retract(model, context); res = (res||tmp);
      tmp = C8::retract(model, context); res = (res||tmp);
      tmp = C9::retract(model, context); res = (res||tmp);
      tmp = C10::retract(model, context); res = (res||tmp);
      tmp = C11::retract(model, context); res = (res||tmp);
      tmp = C12::retract(model, context); res = (res||tmp);
      tmp = C13::retract(model, context); res = (res||tmp);
      tmp = C14::retract(model, context); res = (res||tmp);
      tmp = C15::retract(model, context); res = (res||tmp);
      tmp = C16::retract(model, context); res = (res||tmp);
      tmp = C17::retract(model, context); res = (res||tmp);
      return res;
   };

   template<typename M, typename E>
   static void sampleSat(M &model, SamplerExecutionContext<E> &context)
   {
      C0::sampleSat(model, context);
      C1::sampleSat(model, context);
      C2::sampleSat(model, context);
      C3::sampleSat(model, context);
      C4::sampleSat(model, context);
      C5::sampleSat(model, context);
      C6::sampleSat(model, context);
      C7::sampleSat(model, context);
      C8::sampleSat(model, context);
      C9::sampleSat(model, context);
      C10::sampleSat(model, context);
      C11::sampleSat(model, context);
      C12::sampleSat(model, context);
      C13::sampleSat(model, context);
      C14::sampleSat(model, context);
      C15::sampleSat(model, context);
      C16::sampleSat(model, context);
      C17::sampleSat(model, context);
   };

};

template<class C0, class C1, class C2, class C3, class C4, class C5, class C6, class C7, class C8, class C9, class C10, class C11, class C12, class C13, class C14, class C15, class C16, class C17>
class EXOR18
{
public:

   template<typename M, typename E>
   static double ppSat(M &model, SamplerExecutionContext<E> &context)
   {
      return C0::ppSat(model, context)
      + C1::ppSat(model, context)
      + C2::ppSat(model, context)
      + C3::ppSat(model, context)
      + C4::ppSat(model, context)
      + C5::ppSat(model, context)
      + C6::ppSat(model, context)
      + C7::ppSat(model, context)
      + C8::ppSat(model, context)
      + C9::ppSat(model, context)
      + C10::ppSat(model, context)
      + C11::ppSat(model, context)
      + C12::ppSat(model, context)
      + C13::ppSat(model, context)
      + C14::ppSat(model, context)
      + C15::ppSat(model, context)
      + C16::ppSat(model, context)
      + C17::ppSat(model, context);
   };

   template<typename M, typename E>
   static bool retract(M &model, SamplerExecutionContext<E> &context)
   {
      if (C0::retract(model, context)) return true;
      if (C1::retract(model, context)) return true;
      if (C2::retract(model, context)) return true;
      if (C3::retract(model, context)) return true;
      if (C4::retract(model, context)) return true;
      if (C5::retract(model, context)) return true;
      if (C6::retract(model, context)) return true;
      if (C7::retract(model, context)) return true;
      if (C8::retract(model, context)) return true;
      if (C9::retract(model, context)) return true;
      if (C10::retract(model, context)) return true;
      if (C11::retract(model, context)) return true;
      if (C12::retract(model, context)) return true;
      if (C13::retract(model, context)) return true;
      if (C14::retract(model, context)) return true;
      if (C15::retract(model, context)) return true;
      if (C16::retract(model, context)) return true;
      if (C17::retract(model, context)) return true;
      return false;
   };

   template<typename M, typename E>
   static void sampleSat(M &model, SamplerExecutionContext<E> &context)
   {
      double probs[18] = {
      C0::ppSat(model, context),
      C1::ppSat(model, context),
      C2::ppSat(model, context),
      C3::ppSat(model, context),
      C4::ppSat(model, context),
      C5::ppSat(model, context),
      C6::ppSat(model, context),
      C7::ppSat(model, context),
      C8::ppSat(model, context),
      C9::ppSat(model, context),
      C10::ppSat(model, context),
      C11::ppSat(model, context),
      C12::ppSat(model, context),
      C13::ppSat(model, context),
      C14::ppSat(model, context),
      C15::ppSat(model, context),
      C16::ppSat(model, context),
      C17::ppSat(model, context)
      };
      int choice = rndChoice<18>(probs, context.rndSeed);
      switch (choice)
      {
         case 0 : C0::sampleSat(model, context); break;
         case 1 : C1::sampleSat(model, context); break;
         case 2 : C2::sampleSat(model, context); break;
         case 3 : C3::sampleSat(model, context); break;
         case 4 : C4::sampleSat(model, context); break;
         case 5 : C5::sampleSat(model, context); break;
         case 6 : C6::sampleSat(model, context); break;
         case 7 : C7::sampleSat(model, context); break;
         case 8 : C8::sampleSat(model, context); break;
         case 9 : C9::sampleSat(model, context); break;
         case 10 : C10::sampleSat(model, context); break;
         case 11 : C11::sampleSat(model, context); break;
         case 12 : C12::sampleSat(model, context); break;
         case 13 : C13::sampleSat(model, context); break;
         case 14 : C14::sampleSat(model, context); break;
         case 15 : C15::sampleSat(model, context); break;
         case 16 : C16::sampleSat(model, context); break;
         case 17 : C17::sampleSat(model, context); break;
      }
   };

};

template<class C0, class C1, class C2, class C3, class C4, class C5, class C6, class C7, class C8, class C9, class C10, class C11, class C12, class C13, class C14, class C15, class C16, class C17, class C18>
class IAND19
{
public:

   template<typename M, typename E>
   static double ppSat(M &model, SamplerExecutionContext<E> &context)
   {
      return C0::ppSat(model, context)
      * C1::ppSat(model, context)
      * C2::ppSat(model, context)
      * C3::ppSat(model, context)
      * C4::ppSat(model, context)
      * C5::ppSat(model, context)
      * C6::ppSat(model, context)
      * C7::ppSat(model, context)
      * C8::ppSat(model, context)
      * C9::ppSat(model, context)
      * C10::ppSat(model, context)
      * C11::ppSat(model, context)
      * C12::ppSat(model, context)
      * C13::ppSat(model, context)
      * C14::ppSat(model, context)
      * C15::ppSat(model, context)
      * C16::ppSat(model, context)
      * C17::ppSat(model, context)
      * C18::ppSat(model, context);
   };

   template<typename M, typename E>
   static bool retract(M &model, SamplerExecutionContext<E> &context)
   {
      bool res = false;
      bool tmp = false;
      tmp = C0::retract(model, context); res = (res||tmp);
      tmp = C1::retract(model, context); res = (res||tmp);
      tmp = C2::retract(model, context); res = (res||tmp);
      tmp = C3::retract(model, context); res = (res||tmp);
      tmp = C4::retract(model, context); res = (res||tmp);
      tmp = C5::retract(model, context); res = (res||tmp);
      tmp = C6::retract(model, context); res = (res||tmp);
      tmp = C7::retract(model, context); res = (res||tmp);
      tmp = C8::retract(model, context); res = (res||tmp);
      tmp = C9::retract(model, context); res = (res||tmp);
      tmp = C10::retract(model, context); res = (res||tmp);
      tmp = C11::retract(model, context); res = (res||tmp);
      tmp = C12::retract(model, context); res = (res||tmp);
      tmp = C13::retract(model, context); res = (res||tmp);
      tmp = C14::retract(model, context); res = (res||tmp);
      tmp = C15::retract(model, context); res = (res||tmp);
      tmp = C16::retract(model, context); res = (res||tmp);
      tmp = C17::retract(model, context); res = (res||tmp);
      tmp = C18::retract(model, context); res = (res||tmp);
      return res;
   };

   template<typename M, typename E>
   static void sampleSat(M &model, SamplerExecutionContext<E> &context)
   {
      C0::sampleSat(model, context);
      C1::sampleSat(model, context);
      C2::sampleSat(model, context);
      C3::sampleSat(model, context);
      C4::sampleSat(model, context);
      C5::sampleSat(model, context);
      C6::sampleSat(model, context);
      C7::sampleSat(model, context);
      C8::sampleSat(model, context);
      C9::sampleSat(model, context);
      C10::sampleSat(model, context);
      C11::sampleSat(model, context);
      C12::sampleSat(model, context);
      C13::sampleSat(model, context);
      C14::sampleSat(model, context);
      C15::sampleSat(model, context);
      C16::sampleSat(model, context);
      C17::sampleSat(model, context);
      C18::sampleSat(model, context);
   };

};

template<class C0, class C1, class C2, class C3, class C4, class C5, class C6, class C7, class C8, class C9, class C10, class C11, class C12, class C13, class C14, class C15, class C16, class C17, class C18>
class EXOR19
{
public:

   template<typename M, typename E>
   static double ppSat(M &model, SamplerExecutionContext<E> &context)
   {
      return C0::ppSat(model, context)
      + C1::ppSat(model, context)
      + C2::ppSat(model, context)
      + C3::ppSat(model, context)
      + C4::ppSat(model, context)
      + C5::ppSat(model, context)
      + C6::ppSat(model, context)
      + C7::ppSat(model, context)
      + C8::ppSat(model, context)
      + C9::ppSat(model, context)
      + C10::ppSat(model, context)
      + C11::ppSat(model, context)
      + C12::ppSat(model, context)
      + C13::ppSat(model, context)
      + C14::ppSat(model, context)
      + C15::ppSat(model, context)
      + C16::ppSat(model, context)
      + C17::ppSat(model, context)
      + C18::ppSat(model, context);
   };

   template<typename M, typename E>
   static bool retract(M &model, SamplerExecutionContext<E> &context)
   {
      if (C0::retract(model, context)) return true;
      if (C1::retract(model, context)) return true;
      if (C2::retract(model, context)) return true;
      if (C3::retract(model, context)) return true;
      if (C4::retract(model, context)) return true;
      if (C5::retract(model, context)) return true;
      if (C6::retract(model, context)) return true;
      if (C7::retract(model, context)) return true;
      if (C8::retract(model, context)) return true;
      if (C9::retract(model, context)) return true;
      if (C10::retract(model, context)) return true;
      if (C11::retract(model, context)) return true;
      if (C12::retract(model, context)) return true;
      if (C13::retract(model, context)) return true;
      if (C14::retract(model, context)) return true;
      if (C15::retract(model, context)) return true;
      if (C16::retract(model, context)) return true;
      if (C17::retract(model, context)) return true;
      if (C18::retract(model, context)) return true;
      return false;
   };

   template<typename M, typename E>
   static void sampleSat(M &model, SamplerExecutionContext<E> &context)
   {
      double probs[19] = {
      C0::ppSat(model, context),
      C1::ppSat(model, context),
      C2::ppSat(model, context),
      C3::ppSat(model, context),
      C4::ppSat(model, context),
      C5::ppSat(model, context),
      C6::ppSat(model, context),
      C7::ppSat(model, context),
      C8::ppSat(model, context),
      C9::ppSat(model, context),
      C10::ppSat(model, context),
      C11::ppSat(model, context),
      C12::ppSat(model, context),
      C13::ppSat(model, context),
      C14::ppSat(model, context),
      C15::ppSat(model, context),
      C16::ppSat(model, context),
      C17::ppSat(model, context),
      C18::ppSat(model, context)
      };
      int choice = rndChoice<19>(probs, context.rndSeed);
      switch (choice)
      {
         case 0 : C0::sampleSat(model, context); break;
         case 1 : C1::sampleSat(model, context); break;
         case 2 : C2::sampleSat(model, context); break;
         case 3 : C3::sampleSat(model, context); break;
         case 4 : C4::sampleSat(model, context); break;
         case 5 : C5::sampleSat(model, context); break;
         case 6 : C6::sampleSat(model, context); break;
         case 7 : C7::sampleSat(model, context); break;
         case 8 : C8::sampleSat(model, context); break;
         case 9 : C9::sampleSat(model, context); break;
         case 10 : C10::sampleSat(model, context); break;
         case 11 : C11::sampleSat(model, context); break;
         case 12 : C12::sampleSat(model, context); break;
         case 13 : C13::sampleSat(model, context); break;
         case 14 : C14::sampleSat(model, context); break;
         case 15 : C15::sampleSat(model, context); break;
         case 16 : C16::sampleSat(model, context); break;
         case 17 : C17::sampleSat(model, context); break;
         case 18 : C18::sampleSat(model, context); break;
      }
   };

};

template<class C0, class C1, class C2, class C3, class C4, class C5, class C6, class C7, class C8, class C9, class C10, class C11, class C12, class C13, class C14, class C15, class C16, class C17, class C18, class C19>
class IAND20
{
public:

   template<typename M, typename E>
   static double ppSat(M &model, SamplerExecutionContext<E> &context)
   {
      return C0::ppSat(model, context)
      * C1::ppSat(model, context)
      * C2::ppSat(model, context)
      * C3::ppSat(model, context)
      * C4::ppSat(model, context)
      * C5::ppSat(model, context)
      * C6::ppSat(model, context)
      * C7::ppSat(model, context)
      * C8::ppSat(model, context)
      * C9::ppSat(model, context)
      * C10::ppSat(model, context)
      * C11::ppSat(model, context)
      * C12::ppSat(model, context)
      * C13::ppSat(model, context)
      * C14::ppSat(model, context)
      * C15::ppSat(model, context)
      * C16::ppSat(model, context)
      * C17::ppSat(model, context)
      * C18::ppSat(model, context)
      * C19::ppSat(model, context);
   };

   template<typename M, typename E>
   static bool retract(M &model, SamplerExecutionContext<E> &context)
   {
      bool res = false;
      bool tmp = false;
      tmp = C0::retract(model, context); res = (res||tmp);
      tmp = C1::retract(model, context); res = (res||tmp);
      tmp = C2::retract(model, context); res = (res||tmp);
      tmp = C3::retract(model, context); res = (res||tmp);
      tmp = C4::retract(model, context); res = (res||tmp);
      tmp = C5::retract(model, context); res = (res||tmp);
      tmp = C6::retract(model, context); res = (res||tmp);
      tmp = C7::retract(model, context); res = (res||tmp);
      tmp = C8::retract(model, context); res = (res||tmp);
      tmp = C9::retract(model, context); res = (res||tmp);
      tmp = C10::retract(model, context); res = (res||tmp);
      tmp = C11::retract(model, context); res = (res||tmp);
      tmp = C12::retract(model, context); res = (res||tmp);
      tmp = C13::retract(model, context); res = (res||tmp);
      tmp = C14::retract(model, context); res = (res||tmp);
      tmp = C15::retract(model, context); res = (res||tmp);
      tmp = C16::retract(model, context); res = (res||tmp);
      tmp = C17::retract(model, context); res = (res||tmp);
      tmp = C18::retract(model, context); res = (res||tmp);
      tmp = C19::retract(model, context); res = (res||tmp);
      return res;
   };

   template<typename M, typename E>
   static void sampleSat(M &model, SamplerExecutionContext<E> &context)
   {
      C0::sampleSat(model, context);
      C1::sampleSat(model, context);
      C2::sampleSat(model, context);
      C3::sampleSat(model, context);
      C4::sampleSat(model, context);
      C5::sampleSat(model, context);
      C6::sampleSat(model, context);
      C7::sampleSat(model, context);
      C8::sampleSat(model, context);
      C9::sampleSat(model, context);
      C10::sampleSat(model, context);
      C11::sampleSat(model, context);
      C12::sampleSat(model, context);
      C13::sampleSat(model, context);
      C14::sampleSat(model, context);
      C15::sampleSat(model, context);
      C16::sampleSat(model, context);
      C17::sampleSat(model, context);
      C18::sampleSat(model, context);
      C19::sampleSat(model, context);
   };

};

template<class C0, class C1, class C2, class C3, class C4, class C5, class C6, class C7, class C8, class C9, class C10, class C11, class C12, class C13, class C14, class C15, class C16, class C17, class C18, class C19>
class EXOR20
{
public:

   template<typename M, typename E>
   static double ppSat(M &model, SamplerExecutionContext<E> &context)
   {
      return C0::ppSat(model, context)
      + C1::ppSat(model, context)
      + C2::ppSat(model, context)
      + C3::ppSat(model, context)
      + C4::ppSat(model, context)
      + C5::ppSat(model, context)
      + C6::ppSat(model, context)
      + C7::ppSat(model, context)
      + C8::ppSat(model, context)
      + C9::ppSat(model, context)
      + C10::ppSat(model, context)
      + C11::ppSat(model, context)
      + C12::ppSat(model, context)
      + C13::ppSat(model, context)
      + C14::ppSat(model, context)
      + C15::ppSat(model, context)
      + C16::ppSat(model, context)
      + C17::ppSat(model, context)
      + C18::ppSat(model, context)
      + C19::ppSat(model, context);
   };

   template<typename M, typename E>
   static bool retract(M &model, SamplerExecutionContext<E> &context)
   {
      if (C0::retract(model, context)) return true;
      if (C1::retract(model, context)) return true;
      if (C2::retract(model, context)) return true;
      if (C3::retract(model, context)) return true;
      if (C4::retract(model, context)) return true;
      if (C5::retract(model, context)) return true;
      if (C6::retract(model, context)) return true;
      if (C7::retract(model, context)) return true;
      if (C8::retract(model, context)) return true;
      if (C9::retract(model, context)) return true;
      if (C10::retract(model, context)) return true;
      if (C11::retract(model, context)) return true;
      if (C12::retract(model, context)) return true;
      if (C13::retract(model, context)) return true;
      if (C14::retract(model, context)) return true;
      if (C15::retract(model, context)) return true;
      if (C16::retract(model, context)) return true;
      if (C17::retract(model, context)) return true;
      if (C18::retract(model, context)) return true;
      if (C19::retract(model, context)) return true;
      return false;
   };

   template<typename M, typename E>
   static void sampleSat(M &model, SamplerExecutionContext<E> &context)
   {
      double probs[20] = {
      C0::ppSat(model, context),
      C1::ppSat(model, context),
      C2::ppSat(model, context),
      C3::ppSat(model, context),
      C4::ppSat(model, context),
      C5::ppSat(model, context),
      C6::ppSat(model, context),
      C7::ppSat(model, context),
      C8::ppSat(model, context),
      C9::ppSat(model, context),
      C10::ppSat(model, context),
      C11::ppSat(model, context),
      C12::ppSat(model, context),
      C13::ppSat(model, context),
      C14::ppSat(model, context),
      C15::ppSat(model, context),
      C16::ppSat(model, context),
      C17::ppSat(model, context),
      C18::ppSat(model, context),
      C19::ppSat(model, context)
      };
      int choice = rndChoice<20>(probs, context.rndSeed);
      switch (choice)
      {
         case 0 : C0::sampleSat(model, context); break;
         case 1 : C1::sampleSat(model, context); break;
         case 2 : C2::sampleSat(model, context); break;
         case 3 : C3::sampleSat(model, context); break;
         case 4 : C4::sampleSat(model, context); break;
         case 5 : C5::sampleSat(model, context); break;
         case 6 : C6::sampleSat(model, context); break;
         case 7 : C7::sampleSat(model, context); break;
         case 8 : C8::sampleSat(model, context); break;
         case 9 : C9::sampleSat(model, context); break;
         case 10 : C10::sampleSat(model, context); break;
         case 11 : C11::sampleSat(model, context); break;
         case 12 : C12::sampleSat(model, context); break;
         case 13 : C13::sampleSat(model, context); break;
         case 14 : C14::sampleSat(model, context); break;
         case 15 : C15::sampleSat(model, context); break;
         case 16 : C16::sampleSat(model, context); break;
         case 17 : C17::sampleSat(model, context); break;
         case 18 : C18::sampleSat(model, context); break;
         case 19 : C19::sampleSat(model, context); break;
      }
   };

};

template<class C0, class C1, class C2, class C3, class C4, class C5, class C6, class C7, class C8, class C9, class C10, class C11, class C12, class C13, class C14, class C15, class C16, class C17, class C18, class C19, class C20>
class IAND21
{
public:

   template<typename M, typename E>
   static double ppSat(M &model, SamplerExecutionContext<E> &context)
   {
      return C0::ppSat(model, context)
      * C1::ppSat(model, context)
      * C2::ppSat(model, context)
      * C3::ppSat(model, context)
      * C4::ppSat(model, context)
      * C5::ppSat(model, context)
      * C6::ppSat(model, context)
      * C7::ppSat(model, context)
      * C8::ppSat(model, context)
      * C9::ppSat(model, context)
      * C10::ppSat(model, context)
      * C11::ppSat(model, context)
      * C12::ppSat(model, context)
      * C13::ppSat(model, context)
      * C14::ppSat(model, context)
      * C15::ppSat(model, context)
      * C16::ppSat(model, context)
      * C17::ppSat(model, context)
      * C18::ppSat(model, context)
      * C19::ppSat(model, context)
      * C20::ppSat(model, context);
   };

   template<typename M, typename E>
   static bool retract(M &model, SamplerExecutionContext<E> &context)
   {
      bool res = false;
      bool tmp = false;
      tmp = C0::retract(model, context); res = (res||tmp);
      tmp = C1::retract(model, context); res = (res||tmp);
      tmp = C2::retract(model, context); res = (res||tmp);
      tmp = C3::retract(model, context); res = (res||tmp);
      tmp = C4::retract(model, context); res = (res||tmp);
      tmp = C5::retract(model, context); res = (res||tmp);
      tmp = C6::retract(model, context); res = (res||tmp);
      tmp = C7::retract(model, context); res = (res||tmp);
      tmp = C8::retract(model, context); res = (res||tmp);
      tmp = C9::retract(model, context); res = (res||tmp);
      tmp = C10::retract(model, context); res = (res||tmp);
      tmp = C11::retract(model, context); res = (res||tmp);
      tmp = C12::retract(model, context); res = (res||tmp);
      tmp = C13::retract(model, context); res = (res||tmp);
      tmp = C14::retract(model, context); res = (res||tmp);
      tmp = C15::retract(model, context); res = (res||tmp);
      tmp = C16::retract(model, context); res = (res||tmp);
      tmp = C17::retract(model, context); res = (res||tmp);
      tmp = C18::retract(model, context); res = (res||tmp);
      tmp = C19::retract(model, context); res = (res||tmp);
      tmp = C20::retract(model, context); res = (res||tmp);
      return res;
   };

   template<typename M, typename E>
   static void sampleSat(M &model, SamplerExecutionContext<E> &context)
   {
      C0::sampleSat(model, context);
      C1::sampleSat(model, context);
      C2::sampleSat(model, context);
      C3::sampleSat(model, context);
      C4::sampleSat(model, context);
      C5::sampleSat(model, context);
      C6::sampleSat(model, context);
      C7::sampleSat(model, context);
      C8::sampleSat(model, context);
      C9::sampleSat(model, context);
      C10::sampleSat(model, context);
      C11::sampleSat(model, context);
      C12::sampleSat(model, context);
      C13::sampleSat(model, context);
      C14::sampleSat(model, context);
      C15::sampleSat(model, context);
      C16::sampleSat(model, context);
      C17::sampleSat(model, context);
      C18::sampleSat(model, context);
      C19::sampleSat(model, context);
      C20::sampleSat(model, context);
   };

};

template<class C0, class C1, class C2, class C3, class C4, class C5, class C6, class C7, class C8, class C9, class C10, class C11, class C12, class C13, class C14, class C15, class C16, class C17, class C18, class C19, class C20>
class EXOR21
{
public:

   template<typename M, typename E>
   static double ppSat(M &model, SamplerExecutionContext<E> &context)
   {
      return C0::ppSat(model, context)
      + C1::ppSat(model, context)
      + C2::ppSat(model, context)
      + C3::ppSat(model, context)
      + C4::ppSat(model, context)
      + C5::ppSat(model, context)
      + C6::ppSat(model, context)
      + C7::ppSat(model, context)
      + C8::ppSat(model, context)
      + C9::ppSat(model, context)
      + C10::ppSat(model, context)
      + C11::ppSat(model, context)
      + C12::ppSat(model, context)
      + C13::ppSat(model, context)
      + C14::ppSat(model, context)
      + C15::ppSat(model, context)
      + C16::ppSat(model, context)
      + C17::ppSat(model, context)
      + C18::ppSat(model, context)
      + C19::ppSat(model, context)
      + C20::ppSat(model, context);
   };

   template<typename M, typename E>
   static bool retract(M &model, SamplerExecutionContext<E> &context)
   {
      if (C0::retract(model, context)) return true;
      if (C1::retract(model, context)) return true;
      if (C2::retract(model, context)) return true;
      if (C3::retract(model, context)) return true;
      if (C4::retract(model, context)) return true;
      if (C5::retract(model, context)) return true;
      if (C6::retract(model, context)) return true;
      if (C7::retract(model, context)) return true;
      if (C8::retract(model, context)) return true;
      if (C9::retract(model, context)) return true;
      if (C10::retract(model, context)) return true;
      if (C11::retract(model, context)) return true;
      if (C12::retract(model, context)) return true;
      if (C13::retract(model, context)) return true;
      if (C14::retract(model, context)) return true;
      if (C15::retract(model, context)) return true;
      if (C16::retract(model, context)) return true;
      if (C17::retract(model, context)) return true;
      if (C18::retract(model, context)) return true;
      if (C19::retract(model, context)) return true;
      if (C20::retract(model, context)) return true;
      return false;
   };

   template<typename M, typename E>
   static void sampleSat(M &model, SamplerExecutionContext<E> &context)
   {
      double probs[21] = {
      C0::ppSat(model, context),
      C1::ppSat(model, context),
      C2::ppSat(model, context),
      C3::ppSat(model, context),
      C4::ppSat(model, context),
      C5::ppSat(model, context),
      C6::ppSat(model, context),
      C7::ppSat(model, context),
      C8::ppSat(model, context),
      C9::ppSat(model, context),
      C10::ppSat(model, context),
      C11::ppSat(model, context),
      C12::ppSat(model, context),
      C13::ppSat(model, context),
      C14::ppSat(model, context),
      C15::ppSat(model, context),
      C16::ppSat(model, context),
      C17::ppSat(model, context),
      C18::ppSat(model, context),
      C19::ppSat(model, context),
      C20::ppSat(model, context)
      };
      int choice = rndChoice<21>(probs, context.rndSeed);
      switch (choice)
      {
         case 0 : C0::sampleSat(model, context); break;
         case 1 : C1::sampleSat(model, context); break;
         case 2 : C2::sampleSat(model, context); break;
         case 3 : C3::sampleSat(model, context); break;
         case 4 : C4::sampleSat(model, context); break;
         case 5 : C5::sampleSat(model, context); break;
         case 6 : C6::sampleSat(model, context); break;
         case 7 : C7::sampleSat(model, context); break;
         case 8 : C8::sampleSat(model, context); break;
         case 9 : C9::sampleSat(model, context); break;
         case 10 : C10::sampleSat(model, context); break;
         case 11 : C11::sampleSat(model, context); break;
         case 12 : C12::sampleSat(model, context); break;
         case 13 : C13::sampleSat(model, context); break;
         case 14 : C14::sampleSat(model, context); break;
         case 15 : C15::sampleSat(model, context); break;
         case 16 : C16::sampleSat(model, context); break;
         case 17 : C17::sampleSat(model, context); break;
         case 18 : C18::sampleSat(model, context); break;
         case 19 : C19::sampleSat(model, context); break;
         case 20 : C20::sampleSat(model, context); break;
      }
   };

};

template<class C0, class C1, class C2, class C3, class C4, class C5, class C6, class C7, class C8, class C9, class C10, class C11, class C12, class C13, class C14, class C15, class C16, class C17, class C18, class C19, class C20, class C21>
class IAND22
{
public:

   template<typename M, typename E>
   static double ppSat(M &model, SamplerExecutionContext<E> &context)
   {
      return C0::ppSat(model, context)
      * C1::ppSat(model, context)
      * C2::ppSat(model, context)
      * C3::ppSat(model, context)
      * C4::ppSat(model, context)
      * C5::ppSat(model, context)
      * C6::ppSat(model, context)
      * C7::ppSat(model, context)
      * C8::ppSat(model, context)
      * C9::ppSat(model, context)
      * C10::ppSat(model, context)
      * C11::ppSat(model, context)
      * C12::ppSat(model, context)
      * C13::ppSat(model, context)
      * C14::ppSat(model, context)
      * C15::ppSat(model, context)
      * C16::ppSat(model, context)
      * C17::ppSat(model, context)
      * C18::ppSat(model, context)
      * C19::ppSat(model, context)
      * C20::ppSat(model, context)
      * C21::ppSat(model, context);
   };

   template<typename M, typename E>
   static bool retract(M &model, SamplerExecutionContext<E> &context)
   {
      bool res = false;
      bool tmp = false;
      tmp = C0::retract(model, context); res = (res||tmp);
      tmp = C1::retract(model, context); res = (res||tmp);
      tmp = C2::retract(model, context); res = (res||tmp);
      tmp = C3::retract(model, context); res = (res||tmp);
      tmp = C4::retract(model, context); res = (res||tmp);
      tmp = C5::retract(model, context); res = (res||tmp);
      tmp = C6::retract(model, context); res = (res||tmp);
      tmp = C7::retract(model, context); res = (res||tmp);
      tmp = C8::retract(model, context); res = (res||tmp);
      tmp = C9::retract(model, context); res = (res||tmp);
      tmp = C10::retract(model, context); res = (res||tmp);
      tmp = C11::retract(model, context); res = (res||tmp);
      tmp = C12::retract(model, context); res = (res||tmp);
      tmp = C13::retract(model, context); res = (res||tmp);
      tmp = C14::retract(model, context); res = (res||tmp);
      tmp = C15::retract(model, context); res = (res||tmp);
      tmp = C16::retract(model, context); res = (res||tmp);
      tmp = C17::retract(model, context); res = (res||tmp);
      tmp = C18::retract(model, context); res = (res||tmp);
      tmp = C19::retract(model, context); res = (res||tmp);
      tmp = C20::retract(model, context); res = (res||tmp);
      tmp = C21::retract(model, context); res = (res||tmp);
      return res;
   };

   template<typename M, typename E>
   static void sampleSat(M &model, SamplerExecutionContext<E> &context)
   {
      C0::sampleSat(model, context);
      C1::sampleSat(model, context);
      C2::sampleSat(model, context);
      C3::sampleSat(model, context);
      C4::sampleSat(model, context);
      C5::sampleSat(model, context);
      C6::sampleSat(model, context);
      C7::sampleSat(model, context);
      C8::sampleSat(model, context);
      C9::sampleSat(model, context);
      C10::sampleSat(model, context);
      C11::sampleSat(model, context);
      C12::sampleSat(model, context);
      C13::sampleSat(model, context);
      C14::sampleSat(model, context);
      C15::sampleSat(model, context);
      C16::sampleSat(model, context);
      C17::sampleSat(model, context);
      C18::sampleSat(model, context);
      C19::sampleSat(model, context);
      C20::sampleSat(model, context);
      C21::sampleSat(model, context);
   };

};

template<class C0, class C1, class C2, class C3, class C4, class C5, class C6, class C7, class C8, class C9, class C10, class C11, class C12, class C13, class C14, class C15, class C16, class C17, class C18, class C19, class C20, class C21>
class EXOR22
{
public:

   template<typename M, typename E>
   static double ppSat(M &model, SamplerExecutionContext<E> &context)
   {
      return C0::ppSat(model, context)
      + C1::ppSat(model, context)
      + C2::ppSat(model, context)
      + C3::ppSat(model, context)
      + C4::ppSat(model, context)
      + C5::ppSat(model, context)
      + C6::ppSat(model, context)
      + C7::ppSat(model, context)
      + C8::ppSat(model, context)
      + C9::ppSat(model, context)
      + C10::ppSat(model, context)
      + C11::ppSat(model, context)
      + C12::ppSat(model, context)
      + C13::ppSat(model, context)
      + C14::ppSat(model, context)
      + C15::ppSat(model, context)
      + C16::ppSat(model, context)
      + C17::ppSat(model, context)
      + C18::ppSat(model, context)
      + C19::ppSat(model, context)
      + C20::ppSat(model, context)
      + C21::ppSat(model, context);
   };

   template<typename M, typename E>
   static bool retract(M &model, SamplerExecutionContext<E> &context)
   {
      if (C0::retract(model, context)) return true;
      if (C1::retract(model, context)) return true;
      if (C2::retract(model, context)) return true;
      if (C3::retract(model, context)) return true;
      if (C4::retract(model, context)) return true;
      if (C5::retract(model, context)) return true;
      if (C6::retract(model, context)) return true;
      if (C7::retract(model, context)) return true;
      if (C8::retract(model, context)) return true;
      if (C9::retract(model, context)) return true;
      if (C10::retract(model, context)) return true;
      if (C11::retract(model, context)) return true;
      if (C12::retract(model, context)) return true;
      if (C13::retract(model, context)) return true;
      if (C14::retract(model, context)) return true;
      if (C15::retract(model, context)) return true;
      if (C16::retract(model, context)) return true;
      if (C17::retract(model, context)) return true;
      if (C18::retract(model, context)) return true;
      if (C19::retract(model, context)) return true;
      if (C20::retract(model, context)) return true;
      if (C21::retract(model, context)) return true;
      return false;
   };

   template<typename M, typename E>
   static void sampleSat(M &model, SamplerExecutionContext<E> &context)
   {
      double probs[22] = {
      C0::ppSat(model, context),
      C1::ppSat(model, context),
      C2::ppSat(model, context),
      C3::ppSat(model, context),
      C4::ppSat(model, context),
      C5::ppSat(model, context),
      C6::ppSat(model, context),
      C7::ppSat(model, context),
      C8::ppSat(model, context),
      C9::ppSat(model, context),
      C10::ppSat(model, context),
      C11::ppSat(model, context),
      C12::ppSat(model, context),
      C13::ppSat(model, context),
      C14::ppSat(model, context),
      C15::ppSat(model, context),
      C16::ppSat(model, context),
      C17::ppSat(model, context),
      C18::ppSat(model, context),
      C19::ppSat(model, context),
      C20::ppSat(model, context),
      C21::ppSat(model, context)
      };
      int choice = rndChoice<22>(probs, context.rndSeed);
      switch (choice)
      {
         case 0 : C0::sampleSat(model, context); break;
         case 1 : C1::sampleSat(model, context); break;
         case 2 : C2::sampleSat(model, context); break;
         case 3 : C3::sampleSat(model, context); break;
         case 4 : C4::sampleSat(model, context); break;
         case 5 : C5::sampleSat(model, context); break;
         case 6 : C6::sampleSat(model, context); break;
         case 7 : C7::sampleSat(model, context); break;
         case 8 : C8::sampleSat(model, context); break;
         case 9 : C9::sampleSat(model, context); break;
         case 10 : C10::sampleSat(model, context); break;
         case 11 : C11::sampleSat(model, context); break;
         case 12 : C12::sampleSat(model, context); break;
         case 13 : C13::sampleSat(model, context); break;
         case 14 : C14::sampleSat(model, context); break;
         case 15 : C15::sampleSat(model, context); break;
         case 16 : C16::sampleSat(model, context); break;
         case 17 : C17::sampleSat(model, context); break;
         case 18 : C18::sampleSat(model, context); break;
         case 19 : C19::sampleSat(model, context); break;
         case 20 : C20::sampleSat(model, context); break;
         case 21 : C21::sampleSat(model, context); break;
      }
   };

};

template<class C0, class C1, class C2, class C3, class C4, class C5, class C6, class C7, class C8, class C9, class C10, class C11, class C12, class C13, class C14, class C15, class C16, class C17, class C18, class C19, class C20, class C21, class C22>
class IAND23
{
public:

   template<typename M, typename E>
   static double ppSat(M &model, SamplerExecutionContext<E> &context)
   {
      return C0::ppSat(model, context)
      * C1::ppSat(model, context)
      * C2::ppSat(model, context)
      * C3::ppSat(model, context)
      * C4::ppSat(model, context)
      * C5::ppSat(model, context)
      * C6::ppSat(model, context)
      * C7::ppSat(model, context)
      * C8::ppSat(model, context)
      * C9::ppSat(model, context)
      * C10::ppSat(model, context)
      * C11::ppSat(model, context)
      * C12::ppSat(model, context)
      * C13::ppSat(model, context)
      * C14::ppSat(model, context)
      * C15::ppSat(model, context)
      * C16::ppSat(model, context)
      * C17::ppSat(model, context)
      * C18::ppSat(model, context)
      * C19::ppSat(model, context)
      * C20::ppSat(model, context)
      * C21::ppSat(model, context)
      * C22::ppSat(model, context);
   };

   template<typename M, typename E>
   static bool retract(M &model, SamplerExecutionContext<E> &context)
   {
      bool res = false;
      bool tmp = false;
      tmp = C0::retract(model, context); res = (res||tmp);
      tmp = C1::retract(model, context); res = (res||tmp);
      tmp = C2::retract(model, context); res = (res||tmp);
      tmp = C3::retract(model, context); res = (res||tmp);
      tmp = C4::retract(model, context); res = (res||tmp);
      tmp = C5::retract(model, context); res = (res||tmp);
      tmp = C6::retract(model, context); res = (res||tmp);
      tmp = C7::retract(model, context); res = (res||tmp);
      tmp = C8::retract(model, context); res = (res||tmp);
      tmp = C9::retract(model, context); res = (res||tmp);
      tmp = C10::retract(model, context); res = (res||tmp);
      tmp = C11::retract(model, context); res = (res||tmp);
      tmp = C12::retract(model, context); res = (res||tmp);
      tmp = C13::retract(model, context); res = (res||tmp);
      tmp = C14::retract(model, context); res = (res||tmp);
      tmp = C15::retract(model, context); res = (res||tmp);
      tmp = C16::retract(model, context); res = (res||tmp);
      tmp = C17::retract(model, context); res = (res||tmp);
      tmp = C18::retract(model, context); res = (res||tmp);
      tmp = C19::retract(model, context); res = (res||tmp);
      tmp = C20::retract(model, context); res = (res||tmp);
      tmp = C21::retract(model, context); res = (res||tmp);
      tmp = C22::retract(model, context); res = (res||tmp);
      return res;
   };

   template<typename M, typename E>
   static void sampleSat(M &model, SamplerExecutionContext<E> &context)
   {
      C0::sampleSat(model, context);
      C1::sampleSat(model, context);
      C2::sampleSat(model, context);
      C3::sampleSat(model, context);
      C4::sampleSat(model, context);
      C5::sampleSat(model, context);
      C6::sampleSat(model, context);
      C7::sampleSat(model, context);
      C8::sampleSat(model, context);
      C9::sampleSat(model, context);
      C10::sampleSat(model, context);
      C11::sampleSat(model, context);
      C12::sampleSat(model, context);
      C13::sampleSat(model, context);
      C14::sampleSat(model, context);
      C15::sampleSat(model, context);
      C16::sampleSat(model, context);
      C17::sampleSat(model, context);
      C18::sampleSat(model, context);
      C19::sampleSat(model, context);
      C20::sampleSat(model, context);
      C21::sampleSat(model, context);
      C22::sampleSat(model, context);
   };

};

template<class C0, class C1, class C2, class C3, class C4, class C5, class C6, class C7, class C8, class C9, class C10, class C11, class C12, class C13, class C14, class C15, class C16, class C17, class C18, class C19, class C20, class C21, class C22>
class EXOR23
{
public:

   template<typename M, typename E>
   static double ppSat(M &model, SamplerExecutionContext<E> &context)
   {
      return C0::ppSat(model, context)
      + C1::ppSat(model, context)
      + C2::ppSat(model, context)
      + C3::ppSat(model, context)
      + C4::ppSat(model, context)
      + C5::ppSat(model, context)
      + C6::ppSat(model, context)
      + C7::ppSat(model, context)
      + C8::ppSat(model, context)
      + C9::ppSat(model, context)
      + C10::ppSat(model, context)
      + C11::ppSat(model, context)
      + C12::ppSat(model, context)
      + C13::ppSat(model, context)
      + C14::ppSat(model, context)
      + C15::ppSat(model, context)
      + C16::ppSat(model, context)
      + C17::ppSat(model, context)
      + C18::ppSat(model, context)
      + C19::ppSat(model, context)
      + C20::ppSat(model, context)
      + C21::ppSat(model, context)
      + C22::ppSat(model, context);
   };

   template<typename M, typename E>
   static bool retract(M &model, SamplerExecutionContext<E> &context)
   {
      if (C0::retract(model, context)) return true;
      if (C1::retract(model, context)) return true;
      if (C2::retract(model, context)) return true;
      if (C3::retract(model, context)) return true;
      if (C4::retract(model, context)) return true;
      if (C5::retract(model, context)) return true;
      if (C6::retract(model, context)) return true;
      if (C7::retract(model, context)) return true;
      if (C8::retract(model, context)) return true;
      if (C9::retract(model, context)) return true;
      if (C10::retract(model, context)) return true;
      if (C11::retract(model, context)) return true;
      if (C12::retract(model, context)) return true;
      if (C13::retract(model, context)) return true;
      if (C14::retract(model, context)) return true;
      if (C15::retract(model, context)) return true;
      if (C16::retract(model, context)) return true;
      if (C17::retract(model, context)) return true;
      if (C18::retract(model, context)) return true;
      if (C19::retract(model, context)) return true;
      if (C20::retract(model, context)) return true;
      if (C21::retract(model, context)) return true;
      if (C22::retract(model, context)) return true;
      return false;
   };

   template<typename M, typename E>
   static void sampleSat(M &model, SamplerExecutionContext<E> &context)
   {
      double probs[23] = {
      C0::ppSat(model, context),
      C1::ppSat(model, context),
      C2::ppSat(model, context),
      C3::ppSat(model, context),
      C4::ppSat(model, context),
      C5::ppSat(model, context),
      C6::ppSat(model, context),
      C7::ppSat(model, context),
      C8::ppSat(model, context),
      C9::ppSat(model, context),
      C10::ppSat(model, context),
      C11::ppSat(model, context),
      C12::ppSat(model, context),
      C13::ppSat(model, context),
      C14::ppSat(model, context),
      C15::ppSat(model, context),
      C16::ppSat(model, context),
      C17::ppSat(model, context),
      C18::ppSat(model, context),
      C19::ppSat(model, context),
      C20::ppSat(model, context),
      C21::ppSat(model, context),
      C22::ppSat(model, context)
      };
      int choice = rndChoice<23>(probs, context.rndSeed);
      switch (choice)
      {
         case 0 : C0::sampleSat(model, context); break;
         case 1 : C1::sampleSat(model, context); break;
         case 2 : C2::sampleSat(model, context); break;
         case 3 : C3::sampleSat(model, context); break;
         case 4 : C4::sampleSat(model, context); break;
         case 5 : C5::sampleSat(model, context); break;
         case 6 : C6::sampleSat(model, context); break;
         case 7 : C7::sampleSat(model, context); break;
         case 8 : C8::sampleSat(model, context); break;
         case 9 : C9::sampleSat(model, context); break;
         case 10 : C10::sampleSat(model, context); break;
         case 11 : C11::sampleSat(model, context); break;
         case 12 : C12::sampleSat(model, context); break;
         case 13 : C13::sampleSat(model, context); break;
         case 14 : C14::sampleSat(model, context); break;
         case 15 : C15::sampleSat(model, context); break;
         case 16 : C16::sampleSat(model, context); break;
         case 17 : C17::sampleSat(model, context); break;
         case 18 : C18::sampleSat(model, context); break;
         case 19 : C19::sampleSat(model, context); break;
         case 20 : C20::sampleSat(model, context); break;
         case 21 : C21::sampleSat(model, context); break;
         case 22 : C22::sampleSat(model, context); break;
      }
   };

};

template<class C0, class C1, class C2, class C3, class C4, class C5, class C6, class C7, class C8, class C9, class C10, class C11, class C12, class C13, class C14, class C15, class C16, class C17, class C18, class C19, class C20, class C21, class C22, class C23>
class IAND24
{
public:

   template<typename M, typename E>
   static double ppSat(M &model, SamplerExecutionContext<E> &context)
   {
      return C0::ppSat(model, context)
      * C1::ppSat(model, context)
      * C2::ppSat(model, context)
      * C3::ppSat(model, context)
      * C4::ppSat(model, context)
      * C5::ppSat(model, context)
      * C6::ppSat(model, context)
      * C7::ppSat(model, context)
      * C8::ppSat(model, context)
      * C9::ppSat(model, context)
      * C10::ppSat(model, context)
      * C11::ppSat(model, context)
      * C12::ppSat(model, context)
      * C13::ppSat(model, context)
      * C14::ppSat(model, context)
      * C15::ppSat(model, context)
      * C16::ppSat(model, context)
      * C17::ppSat(model, context)
      * C18::ppSat(model, context)
      * C19::ppSat(model, context)
      * C20::ppSat(model, context)
      * C21::ppSat(model, context)
      * C22::ppSat(model, context)
      * C23::ppSat(model, context);
   };

   template<typename M, typename E>
   static bool retract(M &model, SamplerExecutionContext<E> &context)
   {
      bool res = false;
      bool tmp = false;
      tmp = C0::retract(model, context); res = (res||tmp);
      tmp = C1::retract(model, context); res = (res||tmp);
      tmp = C2::retract(model, context); res = (res||tmp);
      tmp = C3::retract(model, context); res = (res||tmp);
      tmp = C4::retract(model, context); res = (res||tmp);
      tmp = C5::retract(model, context); res = (res||tmp);
      tmp = C6::retract(model, context); res = (res||tmp);
      tmp = C7::retract(model, context); res = (res||tmp);
      tmp = C8::retract(model, context); res = (res||tmp);
      tmp = C9::retract(model, context); res = (res||tmp);
      tmp = C10::retract(model, context); res = (res||tmp);
      tmp = C11::retract(model, context); res = (res||tmp);
      tmp = C12::retract(model, context); res = (res||tmp);
      tmp = C13::retract(model, context); res = (res||tmp);
      tmp = C14::retract(model, context); res = (res||tmp);
      tmp = C15::retract(model, context); res = (res||tmp);
      tmp = C16::retract(model, context); res = (res||tmp);
      tmp = C17::retract(model, context); res = (res||tmp);
      tmp = C18::retract(model, context); res = (res||tmp);
      tmp = C19::retract(model, context); res = (res||tmp);
      tmp = C20::retract(model, context); res = (res||tmp);
      tmp = C21::retract(model, context); res = (res||tmp);
      tmp = C22::retract(model, context); res = (res||tmp);
      tmp = C23::retract(model, context); res = (res||tmp);
      return res;
   };

   template<typename M, typename E>
   static void sampleSat(M &model, SamplerExecutionContext<E> &context)
   {
      C0::sampleSat(model, context);
      C1::sampleSat(model, context);
      C2::sampleSat(model, context);
      C3::sampleSat(model, context);
      C4::sampleSat(model, context);
      C5::sampleSat(model, context);
      C6::sampleSat(model, context);
      C7::sampleSat(model, context);
      C8::sampleSat(model, context);
      C9::sampleSat(model, context);
      C10::sampleSat(model, context);
      C11::sampleSat(model, context);
      C12::sampleSat(model, context);
      C13::sampleSat(model, context);
      C14::sampleSat(model, context);
      C15::sampleSat(model, context);
      C16::sampleSat(model, context);
      C17::sampleSat(model, context);
      C18::sampleSat(model, context);
      C19::sampleSat(model, context);
      C20::sampleSat(model, context);
      C21::sampleSat(model, context);
      C22::sampleSat(model, context);
      C23::sampleSat(model, context);
   };

};

template<class C0, class C1, class C2, class C3, class C4, class C5, class C6, class C7, class C8, class C9, class C10, class C11, class C12, class C13, class C14, class C15, class C16, class C17, class C18, class C19, class C20, class C21, class C22, class C23>
class EXOR24
{
public:

   template<typename M, typename E>
   static double ppSat(M &model, SamplerExecutionContext<E> &context)
   {
      return C0::ppSat(model, context)
      + C1::ppSat(model, context)
      + C2::ppSat(model, context)
      + C3::ppSat(model, context)
      + C4::ppSat(model, context)
      + C5::ppSat(model, context)
      + C6::ppSat(model, context)
      + C7::ppSat(model, context)
      + C8::ppSat(model, context)
      + C9::ppSat(model, context)
      + C10::ppSat(model, context)
      + C11::ppSat(model, context)
      + C12::ppSat(model, context)
      + C13::ppSat(model, context)
      + C14::ppSat(model, context)
      + C15::ppSat(model, context)
      + C16::ppSat(model, context)
      + C17::ppSat(model, context)
      + C18::ppSat(model, context)
      + C19::ppSat(model, context)
      + C20::ppSat(model, context)
      + C21::ppSat(model, context)
      + C22::ppSat(model, context)
      + C23::ppSat(model, context);
   };

   template<typename M, typename E>
   static bool retract(M &model, SamplerExecutionContext<E> &context)
   {
      if (C0::retract(model, context)) return true;
      if (C1::retract(model, context)) return true;
      if (C2::retract(model, context)) return true;
      if (C3::retract(model, context)) return true;
      if (C4::retract(model, context)) return true;
      if (C5::retract(model, context)) return true;
      if (C6::retract(model, context)) return true;
      if (C7::retract(model, context)) return true;
      if (C8::retract(model, context)) return true;
      if (C9::retract(model, context)) return true;
      if (C10::retract(model, context)) return true;
      if (C11::retract(model, context)) return true;
      if (C12::retract(model, context)) return true;
      if (C13::retract(model, context)) return true;
      if (C14::retract(model, context)) return true;
      if (C15::retract(model, context)) return true;
      if (C16::retract(model, context)) return true;
      if (C17::retract(model, context)) return true;
      if (C18::retract(model, context)) return true;
      if (C19::retract(model, context)) return true;
      if (C20::retract(model, context)) return true;
      if (C21::retract(model, context)) return true;
      if (C22::retract(model, context)) return true;
      if (C23::retract(model, context)) return true;
      return false;
   };

   template<typename M, typename E>
   static void sampleSat(M &model, SamplerExecutionContext<E> &context)
   {
      double probs[24] = {
      C0::ppSat(model, context),
      C1::ppSat(model, context),
      C2::ppSat(model, context),
      C3::ppSat(model, context),
      C4::ppSat(model, context),
      C5::ppSat(model, context),
      C6::ppSat(model, context),
      C7::ppSat(model, context),
      C8::ppSat(model, context),
      C9::ppSat(model, context),
      C10::ppSat(model, context),
      C11::ppSat(model, context),
      C12::ppSat(model, context),
      C13::ppSat(model, context),
      C14::ppSat(model, context),
      C15::ppSat(model, context),
      C16::ppSat(model, context),
      C17::ppSat(model, context),
      C18::ppSat(model, context),
      C19::ppSat(model, context),
      C20::ppSat(model, context),
      C21::ppSat(model, context),
      C22::ppSat(model, context),
      C23::ppSat(model, context)
      };
      int choice = rndChoice<24>(probs, context.rndSeed);
      switch (choice)
      {
         case 0 : C0::sampleSat(model, context); break;
         case 1 : C1::sampleSat(model, context); break;
         case 2 : C2::sampleSat(model, context); break;
         case 3 : C3::sampleSat(model, context); break;
         case 4 : C4::sampleSat(model, context); break;
         case 5 : C5::sampleSat(model, context); break;
         case 6 : C6::sampleSat(model, context); break;
         case 7 : C7::sampleSat(model, context); break;
         case 8 : C8::sampleSat(model, context); break;
         case 9 : C9::sampleSat(model, context); break;
         case 10 : C10::sampleSat(model, context); break;
         case 11 : C11::sampleSat(model, context); break;
         case 12 : C12::sampleSat(model, context); break;
         case 13 : C13::sampleSat(model, context); break;
         case 14 : C14::sampleSat(model, context); break;
         case 15 : C15::sampleSat(model, context); break;
         case 16 : C16::sampleSat(model, context); break;
         case 17 : C17::sampleSat(model, context); break;
         case 18 : C18::sampleSat(model, context); break;
         case 19 : C19::sampleSat(model, context); break;
         case 20 : C20::sampleSat(model, context); break;
         case 21 : C21::sampleSat(model, context); break;
         case 22 : C22::sampleSat(model, context); break;
         case 23 : C23::sampleSat(model, context); break;
      }
   };

};

template<class C0, class C1, class C2, class C3, class C4, class C5, class C6, class C7, class C8, class C9, class C10, class C11, class C12, class C13, class C14, class C15, class C16, class C17, class C18, class C19, class C20, class C21, class C22, class C23, class C24>
class IAND25
{
public:

   template<typename M, typename E>
   static double ppSat(M &model, SamplerExecutionContext<E> &context)
   {
      return C0::ppSat(model, context)
      * C1::ppSat(model, context)
      * C2::ppSat(model, context)
      * C3::ppSat(model, context)
      * C4::ppSat(model, context)
      * C5::ppSat(model, context)
      * C6::ppSat(model, context)
      * C7::ppSat(model, context)
      * C8::ppSat(model, context)
      * C9::ppSat(model, context)
      * C10::ppSat(model, context)
      * C11::ppSat(model, context)
      * C12::ppSat(model, context)
      * C13::ppSat(model, context)
      * C14::ppSat(model, context)
      * C15::ppSat(model, context)
      * C16::ppSat(model, context)
      * C17::ppSat(model, context)
      * C18::ppSat(model, context)
      * C19::ppSat(model, context)
      * C20::ppSat(model, context)
      * C21::ppSat(model, context)
      * C22::ppSat(model, context)
      * C23::ppSat(model, context)
      * C24::ppSat(model, context);
   };

   template<typename M, typename E>
   static bool retract(M &model, SamplerExecutionContext<E> &context)
   {
      bool res = false;
      bool tmp = false;
      tmp = C0::retract(model, context); res = (res||tmp);
      tmp = C1::retract(model, context); res = (res||tmp);
      tmp = C2::retract(model, context); res = (res||tmp);
      tmp = C3::retract(model, context); res = (res||tmp);
      tmp = C4::retract(model, context); res = (res||tmp);
      tmp = C5::retract(model, context); res = (res||tmp);
      tmp = C6::retract(model, context); res = (res||tmp);
      tmp = C7::retract(model, context); res = (res||tmp);
      tmp = C8::retract(model, context); res = (res||tmp);
      tmp = C9::retract(model, context); res = (res||tmp);
      tmp = C10::retract(model, context); res = (res||tmp);
      tmp = C11::retract(model, context); res = (res||tmp);
      tmp = C12::retract(model, context); res = (res||tmp);
      tmp = C13::retract(model, context); res = (res||tmp);
      tmp = C14::retract(model, context); res = (res||tmp);
      tmp = C15::retract(model, context); res = (res||tmp);
      tmp = C16::retract(model, context); res = (res||tmp);
      tmp = C17::retract(model, context); res = (res||tmp);
      tmp = C18::retract(model, context); res = (res||tmp);
      tmp = C19::retract(model, context); res = (res||tmp);
      tmp = C20::retract(model, context); res = (res||tmp);
      tmp = C21::retract(model, context); res = (res||tmp);
      tmp = C22::retract(model, context); res = (res||tmp);
      tmp = C23::retract(model, context); res = (res||tmp);
      tmp = C24::retract(model, context); res = (res||tmp);
      return res;
   };

   template<typename M, typename E>
   static void sampleSat(M &model, SamplerExecutionContext<E> &context)
   {
      C0::sampleSat(model, context);
      C1::sampleSat(model, context);
      C2::sampleSat(model, context);
      C3::sampleSat(model, context);
      C4::sampleSat(model, context);
      C5::sampleSat(model, context);
      C6::sampleSat(model, context);
      C7::sampleSat(model, context);
      C8::sampleSat(model, context);
      C9::sampleSat(model, context);
      C10::sampleSat(model, context);
      C11::sampleSat(model, context);
      C12::sampleSat(model, context);
      C13::sampleSat(model, context);
      C14::sampleSat(model, context);
      C15::sampleSat(model, context);
      C16::sampleSat(model, context);
      C17::sampleSat(model, context);
      C18::sampleSat(model, context);
      C19::sampleSat(model, context);
      C20::sampleSat(model, context);
      C21::sampleSat(model, context);
      C22::sampleSat(model, context);
      C23::sampleSat(model, context);
      C24::sampleSat(model, context);
   };

};

template<class C0, class C1, class C2, class C3, class C4, class C5, class C6, class C7, class C8, class C9, class C10, class C11, class C12, class C13, class C14, class C15, class C16, class C17, class C18, class C19, class C20, class C21, class C22, class C23, class C24>
class EXOR25
{
public:

   template<typename M, typename E>
   static double ppSat(M &model, SamplerExecutionContext<E> &context)
   {
      return C0::ppSat(model, context)
      + C1::ppSat(model, context)
      + C2::ppSat(model, context)
      + C3::ppSat(model, context)
      + C4::ppSat(model, context)
      + C5::ppSat(model, context)
      + C6::ppSat(model, context)
      + C7::ppSat(model, context)
      + C8::ppSat(model, context)
      + C9::ppSat(model, context)
      + C10::ppSat(model, context)
      + C11::ppSat(model, context)
      + C12::ppSat(model, context)
      + C13::ppSat(model, context)
      + C14::ppSat(model, context)
      + C15::ppSat(model, context)
      + C16::ppSat(model, context)
      + C17::ppSat(model, context)
      + C18::ppSat(model, context)
      + C19::ppSat(model, context)
      + C20::ppSat(model, context)
      + C21::ppSat(model, context)
      + C22::ppSat(model, context)
      + C23::ppSat(model, context)
      + C24::ppSat(model, context);
   };

   template<typename M, typename E>
   static bool retract(M &model, SamplerExecutionContext<E> &context)
   {
      if (C0::retract(model, context)) return true;
      if (C1::retract(model, context)) return true;
      if (C2::retract(model, context)) return true;
      if (C3::retract(model, context)) return true;
      if (C4::retract(model, context)) return true;
      if (C5::retract(model, context)) return true;
      if (C6::retract(model, context)) return true;
      if (C7::retract(model, context)) return true;
      if (C8::retract(model, context)) return true;
      if (C9::retract(model, context)) return true;
      if (C10::retract(model, context)) return true;
      if (C11::retract(model, context)) return true;
      if (C12::retract(model, context)) return true;
      if (C13::retract(model, context)) return true;
      if (C14::retract(model, context)) return true;
      if (C15::retract(model, context)) return true;
      if (C16::retract(model, context)) return true;
      if (C17::retract(model, context)) return true;
      if (C18::retract(model, context)) return true;
      if (C19::retract(model, context)) return true;
      if (C20::retract(model, context)) return true;
      if (C21::retract(model, context)) return true;
      if (C22::retract(model, context)) return true;
      if (C23::retract(model, context)) return true;
      if (C24::retract(model, context)) return true;
      return false;
   };

   template<typename M, typename E>
   static void sampleSat(M &model, SamplerExecutionContext<E> &context)
   {
      double probs[25] = {
      C0::ppSat(model, context),
      C1::ppSat(model, context),
      C2::ppSat(model, context),
      C3::ppSat(model, context),
      C4::ppSat(model, context),
      C5::ppSat(model, context),
      C6::ppSat(model, context),
      C7::ppSat(model, context),
      C8::ppSat(model, context),
      C9::ppSat(model, context),
      C10::ppSat(model, context),
      C11::ppSat(model, context),
      C12::ppSat(model, context),
      C13::ppSat(model, context),
      C14::ppSat(model, context),
      C15::ppSat(model, context),
      C16::ppSat(model, context),
      C17::ppSat(model, context),
      C18::ppSat(model, context),
      C19::ppSat(model, context),
      C20::ppSat(model, context),
      C21::ppSat(model, context),
      C22::ppSat(model, context),
      C23::ppSat(model, context),
      C24::ppSat(model, context)
      };
      int choice = rndChoice<25>(probs, context.rndSeed);
      switch (choice)
      {
         case 0 : C0::sampleSat(model, context); break;
         case 1 : C1::sampleSat(model, context); break;
         case 2 : C2::sampleSat(model, context); break;
         case 3 : C3::sampleSat(model, context); break;
         case 4 : C4::sampleSat(model, context); break;
         case 5 : C5::sampleSat(model, context); break;
         case 6 : C6::sampleSat(model, context); break;
         case 7 : C7::sampleSat(model, context); break;
         case 8 : C8::sampleSat(model, context); break;
         case 9 : C9::sampleSat(model, context); break;
         case 10 : C10::sampleSat(model, context); break;
         case 11 : C11::sampleSat(model, context); break;
         case 12 : C12::sampleSat(model, context); break;
         case 13 : C13::sampleSat(model, context); break;
         case 14 : C14::sampleSat(model, context); break;
         case 15 : C15::sampleSat(model, context); break;
         case 16 : C16::sampleSat(model, context); break;
         case 17 : C17::sampleSat(model, context); break;
         case 18 : C18::sampleSat(model, context); break;
         case 19 : C19::sampleSat(model, context); break;
         case 20 : C20::sampleSat(model, context); break;
         case 21 : C21::sampleSat(model, context); break;
         case 22 : C22::sampleSat(model, context); break;
         case 23 : C23::sampleSat(model, context); break;
         case 24 : C24::sampleSat(model, context); break;
      }
   };

};

#endif  // _BDEXPR2_H_

