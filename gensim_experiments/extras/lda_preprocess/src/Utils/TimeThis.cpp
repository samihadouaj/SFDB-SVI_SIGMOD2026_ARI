#include "TimeThis.h"

#include <iostream>

TimeThis::TimeThis(std::string name): _name(name), _start(std::chrono::steady_clock::now()) {}

TimeThis::~TimeThis()
{
   std::chrono::steady_clock::time_point _end = std::chrono::steady_clock::now();
   std::cout << _name << ": Elapsed time = " << (std::chrono::duration_cast<std::chrono::milliseconds> (_end - _start).count()) << "[ms]" << std::endl;
}
