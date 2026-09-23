#!/bin/bash

# on failure, terminate the script immediately
set -e

# Determine the script's absolute path
SCRIPTSDIR_ABS_PATH=$(readlink -f ${BASH_SOURCE[0]})
SCRIPTSDIR_ABS_PATH=$(dirname ${SCRIPTSDIR_ABS_PATH})

# Determine the project's root path
PROJECT_ROOT_ABS_PATH=$(readlink -f ${SCRIPTSDIR_ABS_PATH}/../)

# Determine commonly used directories
EXTERNALTOOLS_ABS_PATH=$(readlink -f ${PROJECT_ROOT_ABS_PATH}/external)
EXTERNALLIBS_ABS_PATH=$(readlink -f ${PROJECT_ROOT_ABS_PATH}/libs)
PATCHESDIR_ABS_PATH=$(readlink -f ${PROJECT_ROOT_ABS_PATH}/patches)
BUILDDIR_ABS_PATH=$(readlink -f ${PROJECT_ROOT_ABS_PATH}/build)
DATADIR_ABS_PATH=$(readlink -f ${PROJECT_ROOT_ABS_PATH}/data)
CONFDIR_ABS_PATH=$(readlink -f ${PROJECT_ROOT_ABS_PATH}/conf)
EXTRASDIR_ABS_PATH=$(readlink -f ${PROJECT_ROOT_ABS_PATH}/extras)
BENCHMARKSDIR_ABS_PATH=$(readlink -f ${PROJECT_ROOT_ABS_PATH}/benchmarks)

# Relocate <root>/data, if necessary
if [ $(grep -c "^[^#]" $CONFDIR_ABS_PATH/datadir_redirect.txt) != "0" ]; then
  mkdir -p $(grep "^[^#]" $CONFDIR_ABS_PATH/datadir_redirect.txt | head -n 1)
  DATADIR_ABS_PATH=$(readlink -f $(grep "^[^#]" ${CONFDIR_ABS_PATH}/datadir_redirect.txt | head -n 1))
  echo "NOTICE: <root>/data directory was relocated to ${DATADIR_ABS_PATH}"
fi

mkdir -p ${BENCHMARKSDIR_ABS_PATH}/logs
echo "----------------------------- $(date) -----------------------------" >> ${BENCHMARKSDIR_ABS_PATH}/logs/run_lda_benchmarks_exec_time.txt

# NUM_TOPICS=20
# ALPHA_PRIOR=0.1
# BETA_PRIOR=0.01
# NUM_ITERATIONS=30
# #NUM_ITERATIONS=12
# #NUM_RUNS=3
# NUM_RUNS=1
# SAVE_EVERY=3
# RND_SEED=123
# NUM_THREADS=8
# PARALLEL_PERP_COMP=TRUE
# # BATCH_SIZE=2500000
# VI_ITER_PER_BATCH=4
# BATCH_SIZE=40000




# ########################################################
# ############### KOS  ###################################
# ########################################################

# ALPHA_PRIOR=0.2
# BETA_PRIOR=0.1
# NUM_ITERATIONS=30
# NUM_RUNS=1
# SAVE_EVERY=3
# RND_SEED=123
# PARALLEL_PERP_COMP=TRUE
# BATCH_SIZE=40000  # Adjust or uncomment your preferred BATCH_SIZE
# VI_ITER_PER_BATCH=4

# # Loop over the two topic settings and the desired thread counts
# for NUM_TOPICS in  50; do
#   for NUM_THREADS in  8 ; do
#     echo "Running benchmark for ${NUM_TOPICS} topics with ${NUM_THREADS} threads"

#     TotalStart=$(date +%s)

