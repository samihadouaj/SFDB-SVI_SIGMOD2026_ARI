#ifndef _SVI_TEMPLATE_H
#define _SVI_TEMPLATE_H
#include "Internal_State.h"
#include <algorithm> // For std::copy



#include <tuple>
#include <utility> // for std::index_sequence, std::make_index_sequence


// Helper function to deep copy a raw float array.
static void deep_copy_mu(float* dst_mu, const float* src_mu, int numElements) {
    std::copy(src_mu, src_mu + numElements, dst_mu);
}

template<typename M>
inline static void copy_mu_for_die(M& dst_die, const M& src_die)
{
    // Calculate the total number of elements in the mu array.
    int numElements = src_die.getNumVar() * src_die.getNUMOUTCOMES();
    deep_copy_mu(dst_die.mu, src_die.mu, numElements);
}


template<int NUMDIE>
class Svi_template
{
public:
    // Initialize mu via Gamma for all dice.
    template<typename M>
    inline static void initmuGamma(Internal_State<M>& is)
    {
        initmuGamma_impl(is, std::make_index_sequence<NUMDIE>{});
    }

       //set the symmetric priors
       template<typename M>
       inline static void setPriors(Internal_State<M>& is, float* priors)
       {
           setPriors_impl(is, priors, std::make_index_sequence<NUMDIE>{});
       }
   
    // Copy the mu values for all dice.
    template<typename M>
    inline static void copy_mus(Internal_State<M>& is, Internal_State<M>& is_batch)
    {
        copy_mus_impl(is, is_batch, std::make_index_sequence<NUMDIE>{});
    }

    // Prepare all dice (compute digamma values, sums, etc.).
    template<typename M>
    inline static void prepare_dice(Internal_State<M>& is)
    {
        prepare_dice_impl(is, std::make_index_sequence<NUMDIE>{});
    }



    template<typename M>
    inline static void prepare_diceP(Internal_State<M>& is,int num_threads)
    {
        prepare_diceP_impl(is, num_threads,std::make_index_sequence<NUMDIE>{});
    }


    // Set all values in all dice to zero (or to the prior). Symmetric Priors
    template<typename M>
    inline static void setAllToZero(Internal_State<M>& is)
    {
        setAllToZero_impl(is, std::make_index_sequence<NUMDIE>{});
    }
    // Symmetric Priors
    template<typename M>
    inline static void setAllToZeroP(Internal_State<M>& is,int num_threads)
    {
        setAllToZeroP_impl(is, std::make_index_sequence<NUMDIE>{},num_threads);
    }

    // Merge internal states for all dice.
    template<typename M>
    static void merge_internal_states(Internal_State<M>& is, Internal_State<M>& is_batch,
                                        long long total_unique_nb_sat_assig, long long nb_sat_assign_in_batch,
                                        float rhot,int num_threads)
    {
        merge_internal_states_impl(is, is_batch, total_unique_nb_sat_assig, nb_sat_assign_in_batch, rhot,
                                   std::make_index_sequence<NUMDIE>{},num_threads);
    }

private:
    // Helper functions using index sequences for compile-time iteration.

    template<typename M, std::size_t... Is>
    inline static void initmuGamma_impl(Internal_State<M>& is, std::index_sequence<Is...>)
    {
        (std::get<Is>(*(is.vidice)).initmuGamma(), ...);
    }


    template<typename M, std::size_t... Is>
    inline static void setPriors_impl(Internal_State<M>& is, float* priors, std::index_sequence<Is...>)
    {
        (std::get<Is>(*(is.vidice)).setPrior(priors,static_cast<int>(Is)), ...);
    }

    template<typename M, std::size_t... Is>
    inline static void copy_mus_impl(Internal_State<M>& is, Internal_State<M>& is_batch, std::index_sequence<Is...>)
    {
        (copy_mu_for_die(std::get<Is>(*(is_batch.vidice)), std::get<Is>(*(is.vidice))), ...);
    }

