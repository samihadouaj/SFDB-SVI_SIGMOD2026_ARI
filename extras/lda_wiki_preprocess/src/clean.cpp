#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <algorithm>
#include <memory>

namespace
{
    // Configuration constants
    const size_t BUFFER_SIZE = 1024 * 1024; // 1MB buffer
    const int MIN_WORD_LENGTH = 3;
    const float MIN_DF = 50; // Minimum document frequency (5%)
    // const float MAX_DF = 0.9f;     // Maximum document frequency (90%)

    // Global structures
    std::unordered_map<std::string, int> doc_frequencies;
    std::unordered_set<std::string> stopwords;
    int total_documents = 0;
}

struct IOBuffers
{
    std::unique_ptr<char[]> in_buffer;
    std::unique_ptr<char[]> out_buffer;

    IOBuffers() : in_buffer(new char[BUFFER_SIZE]),
                  out_buffer(new char[BUFFER_SIZE]) {}
};

void load_stopwords(const std::string &stopword_file)
{
    std::ifstream file(stopword_file);
    if (!file)
    {
        std::cerr << "Error: Unable to open stopword file: " << stopword_file << '\n';
        return;
    }

    std::string word;
    while (file >> word)
    {
        if (word.length() >= MIN_WORD_LENGTH)
        {
            stopwords.insert(word);
        }
    }
}

std::string clean_content(std::string content)
{
    // Manual cleaning instead of regex
    std::transform(content.begin(), content.end(), content.begin(), ::tolower);

    std::string cleaned;
    cleaned.reserve(content.length());

    bool in_word = false;
    for (char c : content)
    {
        if (std::isalpha(c) || (c == '-' && in_word))
        {
            cleaned += c;
            in_word = true;
        }
        else
        {
            if (in_word)
            {
                cleaned += ' ';
                in_word = false;
            }
        }
    }

    // Remove trailing space
    if (!cleaned.empty() && cleaned.back() == ' ')
    {
        cleaned.pop_back();
    }

    return cleaned;
}

// Heuristic: a word is considered nonsense if it contains no vowels.
bool isNonsenseWord(const std::string &word)
{
    if (word.empty())
        return true; // If nothing remains, consider it nonsense

    // Check for vowels
    for (char c : word)
    {
        char lower = std::tolower(c);
        if (lower == 'a' || lower == 'e' || lower == 'i' || lower == 'o' || lower == 'u')
        {
            return false; // Found a vowel, so it's not nonsense
        }
    }
    return true; // No vowels found; consider the word nonsense
}

void process_line(const std::string &line, bool count_mode,
                  std::vector<std::string> &valid_words)
{
    if (line.find("#REDIRECT") == 0)
        return;

    const std::string content = clean_content(line);
    std::unordered_set<std::string> unique_words;

    size_t start = 0;
    size_t end = 0;

    while ((end = content.find(' ', start)) != std::string::npos)
    {
        if (end != start)
        {
            std::string word = content.substr(start, end - start);
            if (word.length() >= MIN_WORD_LENGTH &&
                !stopwords.count(word) &&
                word.find("--") == std::string::npos &&
                !isNonsenseWord(word))
            {

                if (count_mode)
                {
                    unique_words.insert(word);
                }
                else if (doc_frequencies[word] > MIN_DF)
                {
                    valid_words.push_back(word);
                }
            }
        }
        start = end + 1;
    }

    // Handle last word
    if (start < content.length())
    {
        std::string word = content.substr(start);
        if (word.length() >= MIN_WORD_LENGTH &&
            !stopwords.count(word) &&
            word.find("--") == std::string::npos &&
            !isNonsenseWord(word))
        {
            if (count_mode)
            {
                unique_words.insert(word);
            }
            else if (doc_frequencies[word] > MIN_DF)
            {
                valid_words.push_back(word);
            }
        }
    }

    if (count_mode)
    {
        for (const auto &word : unique_words)
        {
            doc_frequencies[word]++;
        }
        total_documents++;
    }
}

