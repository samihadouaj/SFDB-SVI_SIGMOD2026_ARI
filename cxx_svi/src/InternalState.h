#ifndef INTERNAL_STATE_H
#define INTERNAL_STATE_H

#include <algorithm>  // std::fill
#include <cstddef>
#include <iostream>
#include <random>
#include <vector>
#include <iomanip>
#include "ViUtils.h"  // digamma()

// This is a struct to hold the variational parameters for both the topic-words distribution and document-topics distribution. In this context, we call these object a die. 
struct Die {
    int num_vars = 0;
    int num_outcomes = 0;

    std::vector<float> mu;               // size: num_vars * num_outcomes
    std::vector<float> prior;            // size: num_vars * num_outcomes
    std::vector<float> sum_mu;           // size: num_vars
    std::vector<float> digamma_mu;       // size: num_vars * num_outcomes
    std::vector<float> digamma_sum_mu;   // size: num_vars


    Die(int nvars, int noutcomes,
          float mu_init = -1.0f,
          float sum_init = -1.0f,
          float digamma_init = 0.0f)
        : num_vars(nvars),
          num_outcomes(noutcomes),
          mu(static_cast<size_t>(nvars) * noutcomes, mu_init),
          prior(static_cast<size_t>(nvars) * noutcomes, 0.0f),
          sum_mu(nvars, sum_init),
          digamma_mu(static_cast<size_t>(nvars) * noutcomes, digamma_init),
          digamma_sum_mu(nvars, digamma_init)
    {}
};


// This class holds the internal state of the SVI algorithm. It includes both dice (topic-words distribution and document-topics distribution) and the lambda parameters which are the token-topic probabilities. 
class Internal_State {
private:
    // std::vector<float> lambdas;              
    long long total_unique_nb_sat_assig ; 
    int nb_sat_assig_per_constraint;


void initmuGamma(Die& die,float shape = 100.0f, float scale = 0.01f, unsigned seed = 123) {
    std::mt19937 gen(seed); 
    std::gamma_distribution<float> gammaDist(shape, scale);
    for (int i = 0; i < die.num_vars * die.num_outcomes; i++) {
        die.mu[i] = gammaDist(gen);
    }
}
public:


    Die red;
    Die blue;

    explicit Internal_State(long long m,
                            int k,
                            int numvars_red,
                            int numoutcomes_red,
                            int numvars_blue,
                            int numoutcomes_blue,
                            bool init_lambda = true)
        : total_unique_nb_sat_assig(m),
          nb_sat_assig_per_constraint(k),
          red(numvars_red,  numoutcomes_red),
          blue(numvars_blue, numoutcomes_blue)
    {
        // if (init_lambda) {
        //     lambdas.assign(static_cast<size_t>(total_unique_nb_sat_assig), 0.0f);
        // }
    }

