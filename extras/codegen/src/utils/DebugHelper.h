
#ifndef _DEBUGHELPER_H_
#define _DEBUGHELPER_H_

#define SUPPRESS_UNUSED_PARAM_WARNING(expr) do { (void)(expr); } while (0)

#include <chrono>     // std::chrono
#include <cstdlib>    // std::abort()
#include <cxxabi.h>   // abi::__cxa_demangle()
#include <execinfo.h> // backtrace, backtrace_symbols_fd
#include <iostream>   // std::cout
#include <string>
#include <limits.h>
#include <unistd.h>


//static inline void printStackTrace(unsigned int max_frames = 200);
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
};

void halt_and_catch_fire();

void uncaught_exception_handler();

void signal_handler(int signal);

std::string getexepath();

std::string getexepath_parent();

std::string getCurrentTimeStampAsString();


#endif // _DEBUGHELPER_H_
