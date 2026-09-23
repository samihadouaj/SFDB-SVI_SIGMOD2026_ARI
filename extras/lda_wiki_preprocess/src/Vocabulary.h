
#ifndef _VOCABULARY2_H_
#define _VOCABULARY2_H_

#include <string>
#include <vector>
#include <unordered_map>
class Vocabulary {
   private:
      std::vector<std::string> _content; // content
      std::unordered_map<std::string, int> _wordToIndex;

      std::string _vocab_filepath; 

   public:
      Vocabulary(std::string const &vocab_filepath);
      int getSize() const;
      std::string getWordByCode(const int &wordCode) const;
      void saveAsCsvFile(std::string &csvFilePath);
      int getWordCode(const std::string &word) const;
   };

#endif // _VOCABULARY2_H_
