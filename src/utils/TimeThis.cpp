/*
   Copyright 2023 University of Michigan-Dearborn

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#include "TimeThis.h"

#include <iostream>

TimeThis::TimeThis(std::string name): _name(name), _start(std::chrono::steady_clock::now()) {}

TimeThis::~TimeThis()
{
   std::chrono::steady_clock::time_point _end = std::chrono::steady_clock::now();
   
   long timeMs = std::chrono::duration_cast<std::chrono::milliseconds> (_end - _start).count();
   long timeNs = std::chrono::duration_cast<std::chrono::nanoseconds> (_end - _start).count();
   std::cout << _name << ": Elapsed time = " << (timeMs==0 ? timeNs : timeMs) << (timeMs==0 ? "[ns]":"[ms]") << std::endl;
}