void generate_vocab(const std::string &vocab_file)
{
    if (total_documents == 0)
    {
        std::cerr << "Error: No documents processed\n";
        return;
    }

    std::vector<std::string> vocab;
    for (const auto &[word, count] : doc_frequencies)
    {
        // const float df = static_cast<float>(count) / total_documents;
        if (count >= MIN_DF)
        {
            vocab.push_back(word);
        }
    }

    std::sort(vocab.begin(), vocab.end());

    std::ofstream out(vocab_file);
    if (!out)
    {
        std::cerr << "Error: Unable to open vocab file: " << vocab_file << '\n';
        return;
    }
    for (const auto &word : vocab)
    {
        out << word << '\n';
    }
}

void clean(std::string input_path = "/scratch/shadouaj/gammapdb_arrow/data/raw/wiki08.txt",
    std::string output_path = "/scratch/shadouaj/gammapdb_arrow/data/raw/wiki08_clean.txt",
    std::string vocab_path = "/scratch/shadouaj/gammapdb_arrow/data/raw/wiki08_clean_vocab.txt",
    std::string stopword_path = "stopwords.txt",
    int reportEvery = 10000,
    int totalNbDocs=5000000)
    {
    IOBuffers buffers;

    // const std::string input_path = "/gammapdb_arrow/data/raw/wiki08.txt";
    // const std::string output_path = "/gammapdb_arrow/data/raw/wiki08_clean.txt";
    // const std::string vocab_path = "/gammapdb_arrow/data/raw/wiki08_clean_vocab.txt";
    // const std::string stopword_path = "/gammapdb_arrow/data/raw/stopwords.txt";

    load_stopwords(stopword_path);
    std::cout << "Stop words loaded, size = " << stopwords.size() << std::endl;

    // First pass: create a hashmap of all the words in the corpus so that we can discard rare words later. We can also use it to discard very common words if we wanted.
    {
        std::ifstream in_file(input_path);
        if (!in_file)
        {
            std::cerr << "Error opening input file: " << input_path << '\n';
            return;
        }
        in_file.rdbuf()->pubsetbuf(buffers.in_buffer.get(), BUFFER_SIZE); // sets the stream buffer to the buffer that I declared before

        std::string line;
        int counter = 0;
        std::vector<std::string> dummy;
        while (std::getline(in_file, line) && counter<=totalNbDocs)
        {
            process_line(line, true, dummy);
            // if (++counter % reportEvery == 0)
            // {
            //     std::cout << "Processed " << counter << " documents (Phase 1)\n";
            //     dummy.clear(); // Prevent memory bloat
            // }
            if (++counter % reportEvery == 0)
            {
                std::cout << "Processed " << counter << " documents (Phase 1)."<<std::endl;
                dummy.clear();
            }
        }
    }

    long long tockens = 0;
    // Second pass
    {
        std::ifstream in_file(input_path);
        std::ofstream out_file(output_path);

        if (!in_file || !out_file)
        {
            std::cerr << "Error opening files for phase 2\n";
            return ;
        }

        in_file.rdbuf()->pubsetbuf(buffers.in_buffer.get(), BUFFER_SIZE);
        out_file.rdbuf()->pubsetbuf(buffers.out_buffer.get(), BUFFER_SIZE);

        std::string line;
        int counter = 0;
        std::vector<std::string> valid_words;
        valid_words.reserve(10000); // Pre-allocate

        while (std::getline(in_file, line)&& counter<=totalNbDocs)
        {
            valid_words.clear();
            process_line(line, false, valid_words);

            if (!valid_words.empty())
            {
                out_file << valid_words[0];
                for (size_t i = 1; i < valid_words.size(); ++i)
                {
                    // std::cout << valid_words[i] << " ";
                    tockens++;
                    out_file << ' ' << valid_words[i];
                }
                out_file << '\n';
            }

            if (++counter % reportEvery == 0)
            {
                std::cout << "Processed " << counter << " documents (Phase 2). Num total tockens so far:" <<tockens<< std::endl;
            }
        }
    }

    generate_vocab(vocab_path);

}


