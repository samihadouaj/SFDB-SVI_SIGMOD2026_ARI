#ifndef _VIDIE_H_
#define _VIDIE_H_

#include <algorithm>   // std::fill_n
#include <tuple>
#include <random>      // for std::mt19937, std::gamma_distribution
#include <iostream>
#include <numeric>     // for std::accumulate
#include <cassert>
#include "ViUtils.h"   // for digamma
#include <cstring>

template<int NUMVARS, int NUMOUTCOMES>
class VIDie {
public:
    float* mu;
    float* sum_mu;
    float* digamma_mu;
    float* digamma_sum_mu;
    float* priorVect;
    
public:
    // Constructor: allocate arrays and initialize with default values.
    VIDie(){
        mu = new float[NUMVARS * NUMOUTCOMES];
        priorVect = new float[NUMVARS * NUMOUTCOMES];
        sum_mu = new float[NUMVARS];
        digamma_mu = new float[NUMVARS * NUMOUTCOMES];
        digamma_sum_mu = new float[NUMVARS];
        
        std::fill_n(mu, NUMVARS * NUMOUTCOMES, -1.0f);
        std::fill_n(sum_mu, NUMVARS, -1.0f);
        std::fill_n(digamma_mu, NUMVARS * NUMOUTCOMES, 0.0f);
        std::fill_n(digamma_sum_mu, NUMVARS, 0.0f);
    }
    
    // Destructor: free allocated memory.
    ~VIDie() {
        delete[] mu;
        delete[] sum_mu;
        delete[] digamma_mu;
        delete[] digamma_sum_mu;
    }
    
    // Disable copying (or implement the copy constructor/assignment operator as needed)
    VIDie(const VIDie&) = delete;
    VIDie& operator=(const VIDie&) = delete;
    
    // Public function to re-initialize mu with Gamma(shape, scale) distribution
    void initmuGamma(float shape = 100.0f, float scale = 0.01f, unsigned seed = 123) {
        std::mt19937 gen(seed); 
        std::gamma_distribution<float> gammaDist(shape, scale);
        for (int i = 0; i < NUMVARS * NUMOUTCOMES; i++) {
            mu[i] = gammaDist(gen);
        }
    }
    
    // void compute_sum_mu_and_digammaSumMu() {
    //     for (int varId = 0; varId < NUMVARS; ++varId) {
    //         float sum = 0.0f;
    //         for (int outcome = 0; outcome < NUMOUTCOMES; ++outcome) {
    //             sum += mu[varId * NUMOUTCOMES + outcome];
    //         }
    //         sum_mu[varId] = sum;
    //         digamma_sum_mu[varId] = digamma(sum);
    //     }
    // }
    
    // void compute_digamma_mu() {
    //     // #pragma omp parallel for numthreads(2)
    //     for (int i = 0; i < NUMVARS * NUMOUTCOMES; i++) {
    //         digamma_mu[i] = digamma(mu[i]);
    //     }
    // }
    
    // void prepare_die() {
    //     compute_digamma_mu();
    //     compute_sum_mu_and_digammaSumMu();
    // }
    

    
    void compute_digamma_mu() {
        for (int i = 0; i < NUMVARS * NUMOUTCOMES; i++) {
            __builtin_prefetch(&mu[i + 4]);
            digamma_mu[i] = digamma(mu[i]);
        }
    }
    
    void compute_sum_mu_and_digammaSumMu() {
        for (int varId = 0; varId < NUMVARS; ++varId) {
            float sum = 0.0f;
            for (int outcome = 0; outcome < NUMOUTCOMES; ++outcome) {
                sum += mu[varId * NUMOUTCOMES + outcome];
            }
            sum_mu[varId] = sum;
            digamma_sum_mu[varId] = digamma(sum);
        }
    }
    
    void prepare_die() {
        {
            compute_digamma_mu();
            compute_sum_mu_and_digammaSumMu();
        }
    }




    void compute_digamma_muP() {
        #pragma omp for schedule(static) 
        for (int i = 0; i < NUMVARS * NUMOUTCOMES; i++) {
            __builtin_prefetch(&mu[i + 4]);
            digamma_mu[i] = digamma(mu[i]);
        }
    }
    