    template<typename M, std::size_t... Is>
    inline static void prepare_dice_impl(Internal_State<M>& is, std::index_sequence<Is...>)
    {
        (std::get<Is>(*(is.vidice)).prepare_die(), ...);
    }



    template<typename M, std::size_t... Is>
    inline static void prepare_diceP_impl(Internal_State<M>& is,int num_threads, std::index_sequence<Is...>)
    {
        (std::get<Is>(*(is.vidice)).prepare_dieP(num_threads), ...);
    }


    template<typename M, std::size_t... Is>
    inline static void setAllToZero_impl(Internal_State<M>& is, std::index_sequence<Is...>)
    {
        (std::get<Is>(*(is.vidice)).setAllToZero(), ...);
    }

    template<typename M, std::size_t... Is>
    inline static void setAllToZeroP_impl(Internal_State<M>& is, std::index_sequence<Is...>,int num_threads)
    {
        (std::get<Is>(*(is.vidice)).setAllToZeroP(num_threads), ...);
    }

    template<typename Die>
    inline static void merge_die(Die& die, Die& die_batch, float one_minus_rhot, float rhot, float scaling_factor)
    {
        for (int i = 0; i < die.getNumVar() * die.getNUMOUTCOMES(); ++i) {
            die.mu[i] = one_minus_rhot * die.mu[i] +
                         rhot * (0.1 + scaling_factor * die_batch.mu[i]);
        }
    }

    template<typename Die>
    inline static void merge_dieP(Die& die, Die& die_batch, float one_minus_rhot, float rhot, float scaling_factor,int num_threads)
    {   
        #pragma omp parallel for num_threads(num_threads)
        for (int i = 0; i < die.getNumVar() * die.getNUMOUTCOMES(); ++i) {
            die.mu[i] = one_minus_rhot * die.mu[i] +
                         rhot * (0.1 + scaling_factor * die_batch.mu[i]);
        }
    }

    template<typename M, std::size_t... Is>
    static void merge_internal_states_impl(Internal_State<M>& is, Internal_State<M>& is_batch,
                                           long long total_unique_nb_sat_assig, long long nb_sat_assign_in_batch,
                                           float rhot, std::index_sequence<Is...>, int num_threads)
    {

        M& vidice = *is.vidice;
        M& vidice_batch = *is_batch.vidice;

        float one_minus_rhot = 1 - rhot;
        float scaling_factor = static_cast<float>(total_unique_nb_sat_assig) / nb_sat_assign_in_batch;
        std::cout << "scaling factor = " << scaling_factor << std::endl;

        // Merge each die using fold expression
        if(num_threads==1)
        (merge_die(std::get<Is>(vidice), std::get<Is>(vidice_batch), one_minus_rhot, rhot, scaling_factor), ...);
        else
        (merge_dieP(std::get<Is>(vidice), std::get<Is>(vidice_batch), one_minus_rhot, rhot, scaling_factor,num_threads), ...);

    }
};
#endif //_SVI_TEMPLATE_H




// class svi1_template
// {
// public:
//     // Initialize mu via Gamma for the single die.
//     template<typename M>
//     inline static void initmuGamma(Internal_State<M>& is)
//     {
//         std::get<0>(*(is.vidice)).initmuGamma();
//     }

//     // Copy the mu values from one internal state to another for the single die.
//     template<typename M>
//     inline static void copy_mus(Internal_State<M>& is, Internal_State<M>& is_batch)
//     {
//         auto& src_die = std::get<0>(*(is.vidice));
//         auto& dst_die = std::get<0>(*(is_batch.vidice));
//         copy_mu_for_die(dst_die, src_die);
//     }

//     // Prepare (compute digamma values, sums, etc.) for the single die.
//     template<typename M>
//     inline static void prepare_dice(Internal_State<M>& is)
//     {
//         std::get<0>(*(is.vidice)).prepare_die();
//     }

//     // Set all values in the die to zero (or to the prior, as defined).
//     template<typename M>
//     inline static void setAllToZero(Internal_State<M>& is)
//     {
//         std::get<0>(*(is.vidice)).setAllToZero();
//     }