// #include <iostream>
// #include <fstream>
// #include <string>
// #include <unordered_map>
// #include <unordered_set>
// #include <algorithm>
// #include <cctype>
// #include <chrono>
// #include <iomanip>
// #include <ctime>
// #include <sstream>
// #include <vector>

// // Helper function to get formatted current time for logging
// std::string getCurrentTime() {
//     auto now = std::chrono::system_clock::now();
//     auto time = std::chrono::system_clock::to_time_t(now);
//     std::stringstream ss;
//     ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
//     return ss.str();
// }

// bool hasRepeatedLetters(const std::string& word) {
//     for (size_t i = 0; i < word.length() - 2; ++i) {
//         if (word[i] == word[i+1] && word[i] == word[i+2]) {
//             return true;
//         }
//     }
//     return false;
// }

// bool isAlphaOnly(const std::string& word) {
//     return std::all_of(word.begin(), word.end(), [](unsigned char c) {
//         return std::isalpha(c);
//     });
// }

// std::string toLower(const std::string& str) {
//     std::string result = str;
//     std::transform(result.begin(), result.end(), result.begin(),
//                    [](unsigned char c) { return std::tolower(c); });
//     return result;
// }

// int main(int argc, char* argv[]) {
//     // Record start time
//     auto startTime = std::chrono::high_resolution_clock::now();

//     const std::string inputPath = "/scratch/shadouaj/gammapdb_arrow/data/raw/wiki08.txt";
//     const std::string outputPath = "/scratch/shadouaj/gammapdb_arrow/data/raw/wiki08_deepseek.txt";
//     const std::string vocabCsvPath = "/scratch/shadouaj/gammapdb_arrow/data/raw/wiki08_deepseek_vocab.txt";
//     int minOccurrences = 5;

//     // Parse command line arguments
//     if (argc > 1) inputPath = argv[1];
//     if (argc > 2) outputPath = argv[2];
//     if (argc > 3) minOccurrences = std::stoi(argv[3]);

//     std::cout << "[" << getCurrentTime() << "] Starting processing" << std::endl;
//     std::cout << "[" << getCurrentTime() << "] Input file: " << inputPath << std::endl;
//     std::cout << "[" << getCurrentTime() << "] Output file: " << outputPath << std::endl;
//     std::cout << "[" << getCurrentTime() << "] Minimum occurrences: " << minOccurrences << std::endl;

//     // First pass: count valid words
//     std::unordered_map<std::string, int> wordCounter;
//     std::ifstream inFile(inputPath);

//     if (!inFile) {
//         std::cerr << "[" << getCurrentTime() << "] ERROR: Failed to open input file: " << inputPath << std::endl;
//         return 1;
//     }

//     std::string line;
//     std::cout << "[" << getCurrentTime() << "] First pass: counting valid words..." << std::endl;

//     // Counters for logging
//     size_t totalLines = 0;
//     size_t linesWithValidUrls = 0;
//     size_t totalWords = 0;
//     size_t nonAlphaWords = 0;
//     size_t repeatedLetterWords = 0;
//     size_t validWords = 0;

//     // Process in chunks and log progress
//     const size_t logFrequency = 1000000; // Log every million lines

//     while (std::getline(inFile, line)) {
//         totalLines++;

//         // Log progress periodically
//         if (totalLines % logFrequency == 0) {
//             std::cout << "[" << getCurrentTime() << "] Processed " << totalLines << " lines"
//                       << " (" << validWords << " valid word occurrences, " << wordCounter.size() << " unique words)" << std::endl;
//         }

//         // Skip empty lines
//         if (line.empty()) {
//             continue;
//         }

//         // Find first space to separate URL from content
//         size_t spacePos = line.find(" ");

//         if (spacePos != std::string::npos) {
//             // Line has a URL and content
//             linesWithValidUrls++;