#     start=$(date +%s)
#     ${SCRIPTSDIR_ABS_PATH}/run_vi.sh \
#       --datasetName KOS \
#       --numTopics ${NUM_TOPICS} \
#       --alpha ${ALPHA_PRIOR} \
#       --beta ${BETA_PRIOR} \
#       --numIterations ${NUM_ITERATIONS} \
#       --malletDir ${EXTRASDIR_ABS_PATH}/mallet/Mallet \
#       --outputDir ${BENCHMARKSDIR_ABS_PATH} \
#       --trainingSetDir ${DATADIR_ABS_PATH}/KOS_train/mallet \
#       --testSetDir ${DATADIR_ABS_PATH}/KOS_test/mallet  \
#       --saveEvery ${SAVE_EVERY} \
#       --rndSeed ${RND_SEED} \
#       --numThreads ${NUM_THREADS} \
#       --numRuns ${NUM_RUNS} \
#       --PARALLEL_PERP_COMP ${PARALLEL_PERP_COMP} \
#       --BATCH_SIZE ${BATCH_SIZE}\
#       --VI_ITER_PER_BATCH ${VI_ITER_PER_BATCH}

#     end=$(date +%s)
#     runtime=$((end - start))
#     echo "Time taken for KOS_${NUM_TOPICS}topics_A${ALPHA_PRIOR}_B${BETA_PRIOR}_NI${NUM_ITERATIONS}_NT${NUM_THREADS}: $runtime seconds" \
#       | tee -a ${BENCHMARKSDIR_ABS_PATH}/logs/run_lda_benchmarks_exec_time.txt

#     TotalEnd=$(date +%s)
#     total_runtime=$((TotalEnd - TotalStart))
#     echo "Total benchmark time for ${NUM_TOPICS} topics with ${NUM_THREADS} threads: ${total_runtime} seconds" \
#       | tee -a ${BENCHMARKSDIR_ABS_PATH}/logs/run_lda_benchmarks_exec_time.txt
#   done
# done



########################################################
############### NYTIMES  ###############################
########################################################

# Define fixed parameters
ALPHA_PRIOR=0.1
BETA_PRIOR=0.2
NUM_ITERATIONS=100
NUM_RUNS=1
SAVE_EVERY=10
RND_SEED=123
PARALLEL_PERP_COMP=TRUE
BATCH_SIZE=700000  
VI_ITER_PER_BATCH=5

# Loop over the two topic settings and the desired thread counts
for NUM_TOPICS in  50; do
  for NUM_THREADS in 8 16 24 ; do
    echo "Running NYTIMES benchmark for ${NUM_TOPICS} topics with ${NUM_THREADS} threads"
    
    TotalStart=$(date +%s)
    start=$(date +%s)
    ${SCRIPTSDIR_ABS_PATH}/run_vi.sh \
      --datasetName NYTIMES \
      --numTopics ${NUM_TOPICS} \
      --alpha ${ALPHA_PRIOR} \
      --beta ${BETA_PRIOR} \
      --numIterations ${NUM_ITERATIONS} \
      --malletDir ${EXTRASDIR_ABS_PATH}/mallet/Mallet \
      --outputDir ${BENCHMARKSDIR_ABS_PATH} \
      --trainingSetDir ${DATADIR_ABS_PATH}/NYTIMES_train/mallet \
      --testSetDir ${DATADIR_ABS_PATH}/NYTIMES_test/mallet \
      --saveEvery ${SAVE_EVERY} \
      --rndSeed ${RND_SEED} \
      --numThreads ${NUM_THREADS} \
      --numRuns ${NUM_RUNS} \
      --PARALLEL_PERP_COMP ${PARALLEL_PERP_COMP} \
      --BATCH_SIZE ${BATCH_SIZE}\
      --VI_ITER_PER_BATCH ${VI_ITER_PER_BATCH}
      
    end=$(date +%s)
    runtime=$((end - start))
    echo "Time taken to run lda-inmemory-vrexpr benchmark on NYTIMES_${NUM_TOPICS}topics_A${ALPHA_PRIOR}_B${BETA_PRIOR}_NI${NUM_ITERATIONS}_NT${NUM_THREADS}_RND saving every ${SAVE_EVERY} with PARALLEL_PERP_COMP=${PARALLEL_PERP_COMP} is: $runtime seconds" \
      | tee -a ${BENCHMARKSDIR_ABS_PATH}/logs/run_lda_benchmarks_exec_time.txt

    TotalEnd=$(date +%s)
    total_runtime=$((TotalEnd - TotalStart))
    echo "Total benchmarking time for NYTIMES dataset with ${NUM_TOPICS} topics and ${NUM_THREADS} threads: $total_runtime seconds" \
      | tee -a ${BENCHMARKSDIR_ABS_PATH}/logs/run_lda_benchmarks_exec_time.txt
  done
