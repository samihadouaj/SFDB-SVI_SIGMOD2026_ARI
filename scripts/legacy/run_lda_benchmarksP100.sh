

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

# NUM_TOPICS=100
# ALPHA_PRIOR=0.2
# BETA_PRIOR=0.1
# NUM_ITERATIONS=20
# #NUM_ITERATIONS=12
# #NUM_RUNS=3
# NUM_RUNS=2
# SAVE_EVERY=2
# RND_SEED=123
# # NUM_THREADS=1
# PARALLEL_PERP_COMP=TRUE


# ##############################################################
# ########################## KOS ###############################
# ##############################################################
# TotalStart=$(date +%s)
# for ThNB in 4 8 16 24
# do
#     start=$(date +%s)
#     # process KOS dataset with Mallet
#     ${SCRIPTSDIR_ABS_PATH}/run_mallet.sh --datasetName KOS --numTopics ${NUM_TOPICS} --alpha ${ALPHA_PRIOR} --beta ${BETA_PRIOR} --numIterations ${NUM_ITERATIONS} --malletDir ${EXTRASDIR_ABS_PATH}/mallet/Mallet --outputDir ${BENCHMARKSDIR_ABS_PATH} --trainingSetDir ${DATADIR_ABS_PATH}/KOS_train/mallet --testSetDir ${DATADIR_ABS_PATH}/KOS_test/mallet  --saveEvery ${SAVE_EVERY} --rndSeed ${RND_SEED} --numThreads ${ThNB} --numRuns ${NUM_RUNS} --PARALLEL_PERP_COMP ${PARALLEL_PERP_COMP}
#     end=$(date +%s)
#     runtime=$((end-start))
#     echo "Time taken to run Mallet benchmark on KOS_${NUM_TOPICS}topics_A${ALPHA_PRIOR}_B${BETA_PRIOR}_NI${NUM_ITERATIONS}_NT${ThNB}_RND saving every ${SAVE_EVERY}  with is PARALLEL_PERP_COMP = ${PARALLEL_PERP_COMP}: $runtime seconds" | tee -a ${BENCHMARKSDIR_ABS_PATH}/logs/run_lda_benchmarksP_exec_time.txt
# done



# TotalStart=$(date +%s)
# for ThNB in 4 8 16 24
# do
#     start=$(date +%s)
#     # process KOS dataset with GammaPDB/vrdP
#     ${SCRIPTSDIR_ABS_PATH}/run_gammapdb_lda.sh --ldaVariant lda-inmemory-vrexprP --datasetName KOS --numTopics ${NUM_TOPICS} --alpha ${ALPHA_PRIOR} --beta ${BETA_PRIOR} --numIterations ${NUM_ITERATIONS} --malletDir ${EXTRASDIR_ABS_PATH}/mallet/Mallet --outputDir ${BENCHMARKSDIR_ABS_PATH} --trainingSetDir ${DATADIR_ABS_PATH}/KOS_train/mallet --testSetDir ${DATADIR_ABS_PATH}/KOS_test/mallet  --saveEvery ${SAVE_EVERY} --rndSeed ${RND_SEED} --numThreads ${ThNB} --numRuns ${NUM_RUNS} --PARALLEL_PERP_COMP ${PARALLEL_PERP_COMP}
#     end=$(date +%s)
#     runtime=$((end-start))
#     echo "Time taken to run lda-inmemory-vrexprP benchmark on KOS_${NUM_TOPICS}topics_A${ALPHA_PRIOR}_B${BETA_PRIOR}_NI${NUM_ITERATIONS}_NT${ThNB}_RND saving every ${SAVE_EVERY}  with is PARALLEL_PERP_COMP = ${PARALLEL_PERP_COMP} is: $runtime seconds"  | tee -a ${BENCHMARKSDIR_ABS_PATH}/logs/run_lda_benchmarksP_exec_time.txt
# done

# TotalEnd=$(date +%s)
# runtime=$((TotalEnd-TotalStart))
# echo "Benchmarking KOS using 4 8 16 24  threads using gammapdb and mallet took $runtime seconds" | tee -a ${BENCHMARKSDIR_ABS_PATH}/logs/run_lda_benchmarksP_exec_time.txt




