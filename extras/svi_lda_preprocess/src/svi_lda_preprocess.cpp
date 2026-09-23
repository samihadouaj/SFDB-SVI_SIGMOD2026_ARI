#include <algorithm> // std::fill
#include <vector>    // std::vector
#include <tuple>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <stdexcept>
#include <chrono>
#include "Utils/argparse.hpp" 



std::string getCurrentTimeStampAsString()
{
   std::time_t currentTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
   std::string timeStr(std::ctime(&currentTime));
   return timeStr.substr(0, timeStr.size() - 1);
}





/**
 * @brief Aggregates word occurrences per document from a CSV file and writes the results to disk.
 *
 * This function reads a CSV file where each line contains a (docId, pos, wordId) triple representing
 * the occurrence of a word in a document at a specific position. It then groups entries by (docId, wordId),
 * counts the occurrences (i.e., number of positions), and writes the aggregated results to an output CSV file
 * in the format (docId, wordId, count).
 *
 * Additionally, it writes a mapping from the original input position (`pos`) to the index of the aggregated group 
 * in the output file. This mapping is useful for tracing each individual input record to its aggregated output.
 *
 * @param csvFilePath Path to the input CSV file containing (docId, pos, wordId) entries.
 * @param outputCsvFilePath Path to the output CSV file where grouped results (docId, wordId, count) are written.
 * @param mappingFilePath Path to the mapping file that records input `pos` → output group index mappings.
 * 
 * @return A flat vector of integers representing the aggregated records:
 *         [docId_1, wordId_1, count_1, docId_2, wordId_2, count_2, ...].
 *         Each group of three integers corresponds to one aggregated (docId, wordId) group.
 *
 * @throws std::runtime_error If any of the input or output files cannot be opened.
 */

std::vector<int> collapse_data_to_disk(
    const std::string& csvFilePath, 
    const std::string& outputCsvFilePath,
    const std::string& mappingFilePath // New parameter for the mapping file
) {

    // Open input file
    std::ifstream infile(csvFilePath);
    if (!infile.is_open()) {
        throw std::runtime_error("Could not open input file: " + csvFilePath);
    }

    // Skip header
    std::string header;
    if (!std::getline(infile, header)) {
        return {};
    }

    // Open output files
    std::ofstream outfile(outputCsvFilePath);
    if (!outfile.is_open()) {
        throw std::runtime_error("Could not open output file: " + outputCsvFilePath);
    }
    outfile << "docId,wordId,count\n"; // Simplified header (removed "k")

    std::ofstream mapfile(mappingFilePath);
    if (!mapfile.is_open()) {
        throw std::runtime_error("Could not open mapping file: " + mappingFilePath);
    }

    // Read and expand data
    struct Record { 
        int docId, wordId, pos; // Added pos to track input position
    };
    std::vector<Record> records;
    std::string line;
    // Read each line and parse the CSV
    std::cout<<"Parsing file..."<<std::endl;
    while (std::getline(infile, line)) {
        if (line.empty()) continue;

        std::istringstream iss(line);
        int docId, pos, wordId;
        char comma;

        // Parse original 3-column format (docId, pos, wordId)
        if (!(iss >> docId >> comma >> pos >> comma >> wordId)) {
            continue; // Skip malformed lines
        }

        records.push_back({docId, wordId, pos});
    }

    std::cout<<"Parsing file ended..."<<std::endl;

    std::cout<<"Sorting records..."<<std::endl;
    // Sort for grouping by (docId, wordId)
    std::sort(records.begin(), records.end(), [](const auto& a, const auto& b) {
        return std::tie(a.docId, a.wordId) < std::tie(b.docId, b.wordId);
    });
    std::cout<<"Sorting records ended..."<<std::endl;

    // Aggregate and output
    std::vector<int> result;
    if (records.empty()) return result;

    // Track groups and write mappings
    size_t startIdx = 0; // Start index of the current group
    int outputIndex = 0; // Index of the output group

    auto write_group = [&](size_t start, size_t end) {
        // Write aggregated output
        const int count = end - start + 1; // Number of records in the group
        const int docId = records[start].docId;
        const int wordId = records[start].wordId;

        result.insert(result.end(), {docId, wordId, count});
        outfile << docId << "," << wordId << "," << count << "\n";

        // Write mapping entries: input_pos -> outputIndex
        for (size_t i = start; i <= end; ++i) {
            mapfile << records[i].pos << " " << outputIndex << "\n";
        }

        ++outputIndex; // Increment for the next group
    };

    std::cout<<"Writing groups..."<<std::endl;
    // Iterate through sorted records to find group boundaries
    for (size_t i = 1; i < records.size(); ++i) {
        if (records[i].docId == records[startIdx].docId && 
            records[i].wordId == records[startIdx].wordId) {
            continue; // Still in the same group
        } else {
            // End of current group: write it
            write_group(startIdx, i - 1);
            startIdx = i; // Start new group
        }
    }

    // Write the last group
    write_group(startIdx, records.size() - 1);
    std::cout<<"Writing groups..."<<std::endl;

    return result;
}