//     // Merge internal states for the single die using the given parameters.
//     template<typename M>
//     static void merge_internal_states(Internal_State<M>& is, Internal_State<M>& is_batch,
//                                         long long total_unique_nb_sat_assig, long long nb_sat_assign_in_batch,
//                                         float rhot)
//     {
//         auto merge_start = std::chrono::high_resolution_clock::now();

//         // Get the tuple of dice from both internal states.
//         M& vidice = *is.vidice;
//         M& vidice_batch = *is_batch.vidice;

//         float one_minus_rhot = 1 - rhot;
//         float scaling_factor = static_cast<float>(total_unique_nb_sat_assig) / nb_sat_assign_in_batch;
//         std::cout << "scaling factor = " << scaling_factor << std::endl;

//         // Merge the single die
//         auto& die = std::get<0>(vidice);
//         auto& die_batch = std::get<0>(vidice_batch);
//         for (int i = 0; i < die.getNumVar() * die.getNUMOUTCOMES(); i++) {
//             die.mu[i] = one_minus_rhot * die.mu[i] +
//                         rhot * (die.prior + scaling_factor * die_batch.mu[i]);
//         }

//         auto merge_end = std::chrono::high_resolution_clock::now();
//         long merge_timeMs = std::chrono::duration_cast<std::chrono::milliseconds>(merge_end - merge_start).count();
//         std::cout << "Merge execution time: " << merge_timeMs << " ms" << std::endl;
//     }
// };

// class Svi2_template
// {
//     public:
//         template<typename M>
//         inline static void initmuGamma(Internal_State<M> &is)
//         {
//             std::get<0>(*(is.vidice)).initmuGamma();
//             std::get<1>(*(is.vidice)).initmuGamma();
//         }


//         template<typename M>
//         inline static void copy_mus(Internal_State<M>& is, Internal_State<M>& is_batch)
//         {
//             auto& src_die1 = std::get<0>(*(is.vidice));
//             auto& dst_die1 = std::get<0>(*(is_batch.vidice));
//             copy_mu_for_die(dst_die1, src_die1);    

//             auto& src_die2 = std::get<1>(*(is.vidice));
//             auto& dst_die2 = std::get<1>(*(is_batch.vidice));
//             copy_mu_for_die(dst_die2, src_die2);    
//         }

    

//         template<typename M>
//         inline static void prepare_dice(Internal_State<M> &is)
//         {
//             std::get<0>(*(is.vidice)).prepare_die();
//             std::get<1>(*(is.vidice)).prepare_die();
//         }


//         template<typename M>
//         inline static void setAllToZero(Internal_State<M> &is)
//         {
//             std::get<0>(*(is.vidice)).setAllToZero();
//             std::get<1>(*(is.vidice)).setAllToZero();
         
//         }


//         template<typename M>
//         static void  merge_internal_states(Internal_State<M> &is, Internal_State<M> &is_batch, long long total_unique_nb_sat_assig,long long nb_sat_assign_in_batch, float rhot)
//         {
//         auto merge_start = std::chrono::high_resolution_clock::now();
        
//         // Merging the first die
//         M& vidice = *is.vidice;
//         M& vidice_batch = *is_batch.vidice;
        
//         auto& die1 = std::get<0>(vidice);
//         auto& die1_batch = std::get<0>(vidice_batch);
        
//         float one_minus_rhot = 1 - rhot;
//         float scaling_factor = static_cast<float>(total_unique_nb_sat_assig) / nb_sat_assign_in_batch;
//         std::cout << "scaling factor = " << scaling_factor << std::endl;
        
//         for (int i = 0; i < die1.getNumVar()*die1.getNUMOUTCOMES(); i++) {
//            die1.mu[i] = one_minus_rhot * die1.mu[i] + rhot * (die1.prior + scaling_factor * die1_batch.mu[i]);
//         }
        

