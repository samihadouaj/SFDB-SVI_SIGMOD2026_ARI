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

#include "Vocabulary.h"
#include <fstream>
#include <stdexcept> // std::runtime_error

Vocabulary::Vocabulary(std::string const &vocab_filepath) 
: _vocab_filepath(vocab_filepath)
{
   std::ifstream ifs(_vocab_filepath);
   if (!ifs) {
      throw std::runtime_error("File not found: "+_vocab_filepath);
   }
   std::string str; 
   std::getline(ifs, str); // skip first row
   while (std::getline(ifs, str))
   {
      _content.push_back(str);
   }
}

int Vocabulary::getSize() const
{
   return _content.size();
}

std::string Vocabulary::getWordByCode(const int &wordCode) const
{
   return _content.at(wordCode);
}
