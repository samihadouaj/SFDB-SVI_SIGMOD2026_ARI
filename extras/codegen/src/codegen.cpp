#include "utils/DebugHelper.h"

#include <chrono>     // std::chrono
#include <exception>  // std::set_terminate()
#include <signal.h>   // signal()

#include <fstream>
#include <iostream>
#include <string>



int runMe(int argc, const char **argv);

// VrdExprOperators
void genVrdExprOperators();
void genVrdExor(std::ofstream &outputFile, int switchLimit);
void genVrdExorSampleSatSwitch(std::ofstream &outputFile, int switchLimit);
void genVrdIand(std::ofstream &of, int switchLimit);

// printline
void pl(const std::string &line, std::ofstream &outputFile);

// PcExpr (pre-compiled expressions: IAND4, EXOR11, etc.)
void printIncludeStatements(std::ofstream &outputFile);
void defineIANDType(std::ofstream &outputFile, int n);
void defineEXORType(std::ofstream &outputFile, int n);
void genPCExprCode();

int main(int argc, const char **argv)
{
   signal(SIGSEGV, signal_handler);
   std::set_terminate(uncaught_exception_handler);
   return runMe(argc, argv);
};

int runMe(int argc, const char **argv)
{
   SUPPRESS_UNUSED_PARAM_WARNING(argc);
   SUPPRESS_UNUSED_PARAM_WARNING(argv);

   std::cout << "codegen " << getCurrentTimeStampAsString() << std::endl;
   
   genVrdExprOperators(); // recreates VrdExprOperators.h
   genPCExprCode();       // recreates BDExpr2.g

   return 0;
};


void genVrdExprOperators()
{
   std::cout << "genVrdExprOperators " << getCurrentTimeStampAsString() << std::endl;
   std::ofstream outputFile("../../../../gammapdb_arrow/src/VrdExprOperators.h");

   outputFile << "#ifndef _VRDEXPROPERATORS_H_" << std::endl; 
   outputFile << "#define _VRDEXPROPERATORS_H_" << std::endl; 

   outputFile << std::endl; 
   outputFile << "#include \"VrdExprLiterals.h\"" << std::endl; 

   outputFile << std::endl; 
   outputFile << "// generated on " << getCurrentTimeStampAsString() << std::endl;

   outputFile << std::endl; 
   outputFile << "namespace  VrdExpr {" << std::endl; 

   outputFile << std::endl; 
   genVrdExor(outputFile, 8);
   genVrdIand(outputFile, 8);
   
   outputFile << std::endl; 
   outputFile << "} // namespace  VrdExpr " << std::endl; 

   outputFile << std::endl; 
   outputFile << "#endif  // _VRDEXPROPERATORS_H_" << std::endl; 
   outputFile << std::endl; 

   if (outputFile.is_open()) {
      outputFile.close();
   }
};