# ##################################################################
# ########################## NYTIMES ###############################
# ##################################################################



# for ThNB in  8 16 24
# do
#     start=$(date +%s)
#     # process NYTIMES dataset with GammaPDB/vrdP
#     ${SCRIPTSDIR_ABS_PATH}/run_gammapdb_lda.sh --ldaVariant lda-inmemory-vrexprP --datasetName NYTIMES --numTopics ${NUM_TOPICS} --alpha ${ALPHA_PRIOR} --beta ${BETA_PRIOR} --numIterations ${NUM_ITERATIONS} --malletDir ${EXTRASDIR_ABS_PATH}/mallet/Mallet --outputDir ${BENCHMARKSDIR_ABS_PATH} --trainingSetDir ${DATADIR_ABS_PATH}/NYTIMES_train/mallet --testSetDir ${DATADIR_ABS_PATH}/NYTIMES_test/mallet  --saveEvery ${SAVE_EVERY} --rndSeed ${RND_SEED} --numThreads ${ThNB} --numRuns ${NUM_RUNS} --PARALLEL_PERP_COMP ${PARALLEL_PERP_COMP}
#     end=$(date +%s)
#     runtime=$((end-start))
#     echo "Time taken to run lda-inmemory-vrexprP benchmark on NYTIMES_${NUM_TOPICS}topics_A${ALPHA_PRIOR}_B${BETA_PRIOR}_NI${NUM_ITERATIONS}_NT${ThNB}_RND saving every ${SAVE_EVERY}  with is PARALLEL_PERP_COMP = ${PARALLEL_PERP_COMP} is: $runtime seconds"  | tee -a ${BENCHMARKSDIR_ABS_PATH}/logs/run_lda_benchmarksP_exec_time.txt
# done



# NUM_ITERATIONS=30
# SAVE_EVERY=3


# for ThNB in  8 16 24
# do
#     start=$(date +%s)
#     # process NYTIMES dataset with Mallet
#     ${SCRIPTSDIR_ABS_PATH}/run_mallet.sh --datasetName NYTIMES --numTopics ${NUM_TOPICS} --alpha ${ALPHA_PRIOR} --beta ${BETA_PRIOR} --numIterations ${NUM_ITERATIONS} --malletDir ${EXTRASDIR_ABS_PATH}/mallet/Mallet --outputDir ${BENCHMARKSDIR_ABS_PATH} --trainingSetDir ${DATADIR_ABS_PATH}/NYTIMES_train/mallet --testSetDir ${DATADIR_ABS_PATH}/NYTIMES_test/mallet  --saveEvery ${SAVE_EVERY} --rndSeed ${RND_SEED} --numThreads ${ThNB} --numRuns ${NUM_RUNS} --PARALLEL_PERP_COMP ${PARALLEL_PERP_COMP}
#     end=$(date +%s)
#     runtime=$((end-start))
#     echo "Time taken to run Mallet benchmark on NYTIMES_${NUM_TOPICS}topics_A${ALPHA_PRIOR}_B${BETA_PRIOR}_NI${NUM_ITERATIONS}_NT${ThNB}_RND saving every ${SAVE_EVERY}  with is PARALLEL_PERP_COMP = ${PARALLEL_PERP_COMP}: $runtime seconds" | tee -a ${BENCHMARKSDIR_ABS_PATH}/logs/run_lda_benchmarksP_exec_time.txt
# done




# TotalEnd=$(date +%s)
# runtime=$((TotalEnd-TotalStart))
# echo "Benchmarking NYTIMES using 4 8 16 24  threads using gammapdb and mallet took $runtime seconds" | tee -a ${BENCHMARKSDIR_ABS_PATH}/logs/run_lda_benchmarksP_exec_time.txt





#################################################################
########################## PUBMED ###############################
#################################################################


NUM_TOPICS=100
ALPHA_PRIOR=0.2
BETA_PRIOR=0.1
NUM_ITERATIONS=40
#NUM_ITERATIONS=12
#NUM_RUNS=3
NUM_RUNS=2
SAVE_EVERY=4
RND_SEED=123
# NUM_THREADS=1
PARALLEL_PERP_COMP=TRUE
CHUNK_SIZE=40



TotalStart=$(date +%s)


# NUM_ITERATIONS=30
# SAVE_EVERY=3

for ThNB in  24
do
    start=$(date +%s)
    # process PUBMED dataset with Mallet
    ${SCRIPTSDIR_ABS_PATH}/run_mallet.sh --datasetName PUBMED --numTopics ${NUM_TOPICS} --alpha ${ALPHA_PRIOR} --beta ${BETA_PRIOR} --numIterations ${NUM_ITERATIONS} --malletDir ${EXTRASDIR_ABS_PATH}/mallet/Mallet --outputDir ${BENCHMARKSDIR_ABS_PATH} --trainingSetDir ${DATADIR_ABS_PATH}/PUBMED_train/mallet --testSetDir ${DATADIR_ABS_PATH}/PUBMED_test/mallet  --saveEvery ${SAVE_EVERY} --rndSeed ${RND_SEED} --numThreads ${ThNB} --numRuns ${NUM_RUNS} --PARALLEL_PERP_COMP ${PARALLEL_PERP_COMP} --CHUNK_SIZE ${CHUNK_SIZE}
    end=$(date +%s) 
    runtime=$((end-start))
    echo "Time taken to run Mallet benchmark on PUBMED_${NUM_TOPICS}topics_A${ALPHA_PRIOR}_B${BETA_PRIOR}_NI${NUM_ITERATIONS}_NT${ThNB}_RND saving every ${SAVE_EVERY}  with is PARALLEL_PERP_COMP = ${PARALLEL_PERP_COMP}: $runtime seconds" | tee -a ${BENCHMARKSDIR_ABS_PATH}/logs/run_lda_benchmarksP_exec_time.txt
done


NUM_ITERATIONS=20
SAVE_EVERY=2
for ThNB in  24
do
    start=$(date +%s)
    # process PUBMED dataset with GammaPDB/vrdP
    ${SCRIPTSDIR_ABS_PATH}/run_gammapdb_lda.sh --ldaVariant lda-inmemory-vrexprP --datasetName PUBMED --numTopics ${NUM_TOPICS} --alpha ${ALPHA_PRIOR} --beta ${BETA_PRIOR} --numIterations ${NUM_ITERATIONS} --malletDir ${EXTRASDIR_ABS_PATH}/mallet/Mallet --outputDir ${BENCHMARKSDIR_ABS_PATH} --trainingSetDir ${DATADIR_ABS_PATH}/PUBMED_train/mallet --testSetDir ${DATADIR_ABS_PATH}/PUBMED_test/mallet  --saveEvery ${SAVE_EVERY} --rndSeed ${RND_SEED} --numThreads ${ThNB} --numRuns ${NUM_RUNS} --PARALLEL_PERP_COMP ${PARALLEL_PERP_COMP} --CHUNK_SIZE ${NUM_ITERATIONS}
    end=$(date +%s)
    runtime=$((end-start))
    echo "Time taken to run lda-inmemory-vrexprP benchmark on PUBMED_${NUM_TOPICS}topics_A${ALPHA_PRIOR}_B${BETA_PRIOR}_NI${NUM_ITERATIONS}_NT${ThNB}_RND saving every ${SAVE_EVERY}  with is PARALLEL_PERP_COMP = ${PARALLEL_PERP_COMP} is: $runtime seconds"  | tee -a ${BENCHMARKSDIR_ABS_PATH}/logs/run_lda_benchmarksP_exec_time.txt
done
TotalEnd=$(date +%s)
runtime=$((TotalEnd-TotalStart))
echo "Benchmarking PUBMED using 4 8 16 24 threads using gammapdb and mallet took $runtime seconds" | tee -a ${BENCHMARKSDIR_ABS_PATH}/logs/run_lda_benchmarksP_exec_time.txt