//             // Extract content (everything after the first space)
//             std::string content = line.substr(spacePos + 1);
//             std::istringstream contentStream(content);
//             std::string word;

//             while (contentStream >> word) {
//                 totalWords++;

//                 // Convert to lowercase
//                 std::string lowerWord = toLower(word);

//                 // Skip words with non-alphabetic characters
//                 if (!isAlphaOnly(lowerWord)) {
//                     nonAlphaWords++;
//                     continue;
//                 }

//                 // Skip words with three or more consecutive same letters
//                 if (hasRepeatedLetters(lowerWord)) {
//                     repeatedLetterWords++;
//                     continue;
//                 }

//                 // Count valid word
//                 wordCounter[lowerWord]++;
//                 validWords++;
//             }
//         }
//     }

//     inFile.close();

//     // Print first pass statistics
//     std::cout << "[" << getCurrentTime() << "] First pass completed:" << std::endl;
//     std::cout << "  - Total lines processed: " << totalLines << std::endl;
//     std::cout << "  - Lines with valid URLs and content: " << linesWithValidUrls << std::endl;
//     std::cout << "  - Total words examined: " << totalWords << std::endl;
//     std::cout << "  - Words with non-alphabetic characters: " << nonAlphaWords
//               << " (" << (totalWords > 0 ? (nonAlphaWords * 100.0 / totalWords) : 0) << "%)" << std::endl;
//     std::cout << "  - Words with repeated letters: " << repeatedLetterWords
//               << " (" << (totalWords > 0 ? (repeatedLetterWords * 100.0 / totalWords) : 0) << "%)" << std::endl;
//     std::cout << "  - Unique valid words (raw vocabulary size): " << wordCounter.size() << std::endl;
//     std::cout << "  - Total valid word occurrences: " << validWords << std::endl;

//     // Create set of frequent words
//     std::unordered_set<std::string> frequentWords;
//     size_t rareWords = 0;

//     for (const auto& pair : wordCounter) {
//         if (pair.second >= minOccurrences) {
//             frequentWords.insert(pair.first);
//         } else {
//             rareWords++;
//         }
//     }

//     std::cout << "[" << getCurrentTime() << "] Filtering words by frequency:" << std::endl;
//     std::cout << "  - Words appearing fewer than " << minOccurrences << " times: " << rareWords
//               << " (" << (wordCounter.size() > 0 ? (rareWords * 100.0 / wordCounter.size()) : 0) << "%)" << std::endl;
//     std::cout << "  - Words appearing at least " << minOccurrences << " times: " << frequentWords.size()
//               << " (" << (wordCounter.size() > 0 ? (frequentWords.size() * 100.0 / wordCounter.size()) : 0) << "%)" << std::endl;
//     std::cout << "  - Final vocabulary size: " << frequentWords.size() << std::endl;

//     // Write vocabulary to CSV file with header
//     std::ofstream vocabCsvFile(vocabCsvPath);
//     if (vocabCsvFile) {
//         // Write header
//         vocabCsvFile << "wordStr,frequency" << std::endl;

//         // Convert set to vector for sorting
//         std::vector<std::pair<std::string, int>> sortedVocab;
//         for (const auto& word : frequentWords) {
//             sortedVocab.push_back({word, wordCounter[word]});
//         }

//         // Sort by frequency (descending)
//         std::sort(sortedVocab.begin(), sortedVocab.end(),
//                  [](const std::pair<std::string, int>& a, const std::pair<std::string, int>& b) {
//                      return a.second > b.second;
//                  });

//         // Write sorted vocabulary
//         for (const auto& pair : sortedVocab) {
//             vocabCsvFile << pair.first << "," << pair.second << std::endl;
//         }

//         vocabCsvFile.close();
//         std::cout << "[" << getCurrentTime() << "] CSV Vocabulary written to: " << vocabCsvPath << std::endl;
//     } else {
//         std::cerr << "[" << getCurrentTime() << "] ERROR: Failed to create vocabulary CSV file." << std::endl;
//     }