void genVrdExor(std::ofstream &of, int switchLimit)
{
   pl("// -------------------------", of);
   pl("// EXOR", of);
   pl("// -------------------------", of);
   pl("", of);
   pl("template <size_t I_, typename T_> ", of);
   pl("struct EXORTerm{", of);
   pl("   typedef T_ term;", of);
   pl("};", of);
   pl("", of);
   pl("", of);


   pl("template <size_t... Is_, typename... Ts_>", of);
   pl("struct EXORImpl<std::index_sequence<Is_...>, Ts_...>", of);
   pl("        : public EXORTerm<Is_, Ts_>... ", of);
   pl("{", of);

   pl("public:", of);
   pl("", of);
   pl("   static inline size_t numTerms()", of);
   pl("   { return sizeof...(Ts_); }", of);
   pl("", of);
   pl("   // ppSat", of);
   pl("", of);
   pl("   template<typename M, typename E>", of);
   pl("   static inline double ppSat(M &model, SamplerExecutionContext<E> &context)", of);
   pl("   {", of);
   pl("      double probs[] = { Ts_::ppSat(model, context)...};", of);
   pl("      double result = 0;", of);
   pl("      for (size_t i=0; i<sizeof...(Ts_); i++)", of);
   pl("      { result += probs[i]; }", of);
   pl("      return result;", of);
   pl("   };", of);
   pl("", of);
   pl("   // retract", of);
   pl("", of);
   pl("   template <int TERMID, typename M, typename E>", of);
   pl("   static typename std::enable_if<(TERMID > 0), bool>::type", of);
   pl("   retractImpl(M &model, SamplerExecutionContext<E> &context)", of);
   pl("   {", of);
   pl("      if (EXORLookup<TERMID,EXOR<Ts_...>>::term::retract(model, context)) return true;", of);
   pl("      return retractImpl<TERMID-1,M>(model, context);", of);
   pl("   };", of);
   pl("", of);
   pl("   template <int TERMID, typename M, typename E>", of);
   pl("   static inline typename std::enable_if<TERMID == 0, bool>::type", of);
   pl("   retractImpl(M &model, SamplerExecutionContext<E> &context)", of);
   pl("   {", of);
   pl("      if (EXORLookup<0,EXOR<Ts_...>>::term::retract(model, context)) return true;", of);
   pl("      return false;", of);
   pl("   };", of);
   pl("", of);
   pl("   template<typename M, typename E>", of);
   pl("   static inline bool retract(M &model, SamplerExecutionContext<E> &context)", of);
   pl("   {", of);
   pl("      return retractImpl<sizeof...(Ts_)-1,M>(model, context);", of);
   pl("   };", of);
   pl("", of);
   pl("   // sampleSat", of);
   pl("", of);
   pl("   template <int NUMCHOICES, typename M, typename E>", of);
   of << "   static inline typename std::enable_if<(NUMCHOICES > " << switchLimit << "), bool>::type " << std::endl;
   pl("   sampleSatSwitch(int termId, M &model, SamplerExecutionContext<E> &context)", of);
   pl("   {", of);
   pl("      switch(termId)", of);
   pl("      {", of);

   for (int i=1; i<=switchLimit; i++)
   {
      of << "         case NUMCHOICES-" << i << ": return EXORLookup<NUMCHOICES-" << i << ",EXOR<Ts_...>>::term::sampleSat(model, context);" << std::endl;
   }

   pl("", of);
   of << "         default: return sampleSatSwitch<std::max((NUMCHOICES-" << switchLimit << "),0)>(termId, model, context);" << std::endl;

   pl("      }", of);
   pl("   };", of);
   pl("", of);


   pl("   template <int NUMCHOICES, typename M, typename E>", of);
   pl("   static inline typename std::enable_if<NUMCHOICES == 0, bool>::type ", of);
   pl("   sampleSatSwitch(int termId, M &model, SamplerExecutionContext<E> &context)", of);
   pl("   {", of);
   pl("      throw std::runtime_error(\"sampleSatSwitch: invalid index\");", of);
   pl("      SUPPRESS_UNUSED_PARAM_WARNING(termId);", of);
   pl("      SUPPRESS_UNUSED_PARAM_WARNING(model);", of);
   pl("      SUPPRESS_UNUSED_PARAM_WARNING(context);", of);
   pl("   };", of);

   for (int i=1; i<=switchLimit; i++)
   {
      pl("", of);
      genVrdExorSampleSatSwitch(of, i);
   }
   
   pl("", of);
   pl("   template<typename M, typename E>", of);
   pl("   static inline bool sampleSat(M &model, SamplerExecutionContext<E> &context)", of);
   pl("   {", of);
   pl("      double probs[] = { Ts_::ppSat(model, context)...};", of);
   pl("      int choice = rndChoice<sizeof...(Ts_)>(probs, context.rndSeed);", of);
   pl("      return sampleSatSwitch<sizeof...(Ts_),M>(choice, model, context);", of);
   pl("   };", of);

   pl("", of);
   pl("   template<typename M, typename E>", of);
   pl("   static inline bool toString(std::ostream &os, M &model, SamplerExecutionContext<E> &context)", of);
   pl("   {", of);
   pl("      os << \"EXOR(\";", of);
   pl("      bool results[] = { Ts_::toString(os, model, context)... };", of);
   pl("      SUPPRESS_UNUSED_PARAM_WARNING(results);", of);
   pl("      os << \")\";", of);
   pl("      return true;", of);
   pl("   }", of);
   pl("", of);

   pl("}; // EXORImpl", of);

   pl("", of);
   pl("template <typename... Ts_>", of);
   pl("struct EXOR : public EXORImpl<std::make_index_sequence<sizeof...(Ts_)>, Ts_...> {};", of);
   pl("", of);
   pl("template <size_t I_, typename T_>", of);
   pl("struct  EXORLookup {};", of);
   pl("", of);
   pl("template <size_t I_>", of);
   pl("class EXORLookup<I_, EXOR<>> {", of);
   pl("public:", of);
   pl("   static_assert(I_ != I_, \"EXORLookup: index out of range\");", of);
   pl("};", of);
   pl("", of);
   pl("template <typename H_, typename... Ts_>", of);
   pl("class EXORLookup<0, EXOR<H_, Ts_...>> {", of);
   pl("public:", of);
   pl("   typedef H_ term;", of);
   pl("};", of);
   pl("", of);
   pl("template <size_t I_, typename H_, typename... Ts_>", of);
   pl("class EXORLookup<I_, EXOR<H_, Ts_...>> {", of);
   pl("public:", of);
   pl("   typedef typename EXORLookup<I_ - 1, EXOR<Ts_...>>::term term;", of);
   pl("};", of);
   pl("", of);
   pl("", of);

};


