#ifndef _READDATA_H_
#define _READDATA_H_

#include <iostream>
#include <vector>
#include <memory>
using namespace std;



 struct SparseDocument {
      //input documents are represented  as wordId freq wordId freq ...
      unique_ptr<int[]> tokenIndex; // vector containing wordId only
      unique_ptr<int[]> tokenCount;  // vector containing freq only
      int docLength;
      int numDistinctWordsPerDoc; // number of distinct words in documents
 };


std::vector<std::string> split( std::string const& original, char separator );
int getnumDocuments(string sparseDataFileName);
int getnumWords(string dictionaryFileName);
std::vector<int> parseFileToFlatVector(const std::string& filename);

std::vector<SparseDocument>readDocsFast(const std::string& sparseDataFileName,
                                               int numDocuments);
#endif // _READDATA_H_