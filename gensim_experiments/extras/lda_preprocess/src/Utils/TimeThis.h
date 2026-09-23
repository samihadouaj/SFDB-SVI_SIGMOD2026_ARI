
#ifndef _TIMETHIS_H_
#define _TIMETHIS_H_

#include <chrono>
#include <string>

class TimeThis 
{
private:
   const std::string _name;
   const std::chrono::steady_clock::time_point _start;
public:
   TimeThis(std::string name);
   ~TimeThis();
};

#endif // _TIMETHIS_H_
