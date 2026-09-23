// #include <iostream>
// #include <fstream>
// #include <string>
// #include <vector>
// #include <algorithm> 
// #include <random> 
// #include <chrono>     // std::chrono



// std::string getCurrentTimeStampAsString()
// {
//    std::time_t currentTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
//    std::string timeStr(std::ctime(&currentTime));
//    return timeStr.substr(0, timeStr.size()-1);
// }

// int main() {
//     std::ifstream input("/home/shadouaj/processingWIKIforGammapdb/data/wiki_train/wikiTOY10K.txt");
//     std::string line;
//     std::vector<std::string> lines;
//     std::cout<<"reading file ..."<<std::endl;
//     while (getline(input, line)) {
//         lines.push_back(line);
//     }
//     input.close();
    
//     // Shuffling the vector
//     std::random_device rd;
//     std::mt19937 g(rd());
//     std::shuffle(lines.begin(), lines.end(), g);

//     // Dividing the vector in half
//     size_t half_size = lines.size()/20;
//     std::ofstream output1("output1.txt"), output2("output2.txt");
//     std::cout<<"printing the test documents into the file"<<std::endl;
//     for (size_t i = 0; i < half_size; ++i) {

//         output1 << lines[i] << std::endl;
//         if(i%1000==0)
//         {
//         std::cout << "[" << getCurrentTimeStampAsString() << "] " << i << " documents processed ..." << std::endl;
//         }
//     }
//     std::cout<<"printing the train documents into the file"<<std::endl;
//     for (size_t i = half_size; i < lines.size(); ++i) {
//         output2 << lines[i] << std::endl;
//                 if(i%1000==0)
//         {
//         std::cout << "[" << getCurrentTimeStampAsString() << "] " << i << " documents processed ..." << std::endl;
//         }
//     }

//     output1.close();
//     output2.close();

//     return 0;
// }