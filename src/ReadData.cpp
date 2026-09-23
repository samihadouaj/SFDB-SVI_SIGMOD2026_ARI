#include "ReadData.h"
#include <tuple>
#include <map>
#include <string>
#include <fstream>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include "utils/DebugHelper.h"
#include <unordered_map>
#include <map>



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


// std::vector<int*> split_into_batches(std::vector<int>& data, int batch_size,int nb_sat_assig_per_constraint) {
//     std::vector<int*> batch_pointers;
    
//     int sat_assign_size = 4; // Each sat_assign consists of 4 elements
//     long long nb_elemnts_per_batch = sat_assign_size*nb_sat_assig_per_constraint;
//     int total_nb_constr = data.size() / nb_elemnts_per_batch; 

//     int num_batches = (total_nb_constr + batch_size - 1) / batch_size; // Compute number of batches

//     for (int i = 0; i < num_batches; i++) {
//         long long start_idx = static_cast<long long>(i) * batch_size * nb_elemnts_per_batch;
//         if (start_idx < data.size()) {
//             batch_pointers.push_back(&data[start_idx]); // Store pointer to batch start
//         }
//     }

//     return batch_pointers;
// }

// long long get_nb_sat_assign_in_batch(int batch_index, const std::vector<int*>& batch_pointers, const std::vector<int>& data, int batch_size, int nb_sat_assig_per_constraint) 
// {
//     int sat_assign_size = 4;  // Each sat_assign has 4 elements
//     long long batch_element_count = batch_size * nb_sat_assig_per_constraint * sat_assign_size; // Number of elements per batch
 
//     long long start_idx = static_cast<long long>(batch_index) * batch_element_count; // Start index in original data
//     // Check if this is the last batch and has fewer elements
//     if (start_idx + batch_element_count > data.size()) {

//         return( data.size() - start_idx)/sat_assign_size; // Return remaining elements
//     }

//     return batch_element_count/sat_assign_size; // Return full batch size
// }

// // reads the data from disk and callapses it and puts in into a flat vector.
// std::vector<int> collapse_data_from_disk( std::string& csvFilePath)
// {
//     // Open CSV
//     std::ifstream infile(csvFilePath);
//     if (!infile.is_open()) {
//         throw std::runtime_error("Could not open file: " + csvFilePath);
//     }

//     // Skip the header line, e.g. "docId,pos,wordId,k"
//     std::string header;
//     if (!std::getline(infile, header)) {
//         // If file is empty or has no header
//         return {};
//     }

//     // We'll store (docId, wordId, topic) temporarily.
//     struct Record {
//         int docId;
//         int wordId;
//         int topic;
//     };
//     std::vector<Record> records;
//     records.reserve(8000000000);  // Optional pre-reserve if you have an estimate

//     // Read file lines:
//     // Format: docId, pos, wordId, k
//     std::string line;
//     while (std::getline(infile, line)) {
//         if (line.empty()) continue;

//         std::istringstream iss(line);
//         int docId, pos, wordId, k;
//         char comma;

//         // Expect "docId,pos,wordId,k"
//         // e.g. "0,0,0,2"
//         if (!(iss >> docId >> comma >> pos >> comma >> wordId >> comma >> k)) {
//             // Handle parse errors if necessary
//             continue;
//         }

//         // We ignore 'pos' (the second field)
//         // We'll group by docId, wordId, topic == k
//         records.push_back({docId, wordId, k});
//     }

//     // Sort by (docId, wordId, topic)
//     std::sort(records.begin(), records.end(), 
//               [](const Record& a, const Record& b) {
//                   if (a.docId != b.docId)    return a.docId < b.docId;
//                   if (a.wordId != b.wordId) return a.wordId < b.wordId;
//                   return a.topic < b.topic;
//               });

//     // Now, aggregate consecutive identical (docId, wordId, topic)
//     std::vector<int> result;
//     result.reserve(records.size() * 2); // Rough estimate

//     if (records.empty()) {
//         return result;
//     }

//     // Start grouping with the first record
//     int currentDocId  = records[0].docId;
//     int currentWordId = records[0].wordId;
//     int currentTopic  = records[0].topic;
//     int currentCount  = 1;

//     for (size_t i = 1; i < records.size(); ++i) {
//         const auto& r = records[i];
//         if (r.docId == currentDocId &&
//             r.wordId == currentWordId &&
//             r.topic == currentTopic) 
//         {
//             // same group
//             currentCount++;
//         } else {
//             // group finished -> store result
//             result.push_back(currentDocId);
//             result.push_back(currentWordId);
//             result.push_back(currentTopic);
//             result.push_back(currentCount);

//             // start a new group
//             currentDocId  = r.docId;
//             currentWordId = r.wordId;
//             currentTopic  = r.topic;
//             currentCount  = 1;
//         }
//     }

//     // push the last group
//     result.push_back(currentDocId);
//     result.push_back(currentWordId);
//     result.push_back(currentTopic);
//     result.push_back(currentCount);

//     return result;
// }



// std::vector<int> collapse_data_to_disk(
//     const std::string& csvFilePath, 
//     const std::string& outputCsvFilePath
// ) {
//     // Open input file
//     std::ifstream infile(csvFilePath);
//     if (!infile.is_open()) {
//         throw std::runtime_error("Could not open input file: " + csvFilePath);
//     }

//     // Skip header
//     std::string header;
//     if (!std::getline(infile, header)) {
//         return {};
//     }

//     // Open output file early to catch errors
//     std::ofstream outfile(outputCsvFilePath);
//     if (!outfile.is_open()) {
//         throw std::runtime_error("Could not open output file: " + outputCsvFilePath);
//     }
//     outfile << "docId,wordId,k,count\n"; // Write CSV header

//     // Read and parse data
//     struct Record { int docId, wordId, topic; };
//     std::vector<Record> records;
//     std::string line;

//     while (std::getline(infile, line)) {
//         if (line.empty()) continue;

//         std::istringstream iss(line);
//         int docId, pos, wordId, k;
//         char comma;

//         if (!(iss >> docId >> comma >> pos >> comma >> wordId >> comma >> k)) {
//             continue; // Skip malformed lines
//         }

//         records.push_back({docId, wordId, k});
//     }

//     // Sort records
//     std::sort(records.begin(), records.end(), [](const auto& a, const auto& b) {
//         return std::tie(a.docId, a.wordId, a.topic) < 
//                std::tie(b.docId, b.wordId, b.topic);
//     });

//     // Aggregate and write results
//     std::vector<int> result;
//     if (records.empty()) return result;

//     int currentDocId = records[0].docId;
//     int currentWordId = records[0].wordId;
//     int currentTopic = records[0].topic;
//     int currentCount = 1;

//     auto write_group = [&]() {
//         // Add to result vector
//         result.insert(result.end(), {currentDocId, currentWordId, currentTopic, currentCount});
        
//         // Write to CSV
//         outfile << currentDocId << ","
//                 << currentWordId << ","
//                 << currentTopic << ","
//                 << currentCount << "\n";
//     };

//     for (size_t i = 1; i < records.size(); ++i) {
//         const auto& r = records[i];
//         if (r.docId == currentDocId && 
//             r.wordId == currentWordId && 
//             r.topic == currentTopic) {
//             currentCount++;
//         } else {
//             write_group();
//             currentDocId = r.docId;
//             currentWordId = r.wordId;
//             currentTopic = r.topic;
//             currentCount = 1;
//         }
//     }

//     // Write final group
//     write_group();

//     return result;
// }