done




#########################################################
################ PUBMED  ################################
#########################################################


# Define fixed parameters
ALPHA_PRIOR=0.2
BETA_PRIOR=0.1
NUM_ITERATIONS=110
NUM_RUNS=2
SAVE_EVERY=11
RND_SEED=123
PARALLEL_PERP_COMP=TRUE
BATCH_SIZE=4000000  
VI_ITER_PER_BATCH=3

# Loop over the two topic settings and the desired thread counts
for NUM_TOPICS in  50; do
  for NUM_THREADS in 8 16 24 ; do
    echo "Running PUBMED benchmark for ${NUM_TOPICS} topics with ${NUM_THREADS} threads"
    
    TotalStart=$(date +%s)

    start=$(date +%s)
    ${SCRIPTSDIR_ABS_PATH}/run_vi.sh \
      --datasetName PUBMED \
      --numTopics ${NUM_TOPICS} \
      --alpha ${ALPHA_PRIOR} \
      --beta ${BETA_PRIOR} \
      --numIterations ${NUM_ITERATIONS} \
      --malletDir ${EXTRASDIR_ABS_PATH}/mallet/Mallet \
      --outputDir ${BENCHMARKSDIR_ABS_PATH} \
      --trainingSetDir ${DATADIR_ABS_PATH}/PUBMED_train/mallet \
      --testSetDir ${DATADIR_ABS_PATH}/PUBMED_test/mallet \
      --saveEvery ${SAVE_EVERY} \
      --rndSeed ${RND_SEED} \
      --numThreads ${NUM_THREADS} \
      --numRuns ${NUM_RUNS} \
      --PARALLEL_PERP_COMP ${PARALLEL_PERP_COMP} \
      --BATCH_SIZE ${BATCH_SIZE}\
      --VI_ITER_PER_BATCH ${VI_ITER_PER_BATCH}
      
    end=$(date +%s)
    runtime=$((end - start))
    echo "Time taken to run lda-inmemory-vrexpr benchmark on PUBMED_${NUM_TOPICS}topics_A${ALPHA_PRIOR}_B${BETA_PRIOR}_NI${NUM_ITERATIONS}_NT${NUM_THREADS}_RND saving every ${SAVE_EVERY} with PARALLEL_PERP_COMP=${PARALLEL_PERP_COMP} is: $runtime seconds" \
      | tee -a ${BENCHMARKSDIR_ABS_PATH}/logs/run_lda_benchmarks_exec_time.txt

    TotalEnd=$(date +%s)
    total_runtime=$((TotalEnd - TotalStart))
    echo "Total benchmarking time for PUBMED dataset with ${NUM_TOPICS} topics and ${NUM_THREADS} threads: $total_runtime seconds" \
      | tee -a ${BENCHMARKSDIR_ABS_PATH}/logs/run_lda_benchmarks_exec_time.txt
  done
done







# #########################################################
# ################ WIKI08  ################################
# #########################################################


# # Define fixed parameters
# ALPHA_PRIOR=0.2
# BETA_PRIOR=0.1
# NUM_ITERATIONS=60
# NUM_RUNS=2
# SAVE_EVERY=6
# RND_SEED=123
# PARALLEL_PERP_COMP=TRUE
# BATCH_SIZE=8500000  
# VI_ITER_PER_BATCH=6

# # Loop over the two topic settings and the desired thread counts
# for NUM_TOPICS in  200; do
#   for NUM_THREADS in 8 ; do
#     echo "Running wiki08 benchmark for ${NUM_TOPICS} topics with ${NUM_THREADS} threads"
    
#     TotalStart=$(date +%s)