//         // Merging the second die
//         auto& die2 = std::get<1>(vidice);
//         auto& die2_batch = std::get<1>(vidice_batch);
//         for (int i = 0; i < die2.getNumVar()*die2.getNUMOUTCOMES(); i++) {
//            die2.mu[i] = one_minus_rhot * die2.mu[i] + rhot * (die2.prior + scaling_factor * die2_batch.mu[i]);
//         }

//         auto merge_end = std::chrono::high_resolution_clock::now();
//         long merge_timeMs = std::chrono::duration_cast<std::chrono::milliseconds>(merge_end - merge_start).count();
//         std::cout << "Merge execution time: " << merge_timeMs << " ms" << std::endl;
//         };
        
// };


// class svi3_template
// {
// public:
//     // Initialize mu via Gamma for all three dice.
//     template<typename M>
//     inline static void initmuGamma(Internal_State<M>& is)
//     {
//         std::get<0>(*(is.vidice)).initmuGamma();
//         std::get<1>(*(is.vidice)).initmuGamma();
//         std::get<2>(*(is.vidice)).initmuGamma();
//     }

//     // Copy the mu values from one internal state to another for all three dice.
//     template<typename M>
//     inline static void copy_mus(Internal_State<M>& is, Internal_State<M>& is_batch)
//     {
//         auto& src_die1 = std::get<0>(*(is.vidice));
//         auto& dst_die1 = std::get<0>(*(is_batch.vidice));
//         copy_mu_for_die(dst_die1, src_die1);

//         auto& src_die2 = std::get<1>(*(is.vidice));
//         auto& dst_die2 = std::get<1>(*(is_batch.vidice));
//         copy_mu_for_die(dst_die2, src_die2);

//         auto& src_die3 = std::get<2>(*(is.vidice));
//         auto& dst_die3 = std::get<2>(*(is_batch.vidice));
//         copy_mu_for_die(dst_die3, src_die3);
//     }

//     // Prepare (compute digamma values, sums, etc.) for all three dice.
//     template<typename M>
//     inline static void prepare_dice(Internal_State<M>& is)
//     {
//         std::get<0>(*(is.vidice)).prepare_die();
//         std::get<1>(*(is.vidice)).prepare_die();
//         std::get<2>(*(is.vidice)).prepare_die();
//     }

//     // Set all values in each die to zero (or to prior, as defined).
//     template<typename M>
//     inline static void setAllToZero(Internal_State<M>& is)
//     {
//         std::get<0>(*(is.vidice)).setAllToZero();
//         std::get<1>(*(is.vidice)).setAllToZero();
//         std::get<2>(*(is.vidice)).setAllToZero();
//     }

//     // Merge internal states for all three dice using the given parameters.
//     template<typename M>
//     static void merge_internal_states(Internal_State<M>& is, Internal_State<M>& is_batch,
//                                         long long total_unique_nb_sat_assig, long long nb_sat_assign_in_batch,
//                                         float rhot)
//     {
//         auto merge_start = std::chrono::high_resolution_clock::now();

//         // Get the tuples of dice from the two internal states.
//         M& vidice = *is.vidice;
//         M& vidice_batch = *is_batch.vidice;

//         float one_minus_rhot = 1 - rhot;
//         float scaling_factor = static_cast<float>(total_unique_nb_sat_assig) / nb_sat_assign_in_batch;
//         std::cout << "scaling factor = " << scaling_factor << std::endl;

//         // Merge first die.
//         auto& die1 = std::get<0>(vidice);
//         auto& die1_batch = std::get<0>(vidice_batch);
//         for (int i = 0; i < die1.getNumVar() * die1.getNUMOUTCOMES(); i++) {
//             die1.mu[i] = one_minus_rhot * die1.mu[i] +
//                          rhot * (die1.prior + scaling_factor * die1_batch.mu[i]);
//         }

//         // Merge second die.
//         auto& die2 = std::get<1>(vidice);
//         auto& die2_batch = std::get<1>(vidice_batch);
//         for (int i = 0; i < die2.getNumVar() * die2.getNUMOUTCOMES(); i++) {
//             die2.mu[i] = one_minus_rhot * die2.mu[i] +
//                          rhot * (die2.prior + scaling_factor * die2_batch.mu[i]);
//         }

