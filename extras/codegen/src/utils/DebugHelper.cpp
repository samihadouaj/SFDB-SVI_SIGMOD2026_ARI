#include "DebugHelper.h"

// https://github.com/tiwoc/cpp-handlers/blob/master/main.cpp

void halt_and_catch_fire()
{
   printStackTrace();
   std::cerr << "Creating CORE DUMP ... " << std::endl; 
   std::abort();
}

void uncaught_exception_handler()
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

void signal_handler(int signal)
{
   std::cerr << "Terminated due to signal " << signal << std::endl;
   halt_and_catch_fire();
}

std::string getexepath()
{
   char buf[ PATH_MAX ];
   ssize_t count = readlink( "/proc/self/exe", buf, PATH_MAX );
   std::string result(buf, (count > 0) ? count : 0);
   if (result.size()>0) {
      result = result.substr(0, result.rfind("/"));
   }
   return result;
}

std::string getexepath_parent()
{
   std::string exepath = getexepath();
   return exepath.substr(0, exepath.rfind("/"));
}



std::string getCurrentTimeStampAsString()
{
   std::time_t currentTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
   std::string timeStr(std::ctime(&currentTime));
   return timeStr.substr(0, timeStr.size()-1);
}