#     start=$(date +%s)
#     ${SCRIPTSDIR_ABS_PATH}/run_vi.sh \
#       --datasetName wiki08 \
#       --numTopics ${NUM_TOPICS} \
#       --alpha ${ALPHA_PRIOR} \
#       --beta ${BETA_PRIOR} \
#       --numIterations ${NUM_ITERATIONS} \
#       --malletDir ${EXTRASDIR_ABS_PATH}/mallet/Mallet \
#       --outputDir ${BENCHMARKSDIR_ABS_PATH} \
#       --trainingSetDir ${DATADIR_ABS_PATH}/wiki08_train/mallet \
#       --testSetDir ${DATADIR_ABS_PATH}/wiki08_test/mallet \
#       --saveEvery ${SAVE_EVERY} \
#       --rndSeed ${RND_SEED} \
#       --numThreads ${NUM_THREADS} \
#       --numRuns ${NUM_RUNS} \
#       --PARALLEL_PERP_COMP ${PARALLEL_PERP_COMP} \
#       --BATCH_SIZE ${BATCH_SIZE}\
#       --VI_ITER_PER_BATCH ${VI_ITER_PER_BATCH}
      
#     end=$(date +%s)
#     runtime=$((end - start))
#     echo "Time taken to run lda-inmemory-vrexpr benchmark on wiki08_${NUM_TOPICS}topics_A${ALPHA_PRIOR}_B${BETA_PRIOR}_NI${NUM_ITERATIONS}_NT${NUM_THREADS}_RND saving every ${SAVE_EVERY} with PARALLEL_PERP_COMP=${PARALLEL_PERP_COMP} is: $runtime seconds" \
#       | tee -a ${BENCHMARKSDIR_ABS_PATH}/logs/run_lda_benchmarks_exec_time.txt

#     TotalEnd=$(date +%s)
#     total_runtime=$((TotalEnd - TotalStart))
#     echo "Total benchmarking time for wiki08 dataset with ${NUM_TOPICS} topics and ${NUM_THREADS} threads: $total_runtime seconds" \
#       | tee -a ${BENCHMARKSDIR_ABS_PATH}/logs/run_lda_benchmarks_exec_time.txt
#   done
# done




# result: bs_700k; vi_iter_batch=5

# ###########################################################################
# ################ GRID SEARCH NYTIMES 100topics ############################
# ###########################################################################

# # Define fixed parameters
# ALPHA_PRIOR=0.1
# BETA_PRIOR=0.2
# # NUM_ITERATIONS=240
# NUM_RUNS=1
# # SAVE_EVERY=60
# RND_SEED=123
# PARALLEL_PERP_COMP=TRUE
# NUM_TOPICS=100
# NUM_THREADS=16





# PARAMS_LIST=(
#   # "NUM_ITERATIONS=150 SAVE_EVERY=30  BATCH_SIZE=1400000  VI_ITERS='2 3 5'"
#   # "NUM_ITERATIONS=300 SAVE_EVERY=60  BATCH_SIZE=700000   VI_ITERS='3'"
#   "NUM_ITERATIONS=80  SAVE_EVERY=8  BATCH_SIZE=800000  VI_ITERS='4'"
#   "NUM_ITERATIONS=70  SAVE_EVERY=7  BATCH_SIZE=1000000  VI_ITERS='4 5'"
#   "NUM_ITERATIONS=60  SAVE_EVERY=6  BATCH_SIZE=1100000  VI_ITERS='4 5'"
#   "NUM_ITERATIONS=30  SAVE_EVERY=3  BATCH_SIZE=2200000  VI_ITERS='3 4 5'"
#   "NUM_ITERATIONS=20  SAVE_EVERY=2  BATCH_SIZE=3400000  VI_ITERS='3 4 5'"
# )

# ##############################################################################
# # Main loop: for each parameter block, run over all VI_ITER_PER_BATCH values
# ##############################################################################
# for param_block in "${PARAMS_LIST[@]}"; do
  
#   # "eval" will assign the variables inside the string (e.g. NUM_ITERATIONS=240)
#   eval "$param_block"

#   echo "========================================"
#   echo "Running with: "
#   echo "  NUM_ITERATIONS=${NUM_ITERATIONS}"
#   echo "  SAVE_EVERY=${SAVE_EVERY}"
#   echo "  BATCH_SIZE=${BATCH_SIZE}"
#   echo "  VI_ITERS=${VI_ITERS}"
#   echo "========================================"

