#ifndef _SVI_LDA_H_
#define _SVI_LDA_H_

#include "InternalState.h"
#include "ReadData.h"
#include <vector>
#include "MappingReader.h"
#include "Vocabulary.h"

void runSVI(
    std::vector<SparseDocument> &data,
    int num_vars_red,
    int num_outcomes_red,
    int num_vars_blue,
    int num_outcomes_blue,
    int nb_sat_assig_per_constraint, // K
    int batchSize,
    int total_num_iter,
    int save_every,
    const std::vector<float> &priors, // [0]=red prior (scalar or per-topic), [1]=blue prior (scalar)
    const std::string &malletStateDir,
    Vocabulary &malletVocab,
    MappingReader &mapping,
    const std::string &outfileId,
    int vi_iter_per_batch,
    std::vector<int> &data_eval // the data from the csv used for evaluation
);





void updateLocalParmas(std::vector<float> &mu_blue_hat, const std::vector<SparseDocument> &data, Internal_State &internal, int vi_iter_per_batch, long long startIdx, int batchSize, int nb_sat_assig_per_constraint, const std::vector<float> &prior,double scaling_factor);



 void printTopics(const std::vector<float>& mu_blue,
                              int K,                 // number of topics
                              const Vocabulary& vocab,
                              int topN = 10,         // how many words per topic
                              bool normalize = false,// show probabilities (row-normalized)
                              std::ostream& os = std::cout);
#endif // _SVI_LDA_H_