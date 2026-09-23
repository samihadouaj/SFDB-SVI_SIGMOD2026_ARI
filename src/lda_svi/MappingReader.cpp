#include <unordered_map>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include "MappingReader.h"


// The mapping file is used during the evaluation to allign the collpased data and the original data format.
// More precisely, when training the model, we have one local param (lambda) per word in the "collapsed data".
// When evaluating the lda model, we want to know for each word in the original data, which local param (lambda) it corresponds to.
// This is done by reading the mapping file, which contains the mapping between input positions and output groups.
// The mapping file is a CSV file with two columns: input_pos and output_group.
// The input_pos is the position of the word in the collapsed data, and the output_group is the group (lambda) it corresponds to.
// The mapping file is read and stored in a hash map for fast access during evaluation.

    // Constructor that reads the mapping file
     MappingReader::MappingReader(const std::string& mapping_file_path) {
        std::ifstream file(mapping_file_path);
        if (!file.is_open()) {
            throw std::runtime_error("Could not open mapping file: " + mapping_file_path);
        }

        std::string line;
        while (std::getline(file, line)) {
            if (line.empty()) continue;

            std::istringstream iss(line);
            int input_pos, output_group;
            
            if (!(iss >> input_pos >> output_group)) {
                throw std::runtime_error("Invalid format in mapping file line: " + line);
            }
            
            position_map[input_pos] = output_group;
        }
    }

    // Get output group for a given input position
    int MappingReader::get_output_group(int input_position) const {
        auto it = position_map.find(input_position);
        if (it == position_map.end()) {
            throw std::out_of_range("Input position not found in mapping: " + std::to_string(input_position));
        }
        return it->second;
    }

    // Optional: Get all mappings (input_pos -> output_group)
    const std::unordered_map<int, int>& MappingReader::get_all_mappings() const {
        return position_map;
    };