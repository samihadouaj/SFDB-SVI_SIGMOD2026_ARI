#ifndef _SVI_LDAP_H_
#define _SVI_LDAP_H_

#include "InternalState.h"
#include "ReadData.h"
#include <vector>
#include "MappingReader.h"
#include "Vocabulary.h"
#include "omp.h"

void runSVIP(
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
    std::vector<int> &data_eval, // the data from the csv used for evaluation
    int num_threads
);

void updateLocalParmasP(std::vector<float> &mu_blue_hat, const std::vector<SparseDocument> &data, Internal_State &internal, int vi_iter_per_batch, size_t startIdx, int batchSize, int nb_sat_assig_per_constraint, const std::vector<float> &prior,double scaling_factor,int num_threads);



#endif // _SVI_LDAP_H_
