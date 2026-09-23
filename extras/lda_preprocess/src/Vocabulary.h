
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
      void saveAsCsvFile(std::string &csvFilePath);
};

#endif // _VOCABULARY_H_
