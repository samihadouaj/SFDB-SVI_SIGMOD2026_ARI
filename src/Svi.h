// #include "atom.h"
// #include "tau.h"
// #include "readData.h"
// #include "internal_state.h"
// #include "VIDie.h"
// #include "VIDieP.h"
// #include "utils/TimeThis.h"
// #include "SavingMalletState.h"
// #include "utils/argparse.hpp"
// #include "utils/DebugHelper.h"
// #include <string>
// #include <signal.h> 
// #include "ViUtils.h"
// #include "omp.h"
// #include "svi_template.h"
// template <std::size_t I, class T>
// using tuple_element_t = typename std::tuple_element<I, T>::type;



// // Computes the internal_state is_batch of a given batch of constraints.
// template <typename SAT_T, typename DICE,typename SVI_TEMPLATE>
// void processBatch(
//    Internal_State<DICE>& is_batch,
//    int* collapse_data,
//    int num_iter_vi,
//    long long total_nb_sat_assig_in_batch
//    )
// {
// std::cout<<"Running processBatch"<<std::endl;
// for(int it=0; it<num_iter_vi; it++)
// {
//    // this does the necessary digamma computations
//    SVI_TEMPLATE::prepare_dice(is_batch);

//    // Compute the local parameters (lambdas)
//    int* context = collapse_data;
//    for(long long record = 0; record < total_nb_sat_assig_in_batch; record++)
//    {
//       SAT_T::computeLocalParams(is_batch, context+ 4*record, record);
//    }

//    // normalize lambdas
//    is_batch.normalizeLambdas();

//    // you want the mus to be set to the priors so that you can compute the new ones
//    SVI_TEMPLATE::setAllToZero(is_batch);

//    // update global parameters (mu of each die)
//    context = collapse_data;
//    for(long long record = 0; record < total_nb_sat_assig_in_batch; record++)
//    {
//       SAT_T::updateGlobalParams(is_batch, context+ 4*record, record);
//    }

//    // set lambdas to zero to compute the new ones in the next iteration
//    is_batch.setLambdasToZero();
// }
// };


// // Computes the internal_state is_batch of a given batch of constraints.
// template <typename SAT_T, typename DICE,typename SVI_TEMPLATE>
// void processBatchP(
//    Internal_State<DICE>& is_batch,
//    int* collapse_data,
//    int num_iter_vi,
//    long long total_nb_sat_assig_in_batch,
//    int num_threads
//    )
// {

// std::cout<<"Running processBatchP"<<std::endl;

// for(int it=0; it<num_iter_vi; it++)
// {
//    // this does the necessary digamma computations
//    SVI_TEMPLATE::prepare_dice(is_batch);

//    // Compute the local parameters (lambdas)
//    int* context = collapse_data;
//    #pragma omp parallel for num_threads(num_threads)
//    for(long long record = 0; record < total_nb_sat_assig_in_batch; record++)
//    {
//       SAT_T::computeLocalParams(is_batch, context+ 4*record, record);
//    }

//    // normalize lambdas
//    is_batch.normalizeLambdas();

//    // you want the mus to be set to the priors so that you can compute the new ones
//    SVI_TEMPLATE::setAllToZero(is_batch);

//    // update global parameters (mu of each die)
//    context = collapse_data;
//    for(long long record = 0; record < total_nb_sat_assig_in_batch; record++)
//    {
//       SAT_T::updateGlobalParams(is_batch, context+ 4*record, record);
//    }

//    // set lambdas to zero to compute the new ones in the next iteration
//    is_batch.setLambdasToZero();
// }
// };



// template <typename SAT_T, typename DICE,typename SVI_TEMPLATE>
// void run_one_svi_iter(
//    int batchId,
//    std::vector<int*>& batch_pointers,
//    std::vector<int>& collapsed_data,
//    long long total_unique_nb_sat_assig ,
//    int nb_sat_assig_per_constr,
//    int batchSize,   
//    int vi_iter_per_batch,
//    Internal_State<DICE>& is,
//    std::vector<float>& all_rhots,
//    int num_threads
// )
// {
//    long long nb_uniq_sat_assign_in_batch = get_nb_sat_assign_in_batch(batchId,batch_pointers,collapsed_data,batchSize,nb_sat_assig_per_constr);
//    static DICE vidice_batch;          // Remember to remove "static" when parallelizing this