int main(int argc, const char** argv) {
    const bool isOptionalArg = true;
    // default arguments
    std::string datasetName = "wiki08";
    std::string datasetNameSuffix = "";
    std::string outputDir = "/gammapdb_arrow/data";
    std::string datasetsDir = "/gammapdb_arrow/data/raw";

    ArgumentParser argsParser;
    // declare arguments
    argsParser.addArgument("--dataset", 1, isOptionalArg);
    argsParser.addArgument("--datasetNameSuffix", 1, isOptionalArg); 
    // mandatory args
    argsParser.addArgument("--outputDir", 1, isOptionalArg);
    argsParser.addArgument("--datasetsDir", 1, isOptionalArg);
 
    // parse arguments
    argsParser.parse(argc, argv);
 
    // read optional arguments
    if (argsParser.count("dataset"))
    {
       datasetName = argsParser.retrieve<std::string>("dataset");
    }
    if (argsParser.count("datasetNameSuffix"))
    {
       datasetNameSuffix = argsParser.retrieve<std::string>("datasetNameSuffix");
    }

    if (argsParser.count("outputDir"))
    {
       outputDir = argsParser.retrieve<std::string>("outputDir");
    }
    if (argsParser.count("datasetsDir"))
    {
       datasetsDir = argsParser.retrieve<std::string>("datasetsDir");
    }



    std::string csvFilePath     =  datasetsDir + "/" + datasetName + datasetNameSuffix + "_train.csv";
    std::string outputCsvFilePath = outputDir + "/" + datasetName + datasetNameSuffix + "_train_svi.csv";
    std::string mappingFilePath   = outputDir + "/" + datasetName + datasetNameSuffix + "_train_svi_mapping.csv";

 
    // print arguments
    std::cout << "-----------------------------------------------------------" << std::endl;
    std::cout << "WIKI LDA PREPROCESS" << std::endl;
    std::cout << "-----------------------------------------------------------" << std::endl;
    std::cout << "current time         = " << getCurrentTimeStampAsString() << std::endl;
    std::cout << "dataset              = " << datasetName << datasetNameSuffix << std::endl;
    std::cout << "outputDir            = " << outputDir << std::endl;
    std::cout << "datasetsDir       = " << datasetsDir << std::endl;
    std::cout << "csvFilePath         = " << csvFilePath << std::endl;
    std::cout << "outputCsvFilePath    = " << outputCsvFilePath << std::endl;
    std::cout << "mappingFilePath      = " << mappingFilePath << std::endl;
    std::cout << "-----------------------------------------------------------" << std::endl;
 

    


    // Call the function with the provided file paths.
    std::vector<int> result = collapse_data_to_disk(csvFilePath, outputCsvFilePath, mappingFilePath);

    // (Optional) Process or display the result.
    std::cout << "Aggregation complete. Processed " 
                << result.size() / 3 << " elements" << std::endl;

    return 0;
    }




    // collapse_data_to_disk("/gammapdb_arrow/data/KOS_train/csv2/KOS_train.csv", "/gammapdb_arrow/data/KOS_train/csv2/KOS_train_svi.csv","/gammapdb_arrow/data/KOS_train/csv2/KOS_train_svi_mapping.csv");

    // collapse_data_to_disk("/gammapdb_arrow/data/NYTIMES_train/csv2/NYTIMES_train.csv", "/gammapdb_arrow/data/NYTIMES_train/csv2/NYTIMES_train_svi.csv","/gammapdb_arrow/data/NYTIMES_train/csv2/NYTIMES_train_svi_mapping.csv");

    // collapse_data_to_disk("/gammapdb_arrow/data/PUBMED_train/csv2/PUBMED_train.csv", "/gammapdb_arrow/data/PUBMED_train/csv2/PUBMED_train_svi.csv","/gammapdb_arrow/data/PUBMED_train/csv2/PUBMED_train_svi_mapping.csv");

    // collapse_data_to_disk("/gammapdb_arrow/data/wiki08_train/csv2/wiki08_train.csv", "/gammapdb_arrow/data/wiki08_train/csv2/wiki08_train_svi.csv","/gammapdb_arrow/data/wiki08_train/csv2/wiki08_train_svi_mapping.csv");



  

