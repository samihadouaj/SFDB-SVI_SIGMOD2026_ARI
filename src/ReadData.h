#ifndef _READDATA_H_
#define _READDATA_H_
#include <algorithm> // std::fill
#include <vector>    // std::vector
#include <tuple>
#include <iostream>
#include <fstream>
#include <sstream>

std::vector<std::vector<int>> parseFileToVector(const std::string& filename);
std::vector<int> parseFileToFlatVector(const std::string& filename);
// int* parseFileToFlatArray(const std::string& filename, size_t& numRows) ;
std::vector<int*> split_into_batches(std::vector<int>& data, int batch_size, int nb_sat_assign_per_constr);
long long get_nb_sat_assign_in_batch(int batch_index, const std::vector<int*>& batch_pointers, const std::vector<int>& data, int batch_size, int nb_sat_assign_per_constr);
std::vector<int> collapse_data_from_disk(std::string& csvFilePath);
std::vector<int> collapse_data_to_disk(
    const std::string& csvFilePath, 
    const std::string& outputCsvFilePath
);
#endif  // _READDATA_H_
