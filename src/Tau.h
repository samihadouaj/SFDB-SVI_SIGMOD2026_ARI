#ifndef _TAU_H_
#define _TAU_H_
#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>
#include <memory>
#include <cmath>
#include <cstdint>
#include <cstring>

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




template <typename... Cs>
class Tau {
public:
    // static void print(std::vector<int> data) {
    //     std::cout << "Hello this is Tau with " << sizeof...(Cs) << " components" << std::endl;
    //     // Call print() for all components
    //     (Cs::print(data), ...);
    // }

    // // Set priors for all components (one prior per component)
    // template<typename M, typename... PriorTs>
    // static void setPriors(Internal_State<M>& is, PriorTs... priors) {
    //     static_assert(sizeof...(Cs) == sizeof...(priors), 
    //                  "Number of priors must match number of components");
    //     // Set prior for each component
    //     (Cs::setPrior(is, priors), ...);
    // }

    // Compute local parameters
    template<typename M>
    static inline void computeLocalParams(Internal_State<M>& is, int* context, long long recordId, size_t current_sat_index) {
        // Sum eLogTheta from all components
        // const float sum_eLogTheta = (0.0f + ... + Cs::compute_e_log_theta_atom(is, context));
        float logThetas[] = {Cs::compute_e_log_theta_atom(is, context)...}; 
        float sum_eLogTheta = 0.0f;
        for (float val : logThetas) {
            sum_eLogTheta += val;
        }
        is.lambdas[recordId+current_sat_index] = fastExp(sum_eLogTheta);
    }

    // Update global parameters 
    template<typename M>
    static inline void updateGlobalParams(Internal_State<M>& is, int* context, long long recordId,  size_t current_sat_index) {
        // Update each component
        (Cs::updateGlobalParamsAtom(is, context, recordId+current_sat_index), ...);
    }
};

template <typename... Taus>
struct ForEachTau {
    template <typename M>
    static void computeLocalParams(Internal_State<M>& is, int* context, long long recordId) {
        computeLocalParamsImpl(is, context, recordId,std::make_index_sequence<sizeof...(Taus)>{});
    }

    template <typename M, std::size_t... Is>
    static void computeLocalParamsImpl(Internal_State<M>& is,
                                      int* context,
                                      long long recordId,
                                      std::index_sequence<Is...>) {
        // Pass individual indices to each Tau
        (Taus::computeLocalParams(is, context, recordId, Is), ...);
    }

    template <typename M>
    static void updateGlobalParams(Internal_State<M>& is, int* context, long long recordId) {
        updateGlobalParamsImpl(is, context, recordId,std::make_index_sequence<sizeof... (Taus)>{});
    }

    template<typename M, std::size_t... Is>
    static void updateGlobalParamsImpl(Internal_State<M>& is,
                                      int* context,
                                      long long recordId,
                                      std::index_sequence<Is...>) {
        // Pass individual indices to each Tau
        (Taus::updateGlobalParams(is, context, recordId, Is), ...);
    }

    // using FirstTau = typename std::tuple_element<0, std::tuple<Taus...>>::type;

    // // Call setPriors on the first Tau in the tuple
    // template <typename M, typename... PriorTs>
    // static void setPriors(Internal_State<M>& is, PriorTs... priors) {
    //     // Delegate to the first Tau's setPriors
    //     FirstTau::setPriors(is, priors...);
    // }
};

// Specialization for tuples
template <typename Tuple>
struct SAT;

template <typename... Ts>
struct SAT<std::tuple<Ts...>> : ForEachTau<Ts...> {};

#endif // _TAU_H_


// template <class C1>
// class Tau1 {
// public:

//     template<typename M>
//     static void setPriors(Internal_State<M>& is, float p1)
//     {
//         C1::setPrior(is, p1);
//     }

//     template<typename M>
//     static inline void computeLocalParams(Internal_State<M>& is, int* context, long long recordId)
//     {
//         const float eLogTheta1 = C1::compute_e_log_theta_atom(is, context);
//         is.lambdas[recordId] = fastExp(eLogTheta1);
//     }

//     template<typename M>
//     static inline void updateGlobalParams(Internal_State<M>& is, int* context, long long recordId)
//     {
//         C1::updateGlobalParamsAtom(is, context, recordId);
//     }
// };


// template <class C1, class C2>
// class Tau2 {
//         public:
//         static void print(std::vector<int> data)
//         {
//             std::cout << "Hello this is Tau2" << std::endl;
//             C1::print(data);
//             C2::print(data);
//         }
//        template<typename M>
//         static void setPriors(Internal_State<M>& is,float p1,float p2)
//         {
//             C1::setPrior(is,p1);
//             C2::setPrior(is,p2);
//         }