#   for VI_ITER_PER_BATCH in ${VI_ITERS}; do
#     echo "Running NYTIMES benchmark with BATCH_SIZE=${BATCH_SIZE}, VI_ITER_PER_BATCH=${VI_ITER_PER_BATCH}"

#     TotalStart=$(date +%s)
#     start=$(date +%s)

#     "${SCRIPTSDIR_ABS_PATH}/run_vi.sh" \
#       --datasetName NYTIMES \
#       --numTopics "${NUM_TOPICS}" \
#       --alpha "${ALPHA_PRIOR}" \
#       --beta "${BETA_PRIOR}" \
#       --numIterations "${NUM_ITERATIONS}" \
#       --malletDir "${EXTRASDIR_ABS_PATH}/mallet/Mallet" \
#       --outputDir "${BENCHMARKSDIR_ABS_PATH}" \
#       --trainingSetDir "${DATADIR_ABS_PATH}/NYTIMES_train/mallet" \
#       --testSetDir "${DATADIR_ABS_PATH}/NYTIMES_test/mallet" \
#       --saveEvery "${SAVE_EVERY}" \
#       --rndSeed "${RND_SEED}" \
#       --numThreads "${NUM_THREADS}" \
#       --numRuns "${NUM_RUNS}" \
#       --PARALLEL_PERP_COMP "${PARALLEL_PERP_COMP}" \
#       --BATCH_SIZE "${BATCH_SIZE}" \
#       --VI_ITER_PER_BATCH "${VI_ITER_PER_BATCH}"

#     end=$(date +%s)
#     runtime=$((end - start))
#     echo "Time taken to run lda-inmemory-vrexpr on NYTIMES_${NUM_TOPICS}topics A${ALPHA_PRIOR}_B${BETA_PRIOR}_NI${NUM_ITERATIONS}_NT${NUM_THREADS}_RND, saving every ${SAVE_EVERY}, PARALLEL_PERP_COMP=${PARALLEL_PERP_COMP} is: $runtime seconds" \
#       | tee -a "${BENCHMARKSDIR_ABS_PATH}/logs/run_lda_benchmarks_exec_time.txt"

#     TotalEnd=$(date +%s)
#     total_runtime=$((TotalEnd - TotalStart))
#     echo "Total benchmarking time for NYTIMES dataset with ${NUM_TOPICS} topics and ${NUM_THREADS} threads: $total_runtime seconds" \
#       | tee -a "${BENCHMARKSDIR_ABS_PATH}/logs/run_lda_benchmarks_exec_time.txt"
#   done

# done


# result: bs_4M; vi_iter_batch=3

# #################################################################
# ################ GRID SEARCH PUBMED #############################
# #################################################################

# # Define fixed parameters
# ALPHA_PRIOR=0.1
# BETA_PRIOR=0.2
# # NUM_ITERATIONS=240
# NUM_RUNS=1
# # SAVE_EVERY=60
# RND_SEED=123
# PARALLEL_PERP_COMP=TRUE
# NUM_TOPICS=50
# NUM_THREADS=4





# PARAMS_LIST=(
#   # "NUM_ITERATIONS=150 SAVE_EVERY=30  BATCH_SIZE=1400000  VI_ITERS='2 3 5'"
#   # "NUM_ITERATIONS=300 SAVE_EVERY=60  BATCH_SIZE=700000   VI_ITERS='3'"
#   "NUM_ITERATIONS=90  SAVE_EVERY=18  BATCH_SIZE=2500000  VI_ITERS='5'"
#   "NUM_ITERATIONS=75  SAVE_EVERY=15  BATCH_SIZE=3000000  VI_ITERS='3 5'"
#   "NUM_ITERATIONS=57  SAVE_EVERY=12  BATCH_SIZE=4000000  VI_ITERS='3 5'"
# )

# ##############################################################################
# # Main loop: for each parameter block, run over all VI_ITER_PER_BATCH values
# ##############################################################################
# for param_block in "${PARAMS_LIST[@]}"; do
  
#   # "eval" will assign the variables inside the string (e.g. NUM_ITERATIONS=240)
#   eval "$param_block"

#   echo "========================================"
#   echo "Running with: "
#   echo "  NUM_ITERATIONS=${NUM_ITERATIONS}"
#   echo "  SAVE_EVERY=${SAVE_EVERY}"
#   echo "  BATCH_SIZE=${BATCH_SIZE}"
#   echo "  VI_ITERS=${VI_ITERS}"
#   echo "========================================"

#   for VI_ITER_PER_BATCH in ${VI_ITERS}; do
#     echo "Running PUBMED benchmark with BATCH_SIZE=${BATCH_SIZE}, VI_ITER_PER_BATCH=${VI_ITER_PER_BATCH}"

#     TotalStart=$(date +%s)
#     start=$(date +%s)

#     "${SCRIPTSDIR_ABS_PATH}/run_vi.sh" \
#       --datasetName PUBMED \
#       --numTopics "${NUM_TOPICS}" \
#       --alpha "${ALPHA_PRIOR}" \
#       --beta "${BETA_PRIOR}" \
#       --numIterations "${NUM_ITERATIONS}" \
#       --malletDir "${EXTRASDIR_ABS_PATH}/mallet/Mallet" \
#       --outputDir "${BENCHMARKSDIR_ABS_PATH}" \
#       --trainingSetDir "${DATADIR_ABS_PATH}/PUBMED_train/mallet" \
#       --testSetDir "${DATADIR_ABS_PATH}/PUBMED_test/mallet" \
#       --saveEvery "${SAVE_EVERY}" \
#       --rndSeed "${RND_SEED}" \
#       --numThreads "${NUM_THREADS}" \
#       --numRuns "${NUM_RUNS}" \
#       --PARALLEL_PERP_COMP "${PARALLEL_PERP_COMP}" \
#       --BATCH_SIZE "${BATCH_SIZE}" \
#       --VI_ITER_PER_BATCH "${VI_ITER_PER_BATCH}"

#     end=$(date +%s)
#     runtime=$((end - start))
#     echo "Time taken to run lda-inmemory-vrexpr on PUBMED_${NUM_TOPICS}topics A${ALPHA_PRIOR}_B${BETA_PRIOR}_NI${NUM_ITERATIONS}_NT${NUM_THREADS}_RND, saving every ${SAVE_EVERY}, PARALLEL_PERP_COMP=${PARALLEL_PERP_COMP} is: $runtime seconds" \
#       | tee -a "${BENCHMARKSDIR_ABS_PATH}/logs/run_lda_benchmarks_exec_time.txt"

#     TotalEnd=$(date +%s)
#     total_runtime=$((TotalEnd - TotalStart))
#     echo "Total benchmarking time for PUBMED dataset with ${NUM_TOPICS} topics and ${NUM_THREADS} threads: $total_runtime seconds" \
#       | tee -a "${BENCHMARKSDIR_ABS_PATH}/logs/run_lda_benchmarks_exec_time.txt"
#   done

# done






# # results using 100 topics: bs_6.5M; vi_iter_batch=5
# # results using 200 topics: bs_6.5M; vi_iter_batch=9 you can also use  bs_8.5M; vi_iter_batch=6

# ###########################################################################
# ################ GRID SEARCH wiki08 200topics ############################
# ###########################################################################

# # Define fixed parameters
# ALPHA_PRIOR=0.1
# BETA_PRIOR=0.2
# # NUM_ITERATIONS=240
# NUM_RUNS=1
# # SAVE_EVERY=60
# RND_SEED=123
# PARALLEL_PERP_COMP=TRUE
# NUM_TOPICS=200
# NUM_THREADS=24





# PARAMS_LIST=(
#   # "NUM_ITERATIONS=150 SAVE_EVERY=30  BATCH_SIZE=1400000  VI_ITERS='2 3 5'"
#   # "NUM_ITERATIONS=300 SAVE_EVERY=60  BATCH_SIZE=700000   VI_ITERS='3'"
#   # "NUM_ITERATIONS=105  SAVE_EVERY=21  BATCH_SIZE=2500000  VI_ITERS='5'"
#   # "NUM_ITERATIONS=75  SAVE_EVERY=15  BATCH_SIZE=3000000  VI_ITERS='3 5'"
#   # "NUM_ITERATIONS=84  SAVE_EVERY=14  BATCH_SIZE=4000000  VI_ITERS='3 5'"
#   # "NUM_ITERATIONS=72  SAVE_EVERY=12  BATCH_SIZE=4500000  VI_ITERS='3 4 5'"
#   # "NUM_ITERATIONS=60  SAVE_EVERY=10  BATCH_SIZE=5000000  VI_ITERS='5'"
#   # "NUM_ITERATIONS=42  SAVE_EVERY=7  BATCH_SIZE=7500000  VI_ITERS='5 6 7'"
#   # "NUM_ITERATIONS=36  SAVE_EVERY=6  BATCH_SIZE=8500000  VI_ITERS='5 6 7'"  
#   "NUM_ITERATIONS=48  SAVE_EVERY=8  BATCH_SIZE=6500000  VI_ITERS='9'"  

# )

# ##############################################################################
# # Main loop: for each parameter block, run over all VI_ITER_PER_BATCH values
# ##############################################################################
# for param_block in "${PARAMS_LIST[@]}"; do
  
#   # "eval" will assign the variables inside the string (e.g. NUM_ITERATIONS=240)
#   eval "$param_block"

#   echo "========================================"
#   echo "Running with: "
#   echo "  NUM_ITERATIONS=${NUM_ITERATIONS}"
#   echo "  SAVE_EVERY=${SAVE_EVERY}"
#   echo "  BATCH_SIZE=${BATCH_SIZE}"
#   echo "  VI_ITERS=${VI_ITERS}"
#   echo "========================================"

#   for VI_ITER_PER_BATCH in ${VI_ITERS}; do
#     echo "Running wiki08 benchmark with BATCH_SIZE=${BATCH_SIZE}, VI_ITER_PER_BATCH=${VI_ITER_PER_BATCH}"

#     TotalStart=$(date +%s)
#     start=$(date +%s)

#     "${SCRIPTSDIR_ABS_PATH}/run_vi.sh" \
#       --datasetName wiki08 \
#       --numTopics "${NUM_TOPICS}" \
#       --alpha "${ALPHA_PRIOR}" \
#       --beta "${BETA_PRIOR}" \
#       --numIterations "${NUM_ITERATIONS}" \
#       --malletDir "${EXTRASDIR_ABS_PATH}/mallet/Mallet" \
#       --outputDir "${BENCHMARKSDIR_ABS_PATH}" \
#       --trainingSetDir "${DATADIR_ABS_PATH}/wiki08_train/mallet" \
#       --testSetDir "${DATADIR_ABS_PATH}/wiki08_test/mallet" \
#       --saveEvery "${SAVE_EVERY}" \
#       --rndSeed "${RND_SEED}" \
#       --numThreads "${NUM_THREADS}" \
#       --numRuns "${NUM_RUNS}" \
#       --PARALLEL_PERP_COMP "${PARALLEL_PERP_COMP}" \
#       --BATCH_SIZE "${BATCH_SIZE}" \
#       --VI_ITER_PER_BATCH "${VI_ITER_PER_BATCH}"

#     end=$(date +%s)
#     runtime=$((end - start))
#     echo "Time taken to run lda-inmemory-vrexpr on wiki08_${NUM_TOPICS}topics A${ALPHA_PRIOR}_B${BETA_PRIOR}_NI${NUM_ITERATIONS}_NT${NUM_THREADS}_RND, saving every ${SAVE_EVERY}, PARALLEL_PERP_COMP=${PARALLEL_PERP_COMP} is: $runtime seconds" \
#       | tee -a "${BENCHMARKSDIR_ABS_PATH}/logs/run_lda_benchmarks_exec_time.txt"

#     TotalEnd=$(date +%s)
#     total_runtime=$((TotalEnd - TotalStart))
#     echo "Total benchmarking time for wiki08 dataset with ${NUM_TOPICS} topics and ${NUM_THREADS} threads: $total_runtime seconds" \
#       | tee -a "${BENCHMARKSDIR_ABS_PATH}/logs/run_lda_benchmarks_exec_time.txt"
#   done
# done
