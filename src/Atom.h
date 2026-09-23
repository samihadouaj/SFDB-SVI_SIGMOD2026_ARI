#ifndef _ATOM_H
#define _ATOM_H
#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>
#include <memory>
#include "Internal_State.h"
#include <chrono>

// AN ATOM REPRESENTS AN ATOMIC CONSTRAINT, YOU THROW A DIE AND YOU OBSERVE A VALUE: RED_did[pos] = topic_id    
template<int VGID, int VARID_ARG,int VALID_ARG,int COUNT_ARG>
class ATOM {
public:


    // Getter methods
    int getVgId() const { return VGID; }
    int getCount() const {return COUNT_ARG; }
    int getVarId() const { return VARID_ARG; }
    int getValid() const { return VALID_ARG; }

    //  static void print(std::vector<int> &context){
    //     std::cout << "Hello this is VAR with VID = " << VGID << " VARID = " << context[VARID_ARG] << " VALID = " << context[VALID_ARG] << std::endl;
    // }

template<typename M>
static inline void setPrior(Internal_State<M>& is, float prior)
{
    M& vidice = *is.vidice;
    auto& die = std::get<VGID>(vidice);
    die.setPrior(prior);
}

template<typename M>
static inline float compute_e_log_theta_atom(Internal_State<M>& is, int* context) 
{ 
    // Dereference vidice since it is a pointer
    M& vidice = *is.vidice;

    // Get the appropriate VIDie object from the tuple
    auto& die = std::get<VGID>(vidice);

    // Retrieve the number of outcomes
    int num_outcomes = die.getNUMOUTCOMES();

    int varId = context[VARID_ARG];
    int valId = context[VALID_ARG];
    // Compute the index
    int index = varId*num_outcomes+valId;
    const float diedigamma_muindex = die.digamma_mu[index];
    const float diedigamma_sum_mu =  die.digamma_sum_mu[varId];
    // float result = (die.digamma_mu[index] - die.digamma_sum_mu[context[VARID_ARG]]);
    float result = diedigamma_muindex - diedigamma_sum_mu;
    return result;
};


template<typename M>
static inline void updateGlobalParamsAtom(Internal_State<M>& is, int* context,long long recordId) 
    {
        auto& die = std::get<VGID>(*(is.vidice));
        const int num_outcomes = die.getNUMOUTCOMES();     
        int varId = context[VARID_ARG];
        int valId = context[VALID_ARG];
        // Compute the index
        int index = varId*num_outcomes+valId;
        #pragma omp atomic
        die.mu[index] += (is.lambdas[recordId]*context[COUNT_ARG]);
    }
};




// AN ATOM REPRESENTS AN ATOMIC CONSTRAINT, YOU THROW A DIE AND YOU OBSERVE A VALUE: RED_did[pos] = topic_id    
// IN ATOM2, VARID IS A CONSTANT (there directly in the template and not read from the csv file)
template<int VGID, int VARID,int VALID_ARG,int COUNT_ARG>
class ATOM2 {
public:


    // Getter methods
    int getVgId() const { return VGID; }
    int getCount() const {return COUNT_ARG; }
    int getVarId() const { return VARID; }
    int getValid() const { return VALID_ARG; }