void genVrdIand(std::ofstream &of, int switchLimit)
{
   SUPPRESS_UNUSED_PARAM_WARNING(switchLimit);

   pl("// -------------------------", of);
   pl("// IAND", of);
   pl("// -------------------------", of);
   pl("", of);
   pl("template <size_t I_, typename T_> ", of);
   pl("struct IANDTerm{", of);
   pl("   typedef T_ term;", of);
   pl("};", of);
   pl("", of);
   pl("template <size_t... Is_, typename... Ts_>", of);
   pl("struct IANDImpl<std::index_sequence<Is_...>, Ts_...>", of);
   pl("        : public IANDTerm<Is_, Ts_>... ", of);
   pl("{", of);
   pl("public:", of);
   pl("", of);
   pl("   static inline size_t numTerms()", of);
   pl("   { return sizeof...(Ts_); }", of);
   pl("", of);
   pl("   // ppSat", of);
   pl("", of);
   pl("   template<typename M, typename E>", of);
   pl("   static inline double ppSat(M &model, SamplerExecutionContext<E> &context)", of);
   pl("   {", of);
   pl("      double probs[] = { Ts_::ppSat(model, context)...};", of);
   pl("      double result = 1.0;", of);
   pl("      for (size_t i=0; i<sizeof...(Ts_); i++)", of);
   pl("      {", of);
   pl("         result *= probs[i];", of);
   pl("      }", of);
   pl("      return result;", of);
   pl("   };", of);
   pl("", of);
   pl("   template<typename M, typename E>", of);
   pl("   static inline bool retract(M &model, SamplerExecutionContext<E> &context)", of);
   pl("   {", of);
   pl("      bool res = false;", of);
   pl("      bool results[] = { Ts_::retract(model, context)... };", of);
   pl("      for (size_t i=0; i<sizeof...(Ts_); i++)", of);
   pl("      {", of);
   pl("         res = (res||results[i]);", of);
   pl("      }", of);
   pl("      return res;", of);
   pl("   };", of);
   pl("", of);
   pl("   template<typename M, typename E>", of);
   pl("   static inline bool sampleSat(M &model, SamplerExecutionContext<E> &context)", of);
   pl("   {", of);
   pl("      bool results[] = { Ts_::sampleSat(model, context)... };", of);
   pl("      SUPPRESS_UNUSED_PARAM_WARNING(results);", of);
   pl("      return true;", of);
   pl("   };", of);

   pl("", of);
   pl("   template<typename M, typename E>", of);
   pl("   static inline bool toString(std::ostream &os, M &model, SamplerExecutionContext<E> &context)", of);
   pl("   {", of);
   pl("      os << \"IAND(\";", of);
   pl("      bool results[] = { Ts_::toString(os, model, context)... };", of);
   pl("      SUPPRESS_UNUSED_PARAM_WARNING(results);", of);
   pl("      os << \")\";", of);
   pl("      return true;", of);
   pl("   }", of);
   pl("", of);

   pl("}; // IANDImpl", of);
   pl("", of);
   pl("", of);
   pl("template <typename... Ts_>", of);
   pl("struct IAND : public IANDImpl<std::make_index_sequence<sizeof...(Ts_)>, Ts_...> {};", of);
   pl("", of);
   pl("template <size_t I_, typename T_>", of);
   pl("struct  IANDLookup {};", of);
   pl("", of);
   pl("template <size_t I_>", of);
   pl("class IANDLookup<I_, IAND<>> {", of);
   pl("public:", of);
   pl("   static_assert(I_ != I_, \"IANDLookup: index out of range\");", of);
   pl("};", of);
   pl("", of);
   pl("template <typename H_, typename... Ts_>", of);
   pl("class IANDLookup<0, IAND<H_, Ts_...>> {", of);
   pl("public:", of);
   pl("   typedef H_ term;", of);
   pl("};", of);
   pl("", of);
   pl("template <size_t I_, typename H_, typename... Ts_>", of);
   pl("class IANDLookup<I_, IAND<H_, Ts_...>> {", of);
   pl("public:", of);
   pl("   typedef typename IANDLookup<I_ - 1, IAND<Ts_...>>::term term;", of);
   pl("};", of);
   pl("", of);
   pl("", of);



};