//     Internal_State<DICE> is_batch(nb_uniq_sat_assign_in_batch,nb_sat_assig_per_constr,&vidice_batch);  // Remember to remove "STATIC" when parallelizing this

// // Preparing the batch_internal_state by coping the current mu values of the dice of the main internal state
//    SVI_TEMPLATE::copy_mus(is,is_batch);

//    is_batch.setVIDice(&vidice_batch);
//    is_batch.setLambdasToZero();

// // Training the batch_internal_state is_batch
// if(num_threads==1)
//  {  
//    processBatch<SAT_T, DICE, SVI_TEMPLATE>(is_batch, batch_pointers[batchId], vi_iter_per_batch, nb_uniq_sat_assign_in_batch);
// }
// else
// {
//    processBatchP<SAT_T, DICE, SVI_TEMPLATE>(is_batch, batch_pointers[batchId], vi_iter_per_batch, nb_uniq_sat_assign_in_batch,num_threads);
// }


// // Merge the internal_state is with the internal_state is_batch learned from this batch.
//    SVI_TEMPLATE::merge_internal_states(is, is_batch, total_unique_nb_sat_assig, nb_uniq_sat_assign_in_batch, all_rhots[batchId]);
// }



// template <typename EXPRESSION>
// [[clang::jit]]void runsvi(
//     std::vector<int*>& batch_pointers,
//     std::vector<int>& data,
//     std::vector<int>& collapsed_data,
//     long long total_unique_nb_sat_assig ,
//     int nb_sat_assig_per_constr,
//     int total_num_iter,
//     float redPrior,
//     float bluePrior,
//     Vocabulary& malletVocab,
//     const std::string& malletStateDir,
//     const std::string& outfileId,
//     int batchSize,   
//     int vi_iter_per_batch,    
//     int save_every,
//    int num_threads)
//    {   
// // Preparing the main internal_state
//     using SAT_T = tuple_element_t<0,EXPRESSION>;
//     using DICE = tuple_element_t<1,EXPRESSION>;
//     using SVI_TEMPLATE = tuple_element_t<2,EXPRESSION>;

//    DICE vidice;

//    Internal_State<DICE> is(total_unique_nb_sat_assig,nb_sat_assig_per_constr,&vidice);
//    SVI_TEMPLATE::initmuGamma(is);
//    SAT_T::setPriors(is,redPrior,bluePrior);
//    std::vector<float> all_rhots = precompute_rhot(batch_pointers.size());

//    for(int it=0;it<total_num_iter;it++)  
//    {  
//       int batchId = (it%batch_pointers.size());
//       auto start = std::chrono::high_resolution_clock::now();


//       run_one_svi_iter<SAT_T,DICE,SVI_TEMPLATE>(batchId ,batch_pointers, collapsed_data, total_unique_nb_sat_assig ,nb_sat_assig_per_constr,batchSize, vi_iter_per_batch,is,all_rhots, num_threads);

//       auto end = std::chrono::high_resolution_clock::now();
//       long timeMs = std::chrono::duration_cast<std::chrono::milliseconds> (end - start).count();

//       std::cout << "#### " <<it+1<< "," << timeMs << std::endl;


// // ##### SAVING THE STATE AND DISPLAYING THE TOPICS
//       if((it+1)%save_every==0 && save_every!=-1)
//       {

//          // printImportantTopics(is,&malletVocab);
//          is.setLambdasToZero();

//       // Compute the local parameters (lambdas)

//          SVI_TEMPLATE::prepare_dice(is);

//          int* context = collapsed_data.data();
//          #pragma omp parallel for num_threads(12)
//          for(long long record =0; record<total_unique_nb_sat_assig; record++)
//          {   
//             SAT_T::computeLocalParams(is,context+record*4,record);
//          }

//          std::ofstream malletStateOFS;
//          std::string snapshotFilePath = malletStateDir+"/chain-state_"+outfileId+".txt."+std::to_string(it+1);
//          std::cout<<"writing in file: "<<snapshotFilePath<<std::endl;
//          malletStateOFS.open(snapshotFilePath);
//          if (!malletStateOFS.is_open()) {
//          std::cerr << "Failed to open file: " << snapshotFilePath << std::endl;
//          }
//          saveAsMalletBatchVI(malletStateOFS, nb_sat_assig_per_constr, &malletVocab, data, collapsed_data, is);
//          malletStateOFS.flush();
//          malletStateOFS.close();
//          }
//    }
// };
