
#ifndef _SAVE_AS_MALLET_BATCH_VI_H
#define _SAVE_AS_MALLET_BATCH_VI_H

#include "Internal_State.h"
#include "lda/Vocabulary.h"
#include "lda_svi/MappingReader.h"
// template<typename M>
void saveAsMalletBatchVI( std::ostream &os, int num_topics, Vocabulary* vocab,  std::vector<int>& data,  std::vector<int>& tokenTopic, MappingReader& mapping)
{
    // get a topic for each token based on the current model
    //  auto& lambdas = is.lambdas;
    // std::cout<<"size of lambdas: " <<lambdas.size()<<std::endl;
    // std::cout<<"size of lambdas inside save as mallet: " <<lambdas.size()<<std::endl;
    // std::vector<int> tokenTopic= getTokenTopic(lambdas, num_topics);
    std::cout<<"inside saveAsMalletBatchVI:  size of tokenTopic: "<<tokenTopic.size()<<std::endl;

    os << "#doc source pos typeindex type topic" << std::endl;
    os << "#alpha : ";
    std::stringstream ss;

      for (int topicId=0; topicId<num_topics; topicId++)
      {
         os << 0.2 << " ";
      }
      os << std::endl;

      os << "#beta : ";
      os << 0.1 << std::endl;


    for (int itau = 0; itau < (data.size()/3); itau++)
    {     
        int* context = &data[itau*3];
        int malletDocId = context[0]+1;
        // int topicId = getTokenTopic(lambdas, docID, itau/num_topics, num_topics);

        int topicId = tokenTopic[mapping.get_output_group(itau)];
        ss << malletDocId << " NA " << itau/num_topics << " " << vocab->getWordByCode(context[2]) << " " << topicId << std::endl;
    }
    os << ss.str();
};


std::vector<int> getTopSortedIndexes(const std::vector<int>& vec, int topN = 10) {
    // Create a vector of indexes: 0, 1, 2, ..., vec.size()-1
    std::vector<int> indexes(vec.size());
    for (int i = 0; i < vec.size(); ++i) {
        indexes[i] = i;
    }


    // Partially sort the first `topN` indexes based on the values in the original vector
    std::partial_sort(indexes.begin(), indexes.begin() + topN, indexes.end(),
                      [&vec](int i1, int i2) { return vec[i1] > vec[i2]; });

    // Resize the vector to include only the first `topN` elements
    indexes.resize(topN);

    return indexes;
}


template<typename M>
void printImportantTopics(Internal_State<M>& is, Vocabulary* vocab)
{ 
    std::cout<<"Important topics"<<std::endl;
      // Dereference vidice since it is a pointer
    M& vidice = *is.vidice;

    // Get the appropriate VIDie object from the tuple
    auto& blue = std::get<1>(vidice);

    int blueNumOutcomes = blue.getNUMOUTCOMES(); 
    int blueNUMVARS     = blue.getNumVar();   
  for(int varID = 0; varID < blueNUMVARS; varID++)
  {
        auto start = blue.mu + (varID * blueNumOutcomes);
        auto end = start + blueNumOutcomes;
        std::vector<int> topicVARID(start, end);
        std::vector<int> sortedIndexes = getTopSortedIndexes(topicVARID);
        std::cout << "Important words for topic " << varID << ": ";
        for(int i = 0; i < 10; i++)
        {
            int wid = sortedIndexes[i];
            std::cout << vocab->getWordByCode(wid) << " ";
        }
        std::cout << std::endl;
  }
};


#endif  // _SAVE_AS_MALLET_BATCH_VI_H