//         // Merge third die.
//         auto& die3 = std::get<2>(vidice);
//         auto& die3_batch = std::get<2>(vidice_batch);
//         for (int i = 0; i < die3.getNumVar() * die3.getNUMOUTCOMES(); i++) {
//             die3.mu[i] = one_minus_rhot * die3.mu[i] +
//                          rhot * (die3.prior + scaling_factor * die3_batch.mu[i]);
//         }

//         auto merge_end = std::chrono::high_resolution_clock::now();
//         long merge_timeMs = std::chrono::duration_cast<std::chrono::milliseconds>(merge_end - merge_start).count();
//         std::cout << "Merge execution time: " << merge_timeMs << " ms" << std::endl;
//     }
// };


// class svi4_template
// {
// public:
//     // Initialize mu via Gamma for all four dice.
//     template<typename M>
//     inline static void initmuGamma(Internal_State<M>& is)
//     {
//         std::get<0>(*(is.vidice)).initmuGamma();
//         std::get<1>(*(is.vidice)).initmuGamma();
//         std::get<2>(*(is.vidice)).initmuGamma();
//         std::get<3>(*(is.vidice)).initmuGamma();
//     }

//     // Copy the mu values from one internal state to another for all four dice.
//     template<typename M>
//     inline static void copy_mus(Internal_State<M>& is, Internal_State<M>& is_batch)
//     {
//         auto& src_die1 = std::get<0>(*(is.vidice));
//         auto& dst_die1 = std::get<0>(*(is_batch.vidice));
//         copy_mu_for_die(dst_die1, src_die1);

//         auto& src_die2 = std::get<1>(*(is.vidice));
//         auto& dst_die2 = std::get<1>(*(is_batch.vidice));
//         copy_mu_for_die(dst_die2, src_die2);

//         auto& src_die3 = std::get<2>(*(is.vidice));
//         auto& dst_die3 = std::get<2>(*(is_batch.vidice));
//         copy_mu_for_die(dst_die3, src_die3);

//         auto& src_die4 = std::get<3>(*(is.vidice));
//         auto& dst_die4 = std::get<3>(*(is_batch.vidice));
//         copy_mu_for_die(dst_die4, src_die4);
//     }

//     // Prepare (compute digamma values, sums, etc.) for all four dice.
//     template<typename M>
//     inline static void prepare_dice(Internal_State<M>& is)
//     {
//         std::get<0>(*(is.vidice)).prepare_die();
//         std::get<1>(*(is.vidice)).prepare_die();
//         std::get<2>(*(is.vidice)).prepare_die();
//         std::get<3>(*(is.vidice)).prepare_die();
//     }

//     // Set all values in each die to zero (or to prior, as defined).
//     template<typename M>
//     inline static void setAllToZero(Internal_State<M>& is)
//     {
//         std::get<0>(*(is.vidice)).setAllToZero();
//         std::get<1>(*(is.vidice)).setAllToZero();
//         std::get<2>(*(is.vidice)).setAllToZero();
//         std::get<3>(*(is.vidice)).setAllToZero();
//     }

//     // Merge internal states for all four dice using the given parameters.
//     template<typename M>
//     static void merge_internal_states(Internal_State<M>& is, Internal_State<M>& is_batch,
//                                         long long total_unique_nb_sat_assig, long long nb_sat_assign_in_batch,
//                                         float rhot)
//     {
//         auto merge_start = std::chrono::high_resolution_clock::now();

//         // Obtain the tuples of dice from both internal states.
//         M& vidice = *is.vidice;
//         M& vidice_batch = *is_batch.vidice;

//         float one_minus_rhot = 1 - rhot;
//         float scaling_factor = static_cast<float>(total_unique_nb_sat_assig) / nb_sat_assign_in_batch;
//         std::cout << "scaling factor = " << scaling_factor << std::endl;

//         // Merge die 1
//         {
//             auto& die1 = std::get<0>(vidice);
//             auto& die1_batch = std::get<0>(vidice_batch);
//             for (int i = 0; i < die1.getNumVar() * die1.getNUMOUTCOMES(); i++) {
//                 die1.mu[i] = one_minus_rhot * die1.mu[i] +
//                              rhot * (die1.prior + scaling_factor * die1_batch.mu[i]);
//             }
//         }
//         // Merge die 2
//         {
//             auto& die2 = std::get<1>(vidice);
//             auto& die2_batch = std::get<1>(vidice_batch);
//             for (int i = 0; i < die2.getNumVar() * die2.getNUMOUTCOMES(); i++) {
//                 die2.mu[i] = one_minus_rhot * die2.mu[i] +
//                              rhot * (die2.prior + scaling_factor * die2_batch.mu[i]);
//             }
//         }
//         // Merge die 3
//         {
//             auto& die3 = std::get<2>(vidice);
//             auto& die3_batch = std::get<2>(vidice_batch);
//             for (int i = 0; i < die3.getNumVar() * die3.getNUMOUTCOMES(); i++) {
//                 die3.mu[i] = one_minus_rhot * die3.mu[i] +
//                              rhot * (die3.prior + scaling_factor * die3_batch.mu[i]);
//             }
//         }
//         // Merge die 4
//         {
//             auto& die4 = std::get<3>(vidice);
//             auto& die4_batch = std::get<3>(vidice_batch);
//             for (int i = 0; i < die4.getNumVar() * die4.getNUMOUTCOMES(); i++) {
//                 die4.mu[i] = one_minus_rhot * die4.mu[i] +
//                              rhot * (die4.prior + scaling_factor * die4_batch.mu[i]);
//             }
//         }

//         auto merge_end = std::chrono::high_resolution_clock::now();
//         long merge_timeMs = std::chrono::duration_cast<std::chrono::milliseconds>(merge_end - merge_start).count();
//         std::cout << "Merge execution time: " << merge_timeMs << " ms" << std::endl;
//     }
// };


// class svi5_template
// {
// public:
//     // Initialize mu via Gamma for all five dice.
//     template<typename M>
//     inline static void initmuGamma(Internal_State<M>& is)
//     {
//         std::get<0>(*(is.vidice)).initmuGamma();
//         std::get<1>(*(is.vidice)).initmuGamma();
//         std::get<2>(*(is.vidice)).initmuGamma();
//         std::get<3>(*(is.vidice)).initmuGamma();
//         std::get<4>(*(is.vidice)).initmuGamma();
//     }

//     // Copy the mu values from one internal state to another for all five dice.
//     template<typename M>
//     inline static void copy_mus(Internal_State<M>& is, Internal_State<M>& is_batch)
//     {
//         auto& src_die1 = std::get<0>(*(is.vidice));
//         auto& dst_die1 = std::get<0>(*(is_batch.vidice));
//         copy_mu_for_die(dst_die1, src_die1);

//         auto& src_die2 = std::get<1>(*(is.vidice));
//         auto& dst_die2 = std::get<1>(*(is_batch.vidice));
//         copy_mu_for_die(dst_die2, src_die2);

//         auto& src_die3 = std::get<2>(*(is.vidice));
//         auto& dst_die3 = std::get<2>(*(is_batch.vidice));
//         copy_mu_for_die(dst_die3, src_die3);

//         auto& src_die4 = std::get<3>(*(is.vidice));
//         auto& dst_die4 = std::get<3>(*(is_batch.vidice));
//         copy_mu_for_die(dst_die4, src_die4);

//         auto& src_die5 = std::get<4>(*(is.vidice));
//         auto& dst_die5 = std::get<4>(*(is_batch.vidice));
//         copy_mu_for_die(dst_die5, src_die5);
//     }

//     // Prepare (compute digamma values, sums, etc.) for all five dice.
//     template<typename M>
//     inline static void prepare_dice(Internal_State<M>& is)
//     {
//         std::get<0>(*(is.vidice)).prepare_die();
//         std::get<1>(*(is.vidice)).prepare_die();
//         std::get<2>(*(is.vidice)).prepare_die();
//         std::get<3>(*(is.vidice)).prepare_die();
//         std::get<4>(*(is.vidice)).prepare_die();
//     }