    // If the object owns lots of memory, you can forbid copying and allow moving:
    Internal_State(const Internal_State&) = delete;
    Internal_State& operator=(const Internal_State&) = delete;
    Internal_State(Internal_State&&) = default;
    Internal_State& operator=(Internal_State&&) = default;


void initmuGamma() {
    initmuGamma(red);
    initmuGamma(blue);
}

void set_mu_blue(int index,float value){
    blue.mu[index] = value;
}
void set_mu_red(int index,float value){
    red.mu[index] = value;
}


void compute_digamma_mu(Die& die) {
    for (int i = 0; i < die.num_vars * die.num_outcomes; i++) {
        __builtin_prefetch(&die.mu[i + 4]);
        die.digamma_mu[i] = digamma(die.mu[i]);
    }
}

void compute_sum_mu_and_digammaSumMu(Die & die) {
    for (int varId = 0; varId < die.num_vars; ++varId) {
        float sum = 0.0f;
        for (int outcome = 0; outcome < die.num_outcomes; ++outcome) {
            sum += die.mu[varId * die.num_outcomes + outcome];
        }
        die.sum_mu[varId] = sum;
        die.digamma_sum_mu[varId] = digamma(sum);
    }
}


inline void compute_digamma_muP(Die& die) {
    const int N = die.num_vars * die.num_outcomes;
    #pragma omp for
    for (int i = 0; i < N; ++i) {
        __builtin_prefetch(&die.mu[i + 4]);
        die.digamma_mu[i] = digamma(die.mu[i]);
    }
}

inline void compute_sum_mu_and_digammaSumMuP(Die& die) {
    #pragma omp for
    for (int varId = 0; varId < die.num_vars; ++varId) {
        float sum = 0.0f;
        for (int outcome = 0; outcome < die.num_outcomes; ++outcome) {
            sum += die.mu[varId * die.num_outcomes + outcome];
        }
        die.sum_mu[varId]         = sum;
        die.digamma_sum_mu[varId] = digamma(sum);
    }
}


void prepare_red_die() {
    {
        compute_digamma_mu(red);
        compute_sum_mu_and_digammaSumMu(red);
    }
}
void prepare_blue_die() {
    {
        compute_digamma_mu(blue);
        compute_sum_mu_and_digammaSumMu(blue);
    }
}




void prepare_red_dieP(int num_threads) {
    #pragma omp parallel num_threads(num_threads)
    {
        compute_digamma_muP(red);
        compute_sum_mu_and_digammaSumMuP(red);
    }
}

void prepare_blue_dieP(int num_threads) {
    #pragma omp parallel num_threads(num_threads)
    {
        compute_digamma_muP(blue);
        compute_sum_mu_and_digammaSumMuP(blue);
    }
}



inline void compute_digamma_mu_for_var(Die& die, size_t varId) {
    const int O = die.num_outcomes;
    const int base = varId * O;
    for (int o = 0; o < O; ++o) {
        __builtin_prefetch(&die.mu[base + o + 4]);
        die.digamma_mu[base + o] = digamma(die.mu[base + o]);
    }
}

// inline void compute_digamma_mu_for_varP(Die& die, int varId) {
//     const int O = die.num_outcomes;
//     const int base = varId * O;
//    #pragma omp for
//     for (int o = 0; o < O; ++o) {
//         __builtin_prefetch(&die.mu[base + o + 4]);
//         die.digamma_mu[base + o] = digamma(die.mu[base + o]);
//     }
// }

inline void compute_sum_mu_and_digammaSumMu_for_var(Die& die, size_t varId) {
    const int O = die.num_outcomes;
    const int base = varId * O;
    float sum = 0.0f;
    for (int o = 0; o < O; ++o) {
        sum += die.mu[base + o];
    }
    die.sum_mu[varId]         = sum;
    die.digamma_sum_mu[varId] = digamma(sum);
}



// inline void compute_sum_mu_and_digammaSumMu_for_varP(Die& die, int varId) {
//     const int O = die.num_outcomes;
//     const int base = varId * O;
//     float sum = 0.0f;
//     #pragma omp for
//     for (int o = 0; o < O; ++o) {
//         sum += die.mu[base + o];
//     }
//     die.sum_mu[varId]         = sum;
//     die.digamma_sum_mu[varId] = digamma(sum);
// }



void prepare_red_die_for_doc(size_t docId) {
    compute_digamma_mu_for_var(red, docId);
    compute_sum_mu_and_digammaSumMu_for_var(red, docId);
}


// void prepare_red_die_for_docP(int docId, int num_threads) {
//     #pragma omp parallel num_threads(num_threads)
//     {
//         compute_digamma_mu_for_varP(red, docId);
//         compute_sum_mu_and_digammaSumMu_for_varP(red, docId);
//     }
// }






    // //this will normalize lambda in groups of nb_sat_assig_per_constr
    // void normalizeLambdas()
    // {
    // // Iterate over data in steps of K
    // for (std::size_t i = 0; i < lambdas.size(); i += nb_sat_assig_per_constraint) {
    //     // 1) Compute sum of the block [i, i+K), clamped to lambdas.size()
    //     float blockSum = 0.0;
    //     std::size_t blockEnd = i+nb_sat_assig_per_constraint;
    //     for (std::size_t j = i; j < blockEnd; j++) {
    //         blockSum += lambdas[j];
    //     }

    //     // 2) Normalize that block
    //     if (blockSum != 0.0) {
    //         for (std::size_t j = i; j < blockEnd; j++) {
    //             lambdas[j] /= blockSum;
    //         }
    //     } 
    //     else
    //     {
    //         std::fill(lambdas.begin() + i, lambdas.begin() + blockEnd, 0.0);
    //     }

    // }
    // }






    // void normalizeLambdasP(int num_threads,std::vector<float>& lambdas) {
    //     const std::size_t K = nb_sat_assig_per_constraint;
    //     const std::size_t n = lambdas.size();
    //     const std::size_t num_blocks = (n + K - 1) / K;
    
    //     #pragma omp parallel for num_threads(num_threads)
    //     for (std::size_t block_id = 0; block_id < num_blocks; ++block_id) {
    //         const std::size_t i = block_id * K;
    //         const std::size_t end = i+K;
    
    //         // Compute the sum for the current block
    //         float block_sum = 0.0f;
    //         for (std::size_t j = i; j < end; ++j) {
    //             block_sum += lambdas[j];
    //         }
    
    //         // Normalize or zero the block
    //         if (block_sum != 0.0f) {
    //             for (std::size_t j = i; j < end; ++j) {
    //                 lambdas[j] /= block_sum;
    //             }
    //         } else {
    //             for (std::size_t j = i; j < end; ++j) {
    //                 lambdas[j] = 0.0f;
    //             }
    //         }
    //     }
    // }

 




    // for symmetric priors
void setAllMuToPrior() {
        std::fill(red.mu.begin(),  red.mu.end(),  red.prior[0]);
        std::fill(blue.mu.begin(), blue.mu.end(), blue.prior[0]);
}

void setRedMuToPrior() {
        std::fill(red.mu.begin(),  red.mu.end(),  red.prior[0]);
}

    //sets symmetricPriors
void setBluePrior(float newPrior) {
    std::fill(blue.prior.begin(), blue.prior.end(), newPrior);
}


void setRedPrior(float newPrior) {
    std::fill(red.prior.begin(), red.prior.end(), newPrior);
}




// Helper that prints any Die
static void printDie(const char* name, const Die& die, std::ostream& os = std::cout) {
    os << "=== " << name << " Die ===\n";
    os << "num_vars=" << die.num_vars << ", num_outcomes=" << die.num_outcomes << "\n";

    // choose your preferred formatting (optional)
    os << std::fixed << std::setprecision(6);

    os << "\nmu values (grouped by varId):\n";
    for (std::size_t varId = 0; varId < die.num_vars; ++varId) {
        os << " varId=" << varId << ": [";
        const std::size_t base = varId * die.num_outcomes;
        for (std::size_t outcomeId = 0; outcomeId < die.num_outcomes; ++outcomeId) {
            os << die.mu[base + outcomeId];
            if (outcomeId + 1 < die.num_outcomes) os << ", ";
        }
        os << "]\n";
    }

    os << "\nSum of mu for each varId:\n";
    for (std::size_t varId = 0; varId < die.num_vars; ++varId) {
        os << " sum_mu[" << varId << "] = " << die.sum_mu[varId] << "\n";
    }

    os << "\nDigamma(mu):\n";
    for (std::size_t varId = 0; varId < die.num_vars; ++varId) {
        os << " varId=" << varId << ": [";
        const std::size_t base = varId * die.num_outcomes;
        for (std::size_t outcomeId = 0; outcomeId < die.num_outcomes; ++outcomeId) {
            os << die.digamma_mu[base + outcomeId];
            if (outcomeId + 1 < die.num_outcomes) os << ", ";
        }
        os << "]\n";
    }

    os << "\nDigamma(sum_mu):\n";
    for (std::size_t varId = 0; varId < die.num_vars; ++varId) {
        os << " digamma_sum_mu[" << varId << "] = " << die.digamma_sum_mu[varId] << "\n";
    }

    os << "=== End of " << name << " Die ===\n\n";
}


void printRedBlue(std::ostream& os = std::cout) const {
    printDie("Red",  red,  os);
    printDie("Blue", blue, os);
}


};




#endif // INTERNAL_STATE_H