void genVrdExorSampleSatSwitch(std::ofstream &of, int switchLimit)
{
   pl("   template <int NUMCHOICES, typename M, typename E>", of);
   of << "   static inline typename std::enable_if<NUMCHOICES == " << switchLimit << ", bool>::type " << std::endl;
   pl("   sampleSatSwitch(int termId, M &model, SamplerExecutionContext<E> &context)", of);
   pl("   {", of);
   pl("      switch(termId)", of);
   pl("      {", of);

   for (int i=(switchLimit-1); i>=0; i--)
   {
      of << "         case " << i << ": return EXORLookup<" << i << ",EXOR<Ts_...>>::term::sampleSat(model, context); " << std::endl;
   }
   pl("         default: throw std::runtime_error(\"sampleSatSwitch: invalid index\");", of);

   pl("      }", of);
   pl("   };", of);

};

inline void pl(const std::string &line, std::ofstream &outputFile)
{
   outputFile << line << std::endl;
};



// -----------------------------------------------------------
// PCExpr (pre-compiled expression) generator
// -----------------------------------------------------------

void printIncludeStatements(std::ofstream &outputFile)
{
   outputFile << std::endl; 
   outputFile << "#include \"BDSet.h\"" << std::endl; 
   outputFile << "#include \"SamplerExecutionContext.h\"" << std::endl; 
   outputFile << "#include \"utils/DebugHelper.h\"" << std::endl; 
   outputFile << "#include \"utils/RndUtils.h\"" << std::endl; 
   outputFile << std::endl; 
   outputFile << "#include <tuple>" << std::endl; 
};

void defineIANDType(std::ofstream &outputFile, int n)
{
   outputFile << std::endl; 
   outputFile << "template<class C0"; 
   for (int i=1; i<n; i++) {
      outputFile << ", class C" << i; 
   }
   outputFile << ">" << std::endl; 
   outputFile << "class IAND" << n << std::endl; 
   outputFile << "{" << std::endl;
   outputFile << "public:" << std::endl; 

   // define ::ppSat
   outputFile << std::endl; 
   outputFile << "   template<typename M, typename E>" << std::endl; 
   outputFile << "   static double ppSat(M &model, SamplerExecutionContext<E> &context)" << std::endl; 
   outputFile << "   {" << std::endl;
   outputFile << "      return C0::ppSat(model, context)";
   for (int i=1; i<n; i++) {
      outputFile << std::endl << "      * C" << i << "::ppSat(model, context)"; 
   }
   outputFile << ";" << std::endl; 
   outputFile << "   };" << std::endl; 

   // define ::retract
   outputFile << std::endl; 
   outputFile << "   template<typename M, typename E>" << std::endl; 
   outputFile << "   static bool retract(M &model, SamplerExecutionContext<E> &context)" << std::endl; 
   outputFile << "   {" << std::endl; 
   outputFile << "      bool res = false;" << std::endl; 
   outputFile << "      bool tmp = false;" << std::endl;
   for (int i=0; i<n; i++) {
      outputFile << "      tmp = C" << i << "::retract(model, context); res = (res||tmp);" << std::endl;
   }
   outputFile << "      return res;" << std::endl;
   outputFile << "   };" << std::endl; 

   // define ::sampleSat
   outputFile << std::endl; 
   outputFile << "   template<typename M, typename E>" << std::endl; 
   outputFile << "   static void sampleSat(M &model, SamplerExecutionContext<E> &context)" << std::endl;
   outputFile << "   {" << std::endl;
   for (int i=0; i<n; i++) {
      outputFile  << "      C" << i << "::sampleSat(model, context);" << std::endl; 
   }
   outputFile << "   };" << std::endl; 
   
   // close class definition
   outputFile << std::endl; 
   outputFile << "};" << std::endl; 

}; // defineIANDType

