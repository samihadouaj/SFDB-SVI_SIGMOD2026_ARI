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

#ifndef _VOCABULARY_H_
#define _VOCABULARY_H_

#include <string>
#include <vector>

class Vocabulary {
   private:
      std::vector<std::string> _content; // content
      std::string _vocab_filepath; 

   public:
      Vocabulary(std::string const &vocab_filepath);
      int getSize() const;
      std::string getWordByCode(const int &wordCode) const;
};

#endif // _VOCABULARY_H_