    void compute_sum_mu_and_digammaSumMuP() {
        #pragma omp for 
        for (int varId = 0; varId < NUMVARS; ++varId) {
            float sum = 0.0f;
            for (int outcome = 0; outcome < NUMOUTCOMES; ++outcome) {
                sum += mu[varId * NUMOUTCOMES + outcome];
            }
            sum_mu[varId] = sum;
            digamma_sum_mu[varId] = digamma(sum);
        }
    }
    
    void prepare_dieP(int num_threads) {
        // std::cout<<"Running prepare_dieP"<<std::endl;
        #pragma omp parallel num_threads(num_threads)
        {
            compute_digamma_muP();
            compute_sum_mu_and_digammaSumMuP();
        }
    }

    

    void printVIDie(std::ostream& os = std::cout) const {
        os << "=== VIDie Contents ===\n";
        os << "mu values (grouped by varId):\n";
        for (int varId = 0; varId < NUMVARS; ++varId) {
            os << " varId=" << varId << ": [";
            for (int outcomeId = 0; outcomeId < NUMOUTCOMES; ++outcomeId) {
                os << mu[varId * NUMOUTCOMES + outcomeId];
                if (outcomeId < NUMOUTCOMES - 1) os << ", ";
            }
            os << "]\n";
        }
        os << "\nSum of mu for each varId:\n";
        for (int varId = 0; varId < NUMVARS; ++varId) {
            os << " sum_mu[" << varId << "] = " << sum_mu[varId] << "\n";
        }
        os << "\nDigamma(mu):\n";
        for (int varId = 0; varId < NUMVARS; ++varId) {
            os << " varId=" << varId << ": [";
            for (int outcomeId = 0; outcomeId < NUMOUTCOMES; ++outcomeId) {
                os << digamma_mu[varId * NUMOUTCOMES + outcomeId];
                if (outcomeId < NUMOUTCOMES - 1) os << ", ";
            }
            os << "]\n";
        }
        os << "\nDigamma(sum_mu):\n";
        for (int varId = 0; varId < NUMVARS; ++varId) {
            os << " digamma_sum_mu[" << varId << "] = " << digamma_sum_mu[varId] << "\n";
        }
        os << "=== End of VIDie ===\n";
    }
    
    //for Symmetric Priors
    void setAllToZero() {
        std::fill_n(mu, NUMVARS * NUMOUTCOMES, priorVect[0]);
        // memcpy(mu,priorVect, NUMVARS * NUMOUTCOMES * sizeof(float));
    }

    //for Symmetric Priors
    void setAllToZeroP(int num_threads) {
        const float prior_value = priorVect[0];
        const std::size_t size = NUMVARS * NUMOUTCOMES;
    
        #pragma omp parallel for num_threads(num_threads)
        for (std::size_t i = 0; i < size; ++i) {
            mu[i] = prior_value;
        }
    }
    //sets symmetricPriors
    void setPrior(float* newPrior,int vgid) {
        std::fill_n(priorVect, NUMVARS * NUMOUTCOMES, newPrior[vgid]);
    }







    // void setPriorAsymetric(float* newPrior) {
    //     std::memcpy(priorVect,newPrior, NUMVARS * NUMOUTCOMES, newPrior);
    // }

    // void setAllToZeroAsymetric() {
    //     memcpy(mu,priorVect, NUMVARS * NUMOUTCOMES * sizeof(float));
    // }
    
    
    int getNumVar() const {
        return NUMVARS;
    }
    
    int getNUMOUTCOMES() const {
        return NUMOUTCOMES;
    }
    
    // float getPrior() const {
    //     return prior;
    // }
};

template <typename... Args>
using VIDice = std::tuple<Args...>;

#endif  // _VIDIE_H_





// #ifndef _VIDIE_H_
// #define _VIDIE_H_

// #include <algorithm> // std::fill
// #include <vector>    // std::vector
// #include <tuple>
// #include <random> // for std::mt19937, std::gamma_distribution
// #include "ViUtils.h"
// #include <cassert>