//         template<typename M>
//         static inline void computeLocalParams(Internal_State<M>& is, int* context, long long recordId)
//          {
//             const float eLogTheta1 = C1::compute_e_log_theta_atom(is, context);
//             const float eLogTheta2 = C2::compute_e_log_theta_atom(is, context);
//             is.lambdas[recordId] = fastExp(eLogTheta1 + eLogTheta2);
//         }

//         template<typename M>
//         static inline void updateGlobalParams(Internal_State<M>& is, int* context,long long recordId)
//         {
//             C1:: updateGlobalParamsAtom(is, context, recordId);
//             C2:: updateGlobalParamsAtom(is, context, recordId);
//         }
    
// };


// template <class C1, class C2, class C3>
// class Tau3 {
// public:
//     template<typename M>
//     static void setPriors(Internal_State<M>& is, float p1, float p2, float p3)
//     {
//         C1::setPrior(is, p1);
//         C2::setPrior(is, p2);
//         C3::setPrior(is, p3);
//     }

//     template<typename M>
//     static inline void computeLocalParams(Internal_State<M>& is, int* context, long long recordId)
//     {
//         const float eLogTheta1 = C1::compute_e_log_theta_atom(is, context);
//         const float eLogTheta2 = C2::compute_e_log_theta_atom(is, context);
//         const float eLogTheta3 = C3::compute_e_log_theta_atom(is, context);
//         is.lambdas[recordId] = fastExp(eLogTheta1 + eLogTheta2 + eLogTheta3);
//     }

//     template<typename M>
//     static inline void updateGlobalParams(Internal_State<M>& is, int* context, long long recordId)
//     {
//         C1::updateGlobalParamsAtom(is, context, recordId);
//         C2::updateGlobalParamsAtom(is, context, recordId);
//         C3::updateGlobalParamsAtom(is, context, recordId);
//     }
// };

// template <class C1, class C2, class C3, class C4>
// class Tau4 {
// public:

//     template<typename M>
//     static void setPriors(Internal_State<M>& is, float p1, float p2, float p3, float p4)
//     {
//         C1::setPrior(is, p1);
//         C2::setPrior(is, p2);
//         C3::setPrior(is, p3);
//         C4::setPrior(is, p4);
//     }

//     template<typename M>
//     static inline void computeLocalParams(Internal_State<M>& is, int* context, long long recordId)
//     {
//         const float eLogTheta1 = C1::compute_e_log_theta_atom(is, context);
//         const float eLogTheta2 = C2::compute_e_log_theta_atom(is, context);
//         const float eLogTheta3 = C3::compute_e_log_theta_atom(is, context);
//         const float eLogTheta4 = C4::compute_e_log_theta_atom(is, context);
//         is.lambdas[recordId] = fastExp(eLogTheta1 + eLogTheta2 + eLogTheta3 + eLogTheta4);
//     }

//     template<typename M>
//     static inline void updateGlobalParams(Internal_State<M>& is, int* context, long long recordId)
//     {
//         C1::updateGlobalParamsAtom(is, context, recordId);
//         C2::updateGlobalParamsAtom(is, context, recordId);
//         C3::updateGlobalParamsAtom(is, context, recordId);
//         C4::updateGlobalParamsAtom(is, context, recordId);
//     }
// };

// template <class C1, class C2, class C3, class C4, class C5>
// class Tau5 {
// public:


//     template<typename M>
//     static void setPriors(Internal_State<M>& is, float p1, float p2, float p3, float p4, float p5)
//     {
//         C1::setPrior(is, p1);
//         C2::setPrior(is, p2);
//         C3::setPrior(is, p3);
//         C4::setPrior(is, p4);
//         C5::setPrior(is, p5);
//     }

//     template<typename M>
//     static inline void computeLocalParams(Internal_State<M>& is, int* context, long long recordId)
//     {
//         const float eLogTheta1 = C1::compute_e_log_theta_atom(is, context);
//         const float eLogTheta2 = C2::compute_e_log_theta_atom(is, context);
//         const float eLogTheta3 = C3::compute_e_log_theta_atom(is, context);
//         const float eLogTheta4 = C4::compute_e_log_theta_atom(is, context);
//         const float eLogTheta5 = C5::compute_e_log_theta_atom(is, context);
//         is.lambdas[recordId] = fastExp(eLogTheta1 + eLogTheta2 + eLogTheta3 + eLogTheta4 + eLogTheta5);
//     }

//     template<typename M>
//     static inline void updateGlobalParams(Internal_State<M>& is, int* context, long long recordId)
//     {
//         C1::updateGlobalParamsAtom(is, context, recordId);
//         C2::updateGlobalParamsAtom(is, context, recordId);
//         C3::updateGlobalParamsAtom(is, context, recordId);
//         C4::updateGlobalParamsAtom(is, context, recordId);
//         C5::updateGlobalParamsAtom(is, context, recordId);
//     }
// };






