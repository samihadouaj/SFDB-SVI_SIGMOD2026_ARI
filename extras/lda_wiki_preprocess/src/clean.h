#ifndef TEXT_CLEANER_H
#define TEXT_CLEANER_H

#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <algorithm>
#include <memory>
#include <cctype>

// namespace TextCleaner {

// Configuration constants
inline constexpr size_t BUFFER_SIZE = 1024 * 1024; // 1MB buffer
inline constexpr int MIN_WORD_LENGTH = 3;
inline constexpr float MIN_DF = 50.0f; // Minimum document frequency (5%)

// Global structures (using inline variables available in C++17)
inline std::unordered_map<std::string, int> doc_frequencies;
inline std::unordered_set<std::string> stopwords;
inline int total_documents = 0;

// I/O buffers structure
struct IOBuffers {
    std::unique_ptr<char[]> in_buffer;
    std::unique_ptr<char[]> out_buffer;

    IOBuffers()
        : in_buffer(new char[BUFFER_SIZE]),
          out_buffer(new char[BUFFER_SIZE]) {}
};

// Function declarations

// Loads stopwords from a given file. Words shorter than MIN_WORD_LENGTH are skipped.
void load_stopwords(const std::string &stopword_file);

// Cleans a string by converting it to lowercase and replacing non-alphabetic characters
// (except hyphens inside words) with spaces.
std::string clean_content(std::string content);

// Determines whether a word is "nonsense" (i.e. it does not contain any vowels).
bool isNonsenseWord(const std::string &word);

// Processes a line of text. In count mode, counts unique valid words; otherwise, extracts valid words.
// Valid words must be at least MIN_WORD_LENGTH, not be in the stopwords set,
// not contain "--", and not be considered nonsense.
void process_line(const std::string &line, bool count_mode,
                  std::vector<std::string> &valid_words);

// Generates a vocabulary file from the words with document frequencies above MIN_DF.
void generate_vocab(const std::string &vocab_file);

// The main cleaning function. It takes optional parameters for input, output, vocabulary, and stopword files.
void clean(std::string input_path = "/scratch/shadouaj/gammapdb_arrow/data/raw/wiki08.txt",
           std::string output_path = "/scratch/shadouaj/gammapdb_arrow/data/raw/wiki08_clean.txt",
           std::string vocab_path = "/scratch/shadouaj/gammapdb_arrow/data/raw/wiki08_clean_vocab.txt",
           std::string stopword_path = "stopwords.txt",
           int reportEvery = 10000,
           int totalNbDocs = 5000000);

// } // namespace TextCleaner

#endif // TEXT_CLEANER_H
