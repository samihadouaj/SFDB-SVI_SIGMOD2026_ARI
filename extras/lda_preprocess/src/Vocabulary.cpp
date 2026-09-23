#include "Vocabulary.h"
#include <fstream>
#include <iostream>
#include <stdexcept> // std::runtime_error

Vocabulary::Vocabulary(std::string const &vocab_filepath) 
: _vocab_filepath(vocab_filepath)
{
   std::ifstream ifs(_vocab_filepath);
   if (!ifs) {
      throw std::runtime_error("File not found: "+_vocab_filepath);
   }
   std::string str; 
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

