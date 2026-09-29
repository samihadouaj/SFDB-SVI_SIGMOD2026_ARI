#include <algorithm>   // count, find
#include <charconv>    // from_chars
#include <fstream>     // ifstream
#include <memory>      // unique_ptr, make_unique
#include <stdexcept>   // runtime_error
#include <string>      // string
#include <vector>      // vector

#include "ReadData.h"
using namespace std;


int getnumDocuments(string sparseDataFileName)
{
    ifstream dataFile(sparseDataFileName);
    string str;
    int numDocuments=0;
    while (std::getline(dataFile, str)){
        numDocuments++;
    }
    return numDocuments;
}


int getnumWords(string dictionaryFileName)
{
    ifstream dataFile(dictionaryFileName);
    string str;
    int numWords=0;
    while (std::getline(dataFile, str)){
        numWords++;
    }
    return numWords;
}



std::vector<std::string> split( std::string const& original, char separator )
{
    std::vector<std::string> results;
    std::string::const_iterator start = original.begin();
    std::string::const_iterator end = original.end();
    std::string::const_iterator next = std::find( start, end, separator );
    while ( next != end ) {
        results.push_back( std::string( start, next ) );
        start = next + 1;
        next = std::find( start, end, separator );
    }
    results.push_back( std::string( start, next ) );
    return results;
}


std::vector<SparseDocument> readDocsFast(const std::string& sparseDataFileName,
                                               int numDocuments)
{
    auto documents = std::vector<SparseDocument>(numDocuments);
    std::ifstream dataFile(sparseDataFileName);
    std::string line;
    int docID = 0;

    while (docID < numDocuments && std::getline(dataFile, line)) {
        // Token count (assumes single spaces, well-formed: id cnt id cnt ...)
        const int tokens = static_cast<int>(std::count(line.begin(), line.end(), ' ')) + 1;
        const int pairs  = tokens / 2;

        auto idx = std::make_unique<int[]>(pairs);
        auto cnt = std::make_unique<int[]>(pairs);

        const char* p = line.c_str();
        const char* end = p + line.size();

        int docLen = 0;

        for (int i = 0; i < pairs; ++i) {
            // skip spaces
            while (p < end && *p == ' ') ++p;

            // parse wordId
            int wordId;
            std::from_chars_result r1 = std::from_chars(p, end, wordId);
            p = r1.ptr;

            while (p < end && *p == ' ') ++p;

            // parse count
            int c;
            std::from_chars_result r2 = std::from_chars(p, end, c);
            p = r2.ptr;

            idx[i] = wordId;
            cnt[i] = c;
            docLen += c;
        }

        SparseDocument d;
        d.tokenIndex = std::move(idx);
        d.tokenCount = std::move(cnt);
        d.numDistinctWordsPerDoc = pairs;
        d.docLength = docLen;

        documents[docID++] = std::move(d);
    }

    return documents; // use docID downstream if you need the actual count
}





// This reads the csv2 file efficiently into a flat vector of integers
std::vector<int> parseFileToFlatVector(const std::string& filename) {
    std::vector<int> result;
    std::ifstream file(filename, std::ios::in | std::ios::binary); // Open file in binary mode for efficiency

    if (!file.is_open()) {
        throw std::runtime_error("Could not open file");
    }

    result.reserve(5'000'000'000);

    // Read the header (first line) and ignore it
    std::string header;
    if (!std::getline(file, header)) {
        throw std::runtime_error("File is empty or invalid.");
    }

    constexpr size_t BUFFER_SIZE = 256 * 1024; // Buffer size for reading
    std::vector<char> buffer(BUFFER_SIZE);

    std::string leftover;
    while (file.read(buffer.data(), BUFFER_SIZE) || file.gcount() > 0) {
        size_t bytesRead = file.gcount();
        // Combine leftover from previous iteration with current buffer content
        std::string data = leftover + std::string(buffer.data(), bytesRead);
        leftover.clear();

        size_t start = 0;
        for (size_t i = 0; i < data.size(); ++i) {
            if (data[i] == '\n') {
                // Extract a single line
                std::string line(&data[start], i - start);
                start = i + 1;

                // Parse integers from the line and push them into the flat vector
                size_t pos = 0;
                while (pos < line.size()) {
                    size_t end = line.find(',', pos);
                    if (end == std::string::npos) end = line.size();
                    
                    // Convert the substring to int and push_back
                    result.push_back(std::stoi(std::string(line.substr(pos, end - pos))));
                    pos = end + 1;  // Move past comma
                }
            }
        }

        // Handle leftover (incomplete line at the end of the buffer)
        if (start < data.size()) {
            leftover = data.substr(start);
        }
    }

    file.close();
    return result;
}

