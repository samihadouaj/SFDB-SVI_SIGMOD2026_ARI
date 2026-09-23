#include "Vocabulary.h"
#include <fstream>
#include <iostream>
#include <stdexcept> // std::runtime_error
#include <algorithm>


Vocabulary::Vocabulary(std::string const &vocab_filepath) 
: _vocab_filepath(vocab_filepath)
{
   std::ifstream ifs(_vocab_filepath);
   if (!ifs) {
      throw std::runtime_error("File not found: "+_vocab_filepath);
   }
   std::string str; 
   int idx = 0;
   while (std::getline(ifs, str))
   {
      _content.push_back(str);
      _wordToIndex[str] = idx++;  
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

// int Vocabulary::getWordCode(const std::string &word) const
// {
//    auto it = std::find(_content.begin(), _content.end(), word);
//    return (it - _content.begin());
// }


int Vocabulary::getWordCode(const std::string& word) const {
   auto it = _wordToIndex.find(word);
   if(it != _wordToIndex.end()) {
      return it->second;
   }
   else
   {
      std::cout << "Vocabulary::getWordCode Word not found: " << word << std::endl;
      return -1; // or throw an exception
   }
   // return (it != _wordToIndex.end()) ? it->second :-1;
}

void Vocabulary::saveAsCsvFile(std::string &csvFilePath)
{
   std::ofstream csvVocabFile(csvFilePath);
   if (!csvVocabFile.is_open())
   {
      std::cout << "Unable to create file " << csvFilePath << std::endl;
      throw std::runtime_error("Cannot create output file");
   }
   //csvVocabFile << "wordId,wordStr" << std::endl;
   csvVocabFile << "wordStr" << std::endl;
   int wordId=-1;
   for (auto word : _content)
   {
      wordId++;
      //csvVocabFile << wordId << "," << word << std::endl;
      csvVocabFile << word << std::endl;
   }
   if (csvVocabFile.is_open())
   {
      csvVocabFile.close();
   }
}

