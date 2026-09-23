
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


static inline void printStackTrace(unsigned int max_frames = 200);

void halt_and_catch_fire();

void uncaught_exception_handler();

void signal_handler(int signal);

std::string getexepath();

std::string getexepath_parent();

std::string getCurrentTimeStampAsString();


#endif // _DEBUGHELPER_H_
