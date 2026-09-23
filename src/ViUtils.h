#ifndef _VIUTILS2_H_
#define _VIUTILS2_H_


#include <iostream>
#include <vector>
#include <random>
#include <numeric> // for std::accumulate
#include <algorithm>  // For std::max_element
#include <cassert>
#include "VIDie.h"
#include <tuple>
#include <utility>

// inline float digamma(float x) {
//     x += 6;  // Shift to improve approximation
//     float p = 1.0 / (x * x);

//     // Use pre-computed constants for better performance
//     p = (((0.004166666666667 * p - 0.003968253986254) * p +
//           0.008333333333333) * p - 0.083333333333333) * p;
//     p += std::log(x) - 0.5 / x;

//     // Unroll the harmonic series terms for better performance
//     p -= (1 / (x - 1) + 1 / (x - 2) + 1 / (x - 3) +
//           1 / (x - 4) + 1 / (x - 5) + 1 / (x - 6));
//     return p;
// };


inline float digamma(float x) {
    x += 6.0f;  // Shift to improve approximation
    const float p = 1.0f / (x * x);

    // Optimized polynomial using Horner's method with float constants
    const float approximation = ((-0.003968253986254f * p + 0.008333333333333f) * p - 0.083333333333333f) * p;
    float result = std::log(x) - 0.5f / x + approximation;

    // Subtract the harmonic terms (unrolled for performance)
    result -= 1.0f / (x - 1.0f) + 1.0f / (x - 2.0f) +
              1.0f / (x - 3.0f) + 1.0f / (x - 4.0f) +
              1.0f / (x - 5.0f) + 1.0f / (x - 6.0f);
    return result;
}

// inline float digamma(float x) noexcept {
//     // Valid for x >= 6 (ensure this holds in your parameterization)
//     const float inv_x = 1.0f / x;
//     return std::log(x) - 0.5f * inv_x - 0.08333333333f * inv_x * inv_x;
// }


// inline float digamma(float x)  {
//     // Piecewise approximation for x ≥ 6
//     const float a = -0.5f / x;
//     const float b = 1.12f / (x + 1.3f);
//     return std::log(x) + a + b;
// }  // 3x faster, <0.1% error



void getTokenTopic( std::vector<int>& result, std::vector<float>& lambdas, int K) {
    // std::vector<int> result;
    // Process the filtered lambdas in blocks of K
    size_t sizeLambda = lambdas.size();
    // result.reserve(sizeLambda / K + 1); // +1 for partial final block

    // std::cout<<"inside getTokenTopic size of Lambda_eval = "<<sizeLambda<<std::endl;
    for (std::size_t i = 0; i < sizeLambda; i += K) {
        // Determine the end of this block (could be smaller than K for the last block)
        std::size_t blockEnd = std::min(i + K, static_cast<size_t>(sizeLambda));

        // Find the argmax within this block
        float maxVal = lambdas[i];
        int argMax = 0; // relative index within this block
        for (std::size_t j = i + 1; j < blockEnd; j++) {
            if (lambdas[j] > maxVal) {
                maxVal = lambdas[j];
                argMax = j - i; // relative index within the block
            }
        }
        // Add the local argmax index to the result
        result.push_back(argMax);
    }

    // return result;
};

std::vector<float> precompute_rhot(size_t numIters) {
    std::vector<float> rhot_values(numIters);
    
    for (size_t it = 0; it < numIters; ++it) {
        // rhot_values[it] = 16/(pow(1024+it,0.7)); 
        rhot_values[it] = 1.0/(pow(1+it,0.7)); 
        std::cout<<"rhot_values[it] = "<<rhot_values[it]<<std::endl;
    }

    return rhot_values;
}


// std::vector<float> precompute_rhot(size_t numIters) {
//     std::vector<float> rhot_values(numIters);
    
//     for (size_t it = 0; it < numIters; ++it) {
//         rhot_values[it] = 16/(pow(64+it,0.7)); 
//         std::cout<<"rhot_values[it] = "<<rhot_values[it]<<std::endl;
//     }

//     return rhot_values;
// };



#endif  // _VIUTILS2_H_