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
echo "----------------------------- $(date) -----------------------------" >> ${BENCHMARKSDIR_ABS_PATH}/logs/run_cxx_svi_benchmarks_exec_time.txt

#########################################################
################ PUBMED ################################
#########################################################

# Define fixed parameters
ALPHA_PRIOR=0.1
BETA_PRIOR=0.2
NUM_ITERATIONS=300

NUM_RUNS=1
SAVE_EVERY=20

RND_SEED=123
PARALLEL_PERP_COMP=TRUE
BATCH_SIZE=30000
VI_ITER_PER_BATCH=4
# Loop over the two topic settings and the desired thread counts
for NUM_TOPICS in  100; do
  for NUM_THREADS in 24 ; do
    echo "Running PUBMED benchmark for ${NUM_TOPICS} topics with ${NUM_THREADS} threads"
    
    TotalStart=$(date +%s)

    start=$(date +%s)
    ${SCRIPTSDIR_ABS_PATH}/run_cxx_svi.sh \
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
      | tee -a ${BENCHMARKSDIR_ABS_PATH}/logs/run_cxx_svi_benchmarks_exec_time.txt

    TotalEnd=$(date +%s)
    total_runtime=$((TotalEnd - TotalStart))
    echo "Total benchmarking time for PUBMED dataset with ${NUM_TOPICS} topics and ${NUM_THREADS} threads: $total_runtime seconds" \
      | tee -a ${BENCHMARKSDIR_ABS_PATH}/logs/run_cxx_svi_benchmarks_exec_time.txt
  done
done
