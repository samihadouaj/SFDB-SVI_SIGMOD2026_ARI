#include <algorithm>
#include <vector>
#include <tuple>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <stdexcept>
#include <chrono>

std::string getCurrentTimeStampAsString()
{
    std::time_t currentTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::string timeStr(std::ctime(&currentTime));
    return timeStr.substr(0, timeStr.size() - 1);
}

/**
 * @brief Reads (docId,pos,wordId) CSV and writes:
 *   1) per-document lines: "wordId freq docId ..." (space-separated triplets)
 *   2) mapping file with entries: "pos groupIndex", where groupIndex enumerates
 *      each aggregated (docId,wordId) group in ascending (docId,wordId) order.
 *
 * Input  CSV: header + rows "docId,pos,wordId"
 * Output TXT: one line per docId with sorted wordIds
 * Mapping    : one line per input row's pos: "pos groupIndex"
 */
void write_per_document_and_mapping(
    const std::string& csvFilePath,
    const std::string& perDocOutputFilePath,
    const std::string& mappingFilePath)
{
    // Open input file
    std::ifstream infile(csvFilePath);
    if (!infile.is_open()) {
        throw std::runtime_error("Could not open input file: " + csvFilePath);
    }

    // Skip header
    std::string header;
    if (!std::getline(infile, header)) {
        std::ofstream(perDocOutputFilePath).close();
        std::ofstream(mappingFilePath).close();
        return;
    }

    struct Rec { int docId, wordId, pos; };
    std::vector<Rec> records;
    records.reserve(1 << 20);

    std::string line;
    std::cout << "Parsing file..." << std::endl;
    while (std::getline(infile, line)) {
        if (line.empty()) continue;
        std::istringstream iss(line);
        int docId, pos, wordId;
        char c1, c2;
        if (!(iss >> docId >> c1 >> pos >> c2 >> wordId)) continue;
        if (c1 != ',' || c2 != ',') continue;
        records.push_back({docId, wordId, pos});
    }
    infile.close();
    std::cout << "Parsing ended. Read " << records.size() << " rows." << std::endl;

    // Empty input rows
    if (records.empty()) {
        std::ofstream(perDocOutputFilePath).close();
        std::ofstream(mappingFilePath).close();
        return;
    }

    std::cout << "Sorting by (docId, wordId)..." << std::endl;
    std::sort(records.begin(), records.end(),
              [](const Rec& a, const Rec& b) {
                  return std::tie(a.docId, a.wordId, a.pos)
                       < std::tie(b.docId, b.wordId, b.pos);
              });
    std::cout << "Sorting done." << std::endl;

    std::ofstream perDocOut(perDocOutputFilePath);
    if (!perDocOut.is_open()) {
        throw std::runtime_error("Could not open output file: " + perDocOutputFilePath);
    }
    std::ofstream mapOut(mappingFilePath);
    if (!mapOut.is_open()) {
        throw std::runtime_error("Could not open mapping file: " + mappingFilePath);
    }

    std::cout << "Aggregating, writing per-doc lines, and mapping..." << std::endl;

    // State for per-doc output
    int curDoc = records.front().docId;
    std::vector<std::pair<int,int>> docPairs; // (wordId,count), sorted by wordId
    docPairs.reserve(1024);

    // State for current (docId,wordId) group
    int curWord = records.front().wordId;
    int curCount = 0;
    std::vector<int> curGroupPositions; // all pos belonging to current (docId,wordId)
    curGroupPositions.reserve(64);

    // Global group index for mapping (increments per (docId,wordId) group)
    long long groupIndex = 0;

    auto flush_group_into_doc_and_map = [&](int docId, int wordId) {
        // push word count to current doc line
        docPairs.emplace_back(wordId, curCount);
        // write pos -> groupIndex mappings
        for (int p : curGroupPositions) {
            mapOut << p << ' ' << groupIndex << '\n';
        }
        ++groupIndex;
        // reset group state
        curCount = 0;
        curGroupPositions.clear();
    };

    auto flush_doc_line = [&](int docId) {
        // docPairs already in ascending wordId due to traversal order
        for (size_t i = 0; i < docPairs.size(); ++i) {
            if (i) perDocOut << ' ';
            perDocOut << docPairs[i].first << ' ' << docPairs[i].second;
        }
        perDocOut << '\n';
        docPairs.clear();
    };

    // Main pass
    for (size_t i = 0; i < records.size(); ++i) {
        const auto& r = records[i];

        // New document boundary
        if (r.docId != curDoc) {
            // Close last (docId,wordId) group within previous doc
            if (curCount > 0) {
                flush_group_into_doc_and_map(curDoc, curWord);
            }
            // Emit the finished document line
            flush_doc_line(curDoc);

            // Reset for new doc
            curDoc = r.docId;
            curWord = r.wordId;
            curCount = 1;
            curGroupPositions.clear();
            curGroupPositions.push_back(r.pos);
            continue;
        }

        // Same document, check word boundary
        if (r.wordId == curWord) {
            ++curCount;
            curGroupPositions.push_back(r.pos);
        } else {
            // Close previous word group in same doc
            if (curCount > 0) {
                flush_group_into_doc_and_map(curDoc, curWord);
            }
            // Start new word group
            curWord = r.wordId;
            curCount = 1;
            curGroupPositions.clear();
            curGroupPositions.push_back(r.pos);
        }
    }

    // Flush trailing group and document
    if (curCount > 0) {
        flush_group_into_doc_and_map(curDoc, curWord);
    }
    flush_doc_line(curDoc);

    perDocOut.close();
    mapOut.close();
    std::cout << "Done." << std::endl;
}

int main(int argc, const char** argv)
{
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0]
                  << " <input_train.csv> <output_train_svi.csv> <output_train_svi_mapping.csv>\n"
                  << "  input: CSV with a header line and rows docId,pos,wordId\n";
        return 1;
    }
    try {
        write_per_document_and_mapping(argv[1], argv[2], argv[3]);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}



 