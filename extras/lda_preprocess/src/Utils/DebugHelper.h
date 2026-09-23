#ifndef _DEBUGHELPER_H_
#define _DEBUGHELPER_H_

#define SUPPRESS_UNUSED_PARAM_WARNING(expr) do { (void)(expr); } while (0)

#include <cstdlib>    // std::abort()
#include <cxxabi.h>   // abi::__cxa_demangle()
#include <execinfo.h> // backtrace, backtrace_symbols_fd
#include <iostream>   // std::cout
#include <string>

// https://github.com/tiwoc/cpp-handlers/blob/master/main.cpp

using namespace std::literals::string_literals;

static inline void printStackTrace(unsigned int max_frames = 200)
{

   // adapted from https://panthema.net/2008/0901-stacktrace-demangled/
   // stacktrace.h (c) 2008, Timo Bingmann from http://idlebox.net/
   // published under the WTFPL v2.0
   std::cerr << "stack trace:" << std::endl;

   // storage array for stack trace address data
   void* addrlist[max_frames+1];

   // retrieve current stack addresses
   int addrlen = backtrace(addrlist, sizeof(addrlist) / sizeof(void*));

   if (addrlen == 0) {
      std::cerr << "  <empty, possibly corrupt>" << std::endl;
      return;
   }

   // resolve addresses into strings containing "filename(function+address)",
   // this array must be free()-ed
   char** symbollist = backtrace_symbols(addrlist, addrlen);

   // allocate string which will be filled with the demangled function name
   size_t funcnamesize = 256;
   char* funcname = (char*)malloc(funcnamesize);

   // iterate over the returned symbol lines. skip the first, it is the
   // address of this function.
   for (int i = 1; i < addrlen; i++)
   {
      char *begin_name = 0, *begin_offset = 0, *end_offset = 0;

      // find parentheses and +address offset surrounding the mangled name:
      // ./module(function+0x15c) [0x8048a6d]
      for (char *p = symbollist[i]; *p; ++p)
      {
         if (*p == '(')
            begin_name = p;
         else if (*p == '+')
            begin_offset = p;
         else if (*p == ')' && begin_offset) {
            end_offset = p;
            break;
         }
      }
      if (begin_name && begin_offset && end_offset
         && begin_name < begin_offset)
      {
         *begin_name++ = '\0';
         *begin_offset++ = '\0';
         *end_offset = '\0';

         // mangled name is now in [begin_name, begin_offset) and caller
         // offset in [begin_offset, end_offset). now apply
         // __cxa_demangle():

         int status;
         char* ret = abi::__cxa_demangle(begin_name, funcname, &funcnamesize, &status);
         if (status == 0) 
         {
            funcname = ret; // use possibly realloc()-ed string
            std::cerr << "  " << symbollist[i] << " : " << funcname << "+" << begin_offset << std::endl;
         } 
         else 
         {
            // demangling failed. Output function name as a C function with
            // no arguments.
            std::cerr << "  " << symbollist[i] << " : " << begin_name << "()+" << begin_offset << std::endl;  
         }
      }
      else
      {
         // couldn't parse the line? print the whole line.
         std::cerr << "  " << symbollist[i] << std::endl;
      }
   }

   free(funcname);
   free(symbollist);
}

static inline void halt_and_catch_fire()
{
   printStackTrace();
   std::cerr << "Creating CORE DUMP ... " << std::endl; 
   std::abort();
}

static inline void uncaught_exception_handler()
{
    std::exception_ptr exptr = std::current_exception();
    if (exptr != 0)
    {
        try { std::rethrow_exception(exptr); }
        catch (const std::exception& ex)
        {
            std::cerr << "Uncaught std::exception : " << ex.what() << std::endl;
        }
        catch (...)
        {
            std::cerr << "Terminated due to unknown uncaught exception" << std::endl;
        }
    }
    else
    {
        std::cerr << "uncaught_exception_handler: no exception detected" << std::endl;
    }
    halt_and_catch_fire();
 }

static inline void signal_handler(int signal)
{
   std::cerr << "Terminated due to signal " << signal << std::endl;
   halt_and_catch_fire();
}

namespace DebugHelper {

   template<class T>
   static void verifyLowerBoundIncl(const T val, const T lowerBoundIncl, std::string const& method_name, std::string const& var_name)
   {
      if ((val<lowerBoundIncl)) {
         throw std::out_of_range(method_name+" : "+var_name+" = " + std::to_string(val));
      }
   }

   template<class T>
   static void verifyLowerBoundExcl(const T val, const T lowerBoundExcl, std::string const& method_name, std::string const& var_name)
   {
      if ((val<=lowerBoundExcl)) {
         throw std::out_of_range(method_name+" : "+var_name+" = " + std::to_string(val));
      }
   }

   template<class T>
   static void verifyUpperBoundIncl(const T val, const T upperBoundIncl, std::string const& method_name, std::string const& var_name)
   {
      if ((val>upperBoundIncl)) {
         throw std::out_of_range(method_name+" : "+var_name+" = " + std::to_string(val));
      }
   }

   template<class T>
   static void verifyUpperBoundExcl(const T val, const T upperBoundExcl, std::string const& method_name, std::string const& var_name)
   {
      if ((val>=upperBoundExcl)) {
         throw std::out_of_range(method_name+" : "+var_name+" = " + std::to_string(val));
      }
   }

   template<class T>
   static void verifyRange(const T val, const T lowerBoundIncl, const T upperBoundExcl, std::string const& method_name, std::string const& var_name)
   {
      DebugHelper::verifyLowerBoundIncl(val, lowerBoundIncl, method_name, var_name);
      DebugHelper::verifyUpperBoundExcl(val, upperBoundExcl, method_name, var_name);
   }

} // namespace DebugHelper

#endif // _DEBUGHELPER_H_