void defineEXORType(std::ofstream &outputFile, int n)
{
   outputFile << std::endl; 
   outputFile << "template<class C0"; 
   for (int i=1; i<n; i++) {
      outputFile << ", class C" << i; 
   }
   outputFile << ">" << std::endl; 
   outputFile << "class EXOR" << n << std::endl; 
   outputFile << "{" << std::endl;
   outputFile << "public:" << std::endl; 

   // define ::ppSat
   outputFile << std::endl; 
   outputFile << "   template<typename M, typename E>" << std::endl; 
   outputFile << "   static double ppSat(M &model, SamplerExecutionContext<E> &context)" << std::endl; 
   outputFile << "   {" << std::endl;
   outputFile << "      return C0::ppSat(model, context)";
   for (int i=1; i<n; i++) {
      outputFile << std::endl << "      + C" << i << "::ppSat(model, context)"; 
   }
   outputFile << ";" << std::endl; 
   outputFile << "   };" << std::endl; 

   // define ::retract
   outputFile << std::endl; 
   outputFile << "   template<typename M, typename E>" << std::endl; 
   outputFile << "   static bool retract(M &model, SamplerExecutionContext<E> &context)" << std::endl; 
   outputFile << "   {" << std::endl;
   for (int i=0; i<n; i++) {
      outputFile << "      if (C" << i << "::retract(model, context)) return true;" << std::endl;
   }
   outputFile << "      return false;" << std::endl;
   outputFile << "   };" << std::endl; 

   // define ::sampleSat
   outputFile << std::endl; 
   outputFile << "   template<typename M, typename E>" << std::endl; 
   outputFile << "   static void sampleSat(M &model, SamplerExecutionContext<E> &context)" << std::endl;
   outputFile << "   {" << std::endl;
   outputFile << "      double probs["<< n << "] = {" << std::endl;
   outputFile << "      C0::ppSat(model, context)";
   for (int i=1; i<n; i++) {
      outputFile << "," << std::endl;
      outputFile << "      C" << i << "::ppSat(model, context)"; 
   }
   outputFile << std::endl <<"      };" << std::endl;
   outputFile << "      int choice = rndChoice<" << n << ">(probs, context.rndSeed);"  << std::endl;
   outputFile << "      switch (choice)" << std::endl;
   outputFile << "      {" << std::endl;
   for (int i=0; i<n; i++) {
      outputFile << "         case " << i << " : C" << i << "::sampleSat(model, context); break;"<< std::endl;
   }
   outputFile << "      }" << std::endl;

   outputFile << "   };" << std::endl; 
   

   // close class definition
   outputFile << std::endl; 
   outputFile << "};" << std::endl; 
   
} // defineIANDType

void genPCExprCode()
{

   std::cout << "gencode " << getCurrentTimeStampAsString() << std::endl;


   std::ofstream outputFile("../../../../gammapdb_arrow/src/BDExpr2.h");

   outputFile << "#ifndef _BDEXPR2_H_" << std::endl; 
   outputFile << "#define _BDEXPR2_H_" << std::endl; 

   outputFile << std::endl; 
   outputFile << "// generated on " << getCurrentTimeStampAsString() << std::endl; 

   printIncludeStatements(outputFile);


   for (int i=2; i<=25; i++)
   {
      defineIANDType(outputFile, i);
      defineEXORType(outputFile, i);
   }

   outputFile << std::endl; 
   outputFile << "#endif  // _BDEXPR2_H_" << std::endl; 
   outputFile << std::endl; 

   if (outputFile.is_open()) {
      outputFile.close();
   }

};