//     // Second pass: write cleaned content
//     std::cout << "[" << getCurrentTime() << "] Second pass: writing cleaned output..." << std::endl;
//     inFile.open(inputPath);
//     std::ofstream outFile(outputPath);

//     if (!inFile || !outFile) {
//         std::cerr << "[" << getCurrentTime() << "] ERROR: Failed to open files for second pass." << std::endl;
//         return 1;
//     }

//     // Reset counters for second pass
//     totalLines = 0;
//     size_t outputLines = 0;
//     size_t outputWords = 0;
//     std::unordered_set<std::string> outputVocabulary;  // Track vocabulary in output

//     while (std::getline(inFile, line)) {
//         totalLines++;

//         // Log progress periodically
//         if (totalLines % logFrequency == 0) {
//             std::cout << "[" << getCurrentTime() << "] Second pass: processed " << totalLines
//                       << " lines, wrote " << outputWords << " words" << std::endl;
//         }

//         // Skip empty lines
//         if (line.empty()) {
//             continue;
//         }

//         // Find first space to separate URL from content
//         size_t spacePos = line.find(" ");

//         if (spacePos != std::string::npos) {
//             // Get URL
//             std::string url = line.substr(0, spacePos);

//             // Extract content and filter words
//             std::string content = line.substr(spacePos + 1);
//             std::istringstream contentStream(content);
//             std::vector<std::string> cleanedWords;
//             std::string word;

//             while (contentStream >> word) {
//                 std::string lowerWord = toLower(word);
//                 // Only keep words in our valid set
//                 if (frequentWords.find(lowerWord) != frequentWords.end()) {
//                     cleanedWords.push_back(lowerWord);
//                     outputVocabulary.insert(lowerWord);  // Add to output vocabulary
//                 }
//             }

//             // Write URL and cleaned content if not empty
//             if (!cleanedWords.empty()) {
//                 outFile << url << " ";

//                 for (size_t i = 0; i < cleanedWords.size(); ++i) {
//                     outFile << cleanedWords[i];
//                     if (i < cleanedWords.size() - 1) {
//                         outFile << " ";
//                     }
//                 }
//                 outFile << std::endl;
//                 outputLines++;
//                 outputWords += cleanedWords.size();
//             }
//         }
//     }

//     inFile.close();
//     outFile.close();

//     // Calculate elapsed time
//     auto endTime = std::chrono::high_resolution_clock::now();
//     auto duration = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime).count();

//     // Print final statistics
//     std::cout << "[" << getCurrentTime() << "] Processing completed:" << std::endl;
//     std::cout << "  - Total execution time: " << duration << " seconds" << std::endl;
//     std::cout << "  - Input statistics:" << std::endl;
//     std::cout << "    * Total lines: " << totalLines << std::endl;
//     std::cout << "    * Lines with URLs and content: " << linesWithValidUrls << std::endl;
//     std::cout << "    * Total words examined: " << totalWords << std::endl;
//     std::cout << "    * Raw vocabulary size (before frequency filtering): " << wordCounter.size() << std::endl;
//     std::cout << "  - Output statistics:" << std::endl;
//     std::cout << "    * Lines written: " << outputLines << std::endl;
//     std::cout << "    * Words written: " << outputWords << std::endl;
//     std::cout << "    * Final vocabulary size: " << frequentWords.size() << std::endl;
//     std::cout << "    * Vocabulary actually used in output: " << outputVocabulary.size() << std::endl;
//     std::cout << "    * Reduction: " << (totalWords > 0 ? (100 - outputWords * 100.0 / totalWords) : 0) << "% fewer words" << std::endl;
//     std::cout << "    * Vocabulary reduction: " << (wordCounter.size() > 0 ? (100 - outputVocabulary.size() * 100.0 / wordCounter.size()) : 0) << "%" << std::endl;
//     std::cout << "[" << getCurrentTime() << "] Cleaned data written to: " << outputPath << std::endl;

//     return 0;
// }