//     // Set all values in each die to zero (or to the prior, as defined).
//     template<typename M>
//     inline static void setAllToZero(Internal_State<M>& is)
//     {
//         std::get<0>(*(is.vidice)).setAllToZero();
//         std::get<1>(*(is.vidice)).setAllToZero();
//         std::get<2>(*(is.vidice)).setAllToZero();
//         std::get<3>(*(is.vidice)).setAllToZero();
//         std::get<4>(*(is.vidice)).setAllToZero();
//     }

//     // Merge internal states for all five dice using the given parameters.
//     template<typename M>
//     static void merge_internal_states(Internal_State<M>& is, Internal_State<M>& is_batch,
//                                         long long total_unique_nb_sat_assig, long long nb_sat_assign_in_batch,
//                                         float rhot)
//     {
//         auto merge_start = std::chrono::high_resolution_clock::now();

//         // Get the tuples of dice from both internal states.
//         M& vidice = *is.vidice;
//         M& vidice_batch = *is_batch.vidice;

//         float one_minus_rhot = 1 - rhot;
//         float scaling_factor = static_cast<float>(total_unique_nb_sat_assig) / nb_sat_assign_in_batch;
//         std::cout << "scaling factor = " << scaling_factor << std::endl;

//         // Merge die 1
//         {
//             auto& die1 = std::get<0>(vidice);
//             auto& die1_batch = std::get<0>(vidice_batch);
//             for (int i = 0; i < die1.getNumVar() * die1.getNUMOUTCOMES(); i++) {
//                 die1.mu[i] = one_minus_rhot * die1.mu[i] +
//                              rhot * (die1.prior + scaling_factor * die1_batch.mu[i]);
//             }
//         }
//         // Merge die 2
//         {
//             auto& die2 = std::get<1>(vidice);
//             auto& die2_batch = std::get<1>(vidice_batch);
//             for (int i = 0; i < die2.getNumVar() * die2.getNUMOUTCOMES(); i++) {
//                 die2.mu[i] = one_minus_rhot * die2.mu[i] +
//                              rhot * (die2.prior + scaling_factor * die2_batch.mu[i]);
//             }
//         }
//         // Merge die 3
//         {
//             auto& die3 = std::get<2>(vidice);
//             auto& die3_batch = std::get<2>(vidice_batch);
//             for (int i = 0; i < die3.getNumVar() * die3.getNUMOUTCOMES(); i++) {
//                 die3.mu[i] = one_minus_rhot * die3.mu[i] +
//                              rhot * (die3.prior + scaling_factor * die3_batch.mu[i]);
//             }
//         }
//         // Merge die 4
//         {
//             auto& die4 = std::get<3>(vidice);
//             auto& die4_batch = std::get<3>(vidice_batch);
//             for (int i = 0; i < die4.getNumVar() * die4.getNUMOUTCOMES(); i++) {
//                 die4.mu[i] = one_minus_rhot * die4.mu[i] +
//                              rhot * (die4.prior + scaling_factor * die4_batch.mu[i]);
//             }
//         }
//         // Merge die 5
//         {
//             auto& die5 = std::get<4>(vidice);
//             auto& die5_batch = std::get<4>(vidice_batch);
//             for (int i = 0; i < die5.getNumVar() * die5.getNUMOUTCOMES(); i++) {
//                 die5.mu[i] = one_minus_rhot * die5.mu[i] +
//                              rhot * (die5.prior + scaling_factor * die5_batch.mu[i]);
//             }
//         }

//         auto merge_end = std::chrono::high_resolution_clock::now();
//         long merge_timeMs = std::chrono::duration_cast<std::chrono::milliseconds>(merge_end - merge_start).count();
//         std::cout << "Merge execution time: " << merge_timeMs << " ms" << std::endl;
//     }
// };



// #endif //_SVI_TEMPLATE_H