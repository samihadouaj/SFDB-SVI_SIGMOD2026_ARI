#ifndef _Internal_State_H_
#define _Internal_State_H_
#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>
#include <memory>
#include "VIDie.h"
template<typename VIDice>
class Internal_State {
public:
    std::vector<float> lambdas;
    long long total_unique_nb_sat_assig; // this is the number of satisfying assignments withou repitition
    int nb_sat_assig_per_constraint;
    VIDice* vidice;
    Internal_State(long long m, int k, VIDice* vidice, bool init_lambda=1):
    total_unique_nb_sat_assig(m),
    nb_sat_assig_per_constraint(k),
    vidice(vidice)
    {
        // Initialize lambdas vector with zeros, size based on total_nb_sat_assig
        // lambdas = std::vector<float>(total_nb_sat_assig, 0.0);
        if(init_lambda)
        {
            lambdas.assign(total_unique_nb_sat_assig, 0.0);
        }
    }
    void setVIDice( VIDice* new_vidice)
    {
        vidice=new_vidice;
    }

    void printLambda()
    {
        for (const auto& lambda : lambdas) {
            std::cout << lambda << " ";
        }
        std::cout << std::endl;
        std::cout << "Total number of unique satisfied assignments: " << total_unique_nb_sat_assig << std::endl;
        std::cout << "Number of satisfied assignments per constraint: " << nb_sat_assig_per_constraint << std::endl;
    }

    //this will normalize lambda in groups of nb_sat_assig_per_constr
    void normalizeLambdas()
    {
    // Iterate over data in steps of K
    for (std::size_t i = 0; i < lambdas.size(); i += nb_sat_assig_per_constraint) {
        // 1) Compute sum of the block [i, i+K), clamped to lambdas.size()
        float blockSum = 0.0;
        std::size_t blockEnd = i+nb_sat_assig_per_constraint;
        for (std::size_t j = i; j < blockEnd; j++) {
            blockSum += lambdas[j];
        }

        // 2) Normalize that block
        if (blockSum != 0.0) {
            for (std::size_t j = i; j < blockEnd; j++) {
                lambdas[j] /= blockSum;
            }
        } 
        else
        {
            std::fill(lambdas.begin() + i, lambdas.begin() + blockEnd, 0.0);
        }

    }
    }


    void normalizeLambdasP(int num_threads) {
        const std::size_t K = nb_sat_assig_per_constraint;
        const std::size_t n = lambdas.size();
        const std::size_t num_blocks = (n + K - 1) / K;
    
        #pragma omp parallel for num_threads(num_threads)
        for (std::size_t block_id = 0; block_id < num_blocks; ++block_id) {
            const std::size_t i = block_id * K;
            const std::size_t end = i+K;
    
            // Compute the sum for the current block
            float block_sum = 0.0f;
            for (std::size_t j = i; j < end; ++j) {
                block_sum += lambdas[j];
            }
    
            // Normalize or zero the block
            if (block_sum != 0.0f) {
                for (std::size_t j = i; j < end; ++j) {
                    lambdas[j] /= block_sum;
                }
            } else {
                for (std::size_t j = i; j < end; ++j) {
                    lambdas[j] = 0.0f;
                }
            }
        }
    }

    void setLambdasToZero()
    {
     std::fill(lambdas.begin(), lambdas.end(), 0.0);
    }
    
    void setLambdasToZeroP(int num_threads) {
        const std::size_t size = lambdas.size();
    
        #pragma omp parallel for num_threads(num_threads)
        for (std::size_t i = 0; i < size; ++i) {
            lambdas[i] = 0.0f;
        }
    }
    

void prepare_lambda_for_eval(long long nb_uniq_sat_assign_in_batch)
{
    if (lambdas.size() == nb_uniq_sat_assign_in_batch) {
        std::fill(lambdas.begin(), lambdas.end(), 0.0f);  

    } else {
        lambdas.assign(nb_uniq_sat_assign_in_batch, 0.0f);
    }
}

    VIDice getVidice()
    {
        return vidice;
    }
};

#endif //Internal_State


// #ifndef _Internal_State_H_
// #define _Internal_State_H_

// #include <iostream>
// #include <cstdlib>
// #include <string>
// #include <memory>
// #include <algorithm>   // For std::fill
// #include <numeric>
// #include "VIDie.h"

// template<typename VIDice>
// class Internal_State {
// public:
//     float* lambdas;                // Raw pointer for lambda values
//     long long total_unique_nb_sat_assig; // Total number of satisfying assignments (without repetition)
//     int nb_sat_assig_per_constr;   // Number of satisfying assignments per constraint
//     VIDice* vidice;                // Pointer to the VIDice (e.g., tuple of VIDie objects)

//     // Constructor: allocate memory for lambdas and initialize them to 0.0f.
//     Internal_State(long long m, int k, VIDice* vidice)
//         : total_unique_nb_sat_assig(m),
//           nb_sat_assig_per_constr(k),
//           vidice(vidice)
//     {
//         lambdas = new float[total_unique_nb_sat_assig];
//         std::fill(lambdas, lambdas + total_unique_nb_sat_assig, 0.0f);
//     }

//     // Destructor: free the allocated memory for lambdas.
//     ~Internal_State() {
//         delete[] lambdas;
//     }

//     void setVIDice(VIDice* new_vidice)
//     {
//         vidice = new_vidice;
//     }

//     void printLambda()
//     {
//         for (int i = 0; i < total_unique_nb_sat_assig; ++i) {
//             std::cout << lambdas[i] << " ";
//         }
//         std::cout << std::endl;
//         std::cout << "Total number of unique satisfied assignments: " << total_unique_nb_sat_assig << std::endl;
//         std::cout << "Number of satisfied assignments per constraint: " << nb_sat_assig_per_constr << std::endl;
//     }

//     // Normalize lambdas in groups of nb_sat_assig_per_constr.
//     void normalizeLambdas()
//     {
//         for (std::size_t i = 0; i < static_cast<std::size_t>(total_unique_nb_sat_assig); i += nb_sat_assig_per_constr) {
//             float blockSum = 0.0f;
//             std::size_t blockEnd = i + nb_sat_assig_per_constr;
//             if(blockEnd > static_cast<std::size_t>(total_unique_nb_sat_assig)) {
//                 blockEnd = total_unique_nb_sat_assig;
//             }
//             for (std::size_t j = i; j < blockEnd; j++) {
//                 blockSum += lambdas[j];
//             }
//             if (blockSum != 0.0f) {
//                 for (std::size_t j = i; j < blockEnd; j++) {
//                     lambdas[j] /= blockSum;
//                 }
//             } else {
//                 std::fill(lambdas + i, lambdas + blockEnd, 0.0f);
//             }
//         }
//     }

//     // Set all lambda values to zero.
//     void setLambdasToZero()
//     {
//         std::fill(lambdas, lambdas + total_unique_nb_sat_assig, 0.0f);
//     }
    
//     // Returns the pointer to VIDice.
//     VIDice* getVidice()
//     {
//         return vidice;
//     }
// };

// #endif // _Internal_State_H_
