#ifndef _SAVEMALLET_H_
#define _SAVEMALLET_H_

#include <iostream>    
#include <ostream>     
#include <sstream>     
#include <vector>      
#include <string>      
#include "Vocabulary.h"    
#include "MappingReader.h" 






inline void saveAsMalletBatchVI(std::ostream &os, int num_topics, Vocabulary* vocab,
                               std::vector<int>& data, std::vector<int>& tokenTopic,
                               MappingReader& mapping) {
    // Write header
    os << "#doc source pos typeindex type topic\n";
    os << "#alpha : ";
    for (int topicId = 0; topicId < num_topics; topicId++) {
        os << 0.2 << " ";
    }
    os << "\n#beta : " << 0.1 << "\n";
    os.flush();
    
    size_t c = 0;
    const size_t total_iterations = data.size() / 3;
    const size_t buffer_size = 300000000; // Lines to buffer before flush
    
    std::string buffer;
    buffer.reserve(50000000); // ~5MB buffer
    
    for (size_t itau = 0; itau < total_iterations; itau++) {
        const size_t base_index = itau * 3;
        
        int malletDocId = data[base_index] + 1;
        int topicId = tokenTopic[mapping.get_output_group(itau)];
        int wordCode = data[base_index + 2];
        
        // Append to buffer
        buffer += std::to_string(malletDocId) + " NA " + 
                  std::to_string(itau / num_topics) + " " +
                  vocab->getWordByCode(wordCode) + " " +
                  std::to_string(topicId) + "\n";
        
        c++;
        
        // Flush buffer when it gets large enough
        if (c % buffer_size == 0) {
            os << buffer;
            os.flush();
            buffer.clear();
            buffer.reserve(50000000);
            
            if (c % 50000000 == 0) {
                std::cout << "Progress: " << c << " lines written" << std::endl;
            }
        }
    }
    
    // Write any remaining buffered data
    if (!buffer.empty()) {
        os << buffer;
        os.flush();
    }
    
    std::cout << "Wrote " << c << " tokens to Mallet state." << std::endl;
}
#endif // _SAVEMALLET_H_