    //  static void print(std::vector<int> &context){
    //     std::cout << "Hello this is VAR with VID = " << VGID << " VARID = " << context[VARID_ARG] << " VALID = " << context[VALID_ARG] << std::endl;
    // }

template<typename M>
static inline void setPrior(Internal_State<M>& is, float prior)
{
    M& vidice = *is.vidice;
    auto& die = std::get<VGID>(vidice);
    die.setPrior(prior);
}

template<typename M>
static inline float compute_e_log_theta_atom(Internal_State<M>& is, int* context) 
{ 
    // Dereference vidice since it is a pointer
    M& vidice = *is.vidice;

    // Get the appropriate VIDie object from the tuple
    auto& die = std::get<VGID>(vidice);

    // Retrieve the number of outcomes
    int num_outcomes = die.getNUMOUTCOMES();

    int varId = VARID;
    int valId = context[VALID_ARG];
    // Compute the index
    int index = varId*num_outcomes+valId;
    const float diedigamma_muindex = die.digamma_mu[index];
    const float diedigamma_sum_mu =  die.digamma_sum_mu[varId];
    // float result = (die.digamma_mu[index] - die.digamma_sum_mu[context[VARID_ARG]]);
    float result = diedigamma_muindex - diedigamma_sum_mu;
    return result;
};


template<typename M>
static inline void updateGlobalParamsAtom(Internal_State<M>& is, int* context,long long recordId) 
    {
        auto& die = std::get<VGID>(*(is.vidice));
        const int num_outcomes = die.getNUMOUTCOMES();     
        int varId = VARID;
        int valId = context[VALID_ARG];
        // Compute the index
        int index = varId*num_outcomes+valId;
        #pragma omp atomic
        die.mu[index] += (is.lambdas[recordId]*context[COUNT_ARG]);
    }
};



// AN ATOM REPRESENTS AN ATOMIC CONSTRAINT, YOU THROW A DIE AND YOU OBSERVE A VALUE: RED_did[pos] = topic_id    
// IN ATOM3, VALID IS A CONSTANT (there directly in the template and not read from the csv file)
template<int VGID, int VARID_ARG,int VALID,int COUNT_ARG>
class ATOM3 {
public:


    // Getter methods
    int getVgId() const { return VGID; }
    int getCount() const {return COUNT_ARG; }
    int getVarId() const { return VARID_ARG; }
    int getValid() const { return VALID; }

    //  static void print(std::vector<int> &context){
    //     std::cout << "Hello this is VAR with VID = " << VGID << " VARID = " << context[VARID_ARG] << " VALID = " << context[VALID_ARG] << std::endl;
    // }

template<typename M>
static inline void setPrior(Internal_State<M>& is, float prior)
{
    M& vidice = *is.vidice;
    auto& die = std::get<VGID>(vidice);
    die.setPrior(prior);
}

template<typename M>
static inline float compute_e_log_theta_atom(Internal_State<M>& is, int* context) 
{ 
    // Dereference vidice since it is a pointer
    M& vidice = *is.vidice;

    // Get the appropriate VIDie object from the tuple
    auto& die = std::get<VGID>(vidice);

    // Retrieve the number of outcomes
    int num_outcomes = die.getNUMOUTCOMES();

    int varId = context[VARID_ARG];
    int valId = VALID;
    // Compute the index
    int index = varId*num_outcomes+valId;
    // std::cout<<"context[VARID_ARG] = "<<context[VARID_ARG]<<" context[VALID] = "<<context[VALID]<<std::endl;
    // std::cout<<"VGID"<<VGID<<" VARID_ARG = "<<VARID_ARG<<" varId = "<<varId<<" valId = "<<valId<<std::endl;
    const float diedigamma_muindex = die.digamma_mu[index];
    const float diedigamma_sum_mu =  die.digamma_sum_mu[varId];
    // float result = (die.digamma_mu[index] - die.digamma_sum_mu[context[VARID_ARG]]);
    float result = diedigamma_muindex - diedigamma_sum_mu;
    return result;
};

template<typename M>
static inline void updateGlobalParamsAtom(Internal_State<M>& is, int* context,long long recordId) 
    {
        auto& die = std::get<VGID>(*(is.vidice));
        const int num_outcomes = die.getNUMOUTCOMES();     
        int varId = context[VARID_ARG];
        int valId = VALID;
        // Compute the index
        int index = varId*num_outcomes+valId;
        #pragma omp atomic
        die.mu[index] += (is.lambdas[recordId]*context[COUNT_ARG]);
    }
};

# endif _ATOM_H