// template<int NUMVARS, int NUMOUTCOMES>
// class VIDie{
// public:
// std::vector<float> mu;
// std::vector<float> sum_mu;
// std::vector<float> digamma_mu;
// std::vector<float> digamma_sum_mu;
// //  std::vector<float> e_logTheta;
// float prior;
// public:
//     VIDie( float prior =0.1):
//       mu(NUMVARS*NUMOUTCOMES, -1.0),
//       sum_mu(NUMVARS, -1.0),
//       digamma_mu(NUMVARS*NUMOUTCOMES, 0.0),
//       digamma_sum_mu(NUMVARS, 0.0),
//     //   e_logTheta(NUMVARS*NUMOUTCOMES,-1.0),
//       prior(prior)
//    {}



//     // Public function to re-initialize mu with Gamma(shape, scale) distribution
//     void initmuGamma(float shape = 100.0, float scale = 0.01, unsigned seed=123)
//     {
//         std::mt19937 gen(seed); 
//         std::gamma_distribution<float> gammaDist(shape, scale);

//         // Fill mu with gamma-distributed values
//         for (auto& val : mu) {
//             val = gammaDist(gen);
//         }
//     }


// void compute_sum_mu_and_digammaSumMu() {
//     for (int varId = 0; varId < NUMVARS; ++varId) {
//         const float* start = &mu[varId * NUMOUTCOMES];
//         const float* end = start + NUMOUTCOMES;
//         sum_mu[varId] = std::accumulate(start, end, 0.0);
//         digamma_sum_mu[varId]=digamma(sum_mu[varId]);
//     }
// }

// void prepare_die()
// {
//     compute_digamma_mu();
//     compute_sum_mu_and_digammaSumMu();
// }

// void printVIDie(std::ostream& os = std::cout) const
// {
//     os << "=== VIDie Contents ===\n";

//     // Print mu
//     os << "mu values (grouped by varId):\n";
//     for (int varId = 0; varId < NUMVARS; ++varId) {
//         os << " varId=" << varId << ": [";
//         for (int outcomeId = 0; outcomeId < NUMOUTCOMES; ++outcomeId) {
//             os << mu[varId * NUMOUTCOMES + outcomeId];
//             if (outcomeId < NUMOUTCOMES - 1) os << ", ";
//         }
//         os << "]\n";
//     }

//     // Print sum_mu
//     os << "\nSum of mu for each varId:\n";
//     for (int varId = 0; varId < NUMVARS; ++varId) {
//         os << " sum_mu[" << varId << "] = " << sum_mu[varId] << "\n";
//     }

//     // Print digamma_mu
//     os << "\nDigamma(mu):\n";
//     for (int varId = 0; varId < NUMVARS; ++varId) {
//         os << " varId=" << varId << ": [";
//         for (int outcomeId = 0; outcomeId < NUMOUTCOMES; ++outcomeId) {
//             os << digamma_mu[varId * NUMOUTCOMES + outcomeId];
//             if (outcomeId < NUMOUTCOMES - 1) os << ", ";
//         }
//         os << "]\n";
//     }

//     // Print digamma_sum_mu
//     os << "\nDigamma(sum_mu):\n";
//     for (int varId = 0; varId < NUMVARS; ++varId) {
//         os << " digamma_sum_mu[" << varId << "] = " 
//             << digamma_sum_mu[varId] << "\n";
//     }

//     os << "=== End of VIDie ===\n";
// }

// void compute_digamma_mu()
// {
//     #pragma omp parallel for numthreads(8)
//     for (int val = 0; val <NUMVARS*NUMOUTCOMES; val++)
//     {
//         // assert(mu[val]>0);
//         digamma_mu[val]=digamma(mu[val]);
//     }
// }

// void setAllToZero()
// {
//     std::fill(mu.begin(),mu.end(),prior);
//     // std::fill(e_logTheta.begin(), e_logTheta.end(), -1.0);
// }


// // void set_e_logThetaToZero()
// // {
// //     std::fill(e_logTheta.begin(), e_logTheta.end(), -1.0);

// // }
// void setPrior(float prior)
// {
//     prior = prior;
// }

//     int getNumVar () const{
//         return NUMVARS;
//     }

//        int getNUMOUTCOMES() const{
//         return NUMOUTCOMES;
//     }

//         float getPrior () const{ 
//         return prior;
//     }
    




// };


// template <typename... Args>
// using VIDice = std::tuple<Args...>;
// #endif  // _VIDIE_H_

