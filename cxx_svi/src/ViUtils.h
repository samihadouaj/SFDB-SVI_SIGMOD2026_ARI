#ifndef _VIUTILS2_H_
#define _VIUTILS2_H_

#include <iostream>
#include <vector>
#include <random>
#include <numeric>   // for std::accumulate
#include <algorithm> // For std::max_element
#include <cassert>
#include <tuple>
#include <utility>
#include <cmath>
#include <cstdint>
#include <cstring>
#include "omp.h"
using namespace std;
inline float digamma(float x)
{
    x += 6.0f; // Shift to improve approximation
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

inline void getTokenTopic(std::vector<int> &result, std::vector<float> &lambdas, int K)
{
    // std::vector<int> result;
    // Process the filtered lambdas in blocks of K
    size_t sizeLambda = lambdas.size();
    // result.reserve(sizeLambda / K + 1); // +1 for partial final block

    // std::cout<<"inside getTokenTopic size of Lambda_eval = "<<sizeLambda<<std::endl;
    for (std::size_t i = 0; i < sizeLambda; i += K)
    {
        // Determine the end of this block (could be smaller than K for the last block)
        std::size_t blockEnd = std::min(i + K, static_cast<size_t>(sizeLambda));

        // Find the argmax within this block
        float maxVal = lambdas[i];
        int argMax = 0; // relative index within this block
        for (std::size_t j = i + 1; j < blockEnd; j++)
        {
            if (lambdas[j] > maxVal)
            {
                maxVal = lambdas[j];
                argMax = j - i; // relative index within the block
            }
        }
        // Add the local argmax index to the result
        result[i/K]=argMax;
    }

    // return result;
};

inline std::vector<float> precompute_rhot(size_t numIters)
{
    std::vector<float> rhot_values(numIters);

    for (size_t it = 0; it < numIters; ++it)
    {
        // rhot_values[it] = 16/(pow(1024+it,0.7));
        rhot_values[it] = 1.0 / (pow(1 + it, 0.7));
        // std::cout << "rhot_values[it] = " << rhot_values[it] << std::endl;
    }

    return rhot_values;
}

\


inline void normalizeLambdasRange(std::vector<float>& lambdas,
                                  int K,
                                  std::size_t start,
                                  std::size_t end)
{

    for (std::size_t r = start; r < end; ++r) {
        const std::size_t base = r * static_cast<std::size_t>(K);

        float sum = 0.0f;
        for (int t = 0; t < K; ++t) sum += lambdas[base + t];

        if (sum != 0.0f) {
            const float inv = 1.0f / sum;
            for (int t = 0; t < K; ++t) lambdas[base + t] *= inv;
        } else {
            for (int t = 0; t < K; ++t) lambdas[base + t] = 0.0f;
        }
    }
}


// inline void normalizeLambdasRangeP(std::vector<float>& lambdas,
//                                   int K,
//                                   std::size_t start,
//                                   std::size_t end,
//                                   int numThreads)
// {

//     #pragma omp parallel for num_threads(numThreads)
//     for (std::size_t r = start; r < end; ++r) {
//         const std::size_t base = r * static_cast<std::size_t>(K);

//         float sum = 0.0f;
//         for (int t = 0; t < K; ++t) sum += lambdas[base + t];

//         if (sum != 0.0f) {
//             const float inv = 1.0f / sum;
//             for (int t = 0; t < K; ++t) lambdas[base + t] *= inv;
//         } else {
//             for (int t = 0; t < K; ++t) lambdas[base + t] = 0.0f;
//         }
//     }
// }





// Copyright 2021 Johan Rade (johan.rade@gmail.com)
// Distributed under the MIT license (https://opensource.org/licenses/MIT)
// Code for fastExp extracted from https://gist.github.com/jrade/293a73f89dfef51da6522428c857802d
inline float fastExp(float x)
{
    constexpr float a = (1 << 23) / 0.69314718f;
    constexpr float b = (1 << 23) * (127 - 0.043677448f);
    x = a * x + b;

    // Remove these lines if bounds checking is not needed
    // constexpr float c = (1 << 23);
    // constexpr float d = (1 << 23) * 255;
    // if (x < c || x > d)
    //     x = (x < c) ? 0.0f : d;

    // With C++20 one can use std::bit_cast instead
    uint32_t n = static_cast<uint32_t>(x);
    memcpy(&x, &n, 4);
    return x;
}





#endif // _VIUTILS2_H_