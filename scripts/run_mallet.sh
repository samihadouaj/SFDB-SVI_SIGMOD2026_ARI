#!/bin/bash 

# add -x for verbose debug info

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



export MALLET_MEMORY=64g

MALLET_DIR=${EXTERNALTOOLS_ABS_PATH}/mallet/Mallet
OUTPUT_DIR=${BENCHMARKSDIR_ABS_PATH}
DATASET_NAME="KOS"
NUM_TOPICS=20
ALPHA_PRIOR=0.2
BETA_PRIOR=0.1
NUM_ITERATIONS=30
NUM_RUNS=3
SAVE_EVERY=1
RND_SEED=123
SKIP_TRAINING="FALSE"
NUM_THREADS=12
PARALLEL_PERP_COMP="TRUE"
CHUNK_SIZE=20
POSITIONAL=()
while [[ $# -gt 0 ]]; do
  key="$1"

  case $key in
    -m|--malletDir)
      MALLET_DIR="$2"
      shift # past argument
      shift # past value
      ;;
    -o|--outputDir)
      OUTPUT_DIR="$2"
      shift # past argument
      shift # past value
      ;;
    -d|--datasetName)
      DATASET_NAME="$2"
      shift # past argument
      shift # past value
      ;;
    -t|--trainingSetDir)
      TRAININGSET_DIR="$2"
      shift # past argument
      shift # past value
      ;;
    -e|--testSetDir)
      TESTSET_DIR="$2"
      shift # past argument
      shift # past value
      ;;
    -k|--numTopics)
      NUM_TOPICS="$2"
      shift # past argument
      shift # past value
      ;;
    -a|--alpha)
      ALPHA_PRIOR="$2"
      shift # past argument
      shift # past value
      ;;
    -b|--beta)
      BETA_PRIOR="$2"
      shift # past argument
      shift # past value
      ;;
    -n|--numIterations)
      NUM_ITERATIONS="$2"
      shift # past argument
      shift # past value
      ;;
    --numRuns)
      NUM_RUNS="$2"
      shift # past argument
      shift # past value
      ;;
    -s|--saveEvery)
      SAVE_EVERY="$2"
      shift # past argument
      shift # past value
      ;;
    -r|--rndSeed)
      RND_SEED="$2"
      shift # past argument
      shift # past value
      ;;
    --numThreads)
      NUM_THREADS="$2"
      shift # past argument
      shift # past value
      ;;
    --skipTraining)
      SKIP_TRAINING="TRUE"
      shift # past argument
      ;;
    --CHUNK_SIZE)
      CHUNK_SIZE="$2"
      shift
      ;;
    --PARALLEL_PERP_COMP)
      PARALLEL_PERP_COMP="$2"
      shift # past argument
      ;;
    *)    # unknown option
      POSITIONAL+=("$1") # save it in an array for later
      shift # past argument
      ;;
  esac
done

set -- "${POSITIONAL[@]}" # restore positional parameters

TRAININGSET_DIR=${DATADIR_ABS_PATH}/${DATASET_NAME}_train/mallet
TESTSET_DIR=${DATADIR_ABS_PATH}/${DATASET_NAME}_test/mallet
TRAININGSET_NUMTOKENS=$(echo "scale=4; `cat ${DATADIR_ABS_PATH}/${DATASET_NAME}_train/csv2/${DATASET_NAME}_train.csv | wc -l`-2" | bc)
TESTSET_NUMTOKENS=$(echo "scale=4; `cat ${DATADIR_ABS_PATH}/${DATASET_NAME}_test/csv2/${DATASET_NAME}_test.csv | wc -l`-2" | bc)


#ALPHA_SUM=$(($ALPHA_PRIOR*$NUM_TOPICS))
ALPHA_SUM=$(echo "scale=4; $ALPHA_PRIOR*$NUM_TOPICS" | bc)

echo "---------------------------------------------------"
echo "MALLET_DIR      = ${MALLET_DIR}"
echo "OUTPUT_DIR      = ${OUTPUT_DIR}"
echo "DATASET_NAME    = ${DATASET_NAME}"
echo "TRAININGSET_DIR = ${TRAININGSET_DIR}"
echo "TESTSET_DIR     = ${TESTSET_DIR}"
echo "TRAIN_NUMTOKENS = ${TRAININGSET_NUMTOKENS}"
echo "TEST_NUMTOKENS  = ${TESTSET_NUMTOKENS}"
echo "OUTPUT_DIR      = ${OUTPUT_DIR}"
echo "NUM_TOPICS      = ${NUM_TOPICS}"
echo "ALPHA_PRIOR     = ${ALPHA_PRIOR}"
echo "BETA_PRIOR      = ${BETA_PRIOR}"
echo "ALPHA_SUM       = ${ALPHA_SUM}"
echo "NUM_ITERATIONS  = ${NUM_ITERATIONS}"
echo "NUM_RUNS        = ${NUM_RUNS}"
echo "SAVE_EVERY      = ${SAVE_EVERY}"
echo "RND_SEED        = ${RND_SEED}"
echo "SKIP_TRAINING   = ${SKIP_TRAINING}"
echo "NUM_THREADS     = ${NUM_THREADS}"
echo "CHUNK_SIZE      = ${CHUNK_SIZE}"
echo "---------------------------------------------------"

mkdir -p ${OUTPUT_DIR}/mallet_data/chain_states
mkdir -p ${OUTPUT_DIR}/mallet_data/evaluators
mkdir -p ${OUTPUT_DIR}/mallet_data/topics_defs
mkdir -p ${OUTPUT_DIR}/csv

rm -f ${OUTPUT_DIR}/csv/${DATASET_NAME}_test_${NUM_TOPICS}topics_A${ALPHA_PRIOR}_B${BETA_PRIOR}_NI${NUM_ITERATIONS}_NT${NUM_THREADS}_RND${RND_SEED}_mallet_loglik.csv
rm -f ${OUTPUT_DIR}/csv/${DATASET_NAME}_train_${NUM_TOPICS}topics_A${ALPHA_PRIOR}_B${BETA_PRIOR}_NI${NUM_ITERATIONS}_NT${NUM_THREADS}_RND${RND_SEED}_mallet_loglik.csv

rm -f ${OUTPUT_DIR}/csv/${DATASET_NAME}_test_${NUM_TOPICS}topics_A${ALPHA_PRIOR}_B${BETA_PRIOR}_NI${NUM_ITERATIONS}_NT${NUM_THREADS}_RND${RND_SEED}_mallet_perplexity.csv
rm -f ${OUTPUT_DIR}/csv/${DATASET_NAME}_train_${NUM_TOPICS}topics_A${ALPHA_PRIOR}_B${BETA_PRIOR}_NI${NUM_ITERATIONS}_NT${NUM_THREADS}_RND${RND_SEED}_mallet_perplexity.csv

rm -f ${OUTPUT_DIR}/csv/${DATASET_NAME}_test_${NUM_TOPICS}topics_A${ALPHA_PRIOR}_B${BETA_PRIOR}_NI${NUM_ITERATIONS}_NT${NUM_THREADS}_RND${RND_SEED}_mallet_plotdata.csv
rm -f ${OUTPUT_DIR}/csv/${DATASET_NAME}_train_${NUM_TOPICS}topics_A${ALPHA_PRIOR}_B${BETA_PRIOR}_NI${NUM_ITERATIONS}_NT${NUM_THREADS}_RND${RND_SEED}_mallet_plotdata.csv

rm -f ${OUTPUT_DIR}/csv/${DATASET_NAME}_train_${NUM_TOPICS}topics_A${ALPHA_PRIOR}_B${BETA_PRIOR}_NI${NUM_ITERATIONS}_NT${NUM_THREADS}_RND${RND_SEED}_mallet_time.csv
rm -f ${OUTPUT_DIR}/mallet_data/chain_states/chain-state_${DATASET_NAME}_${NUM_TOPICS}topics_A${ALPHA_PRIOR}_B${BETA_PRIOR}_NI${NUM_ITERATIONS}_NT${NUM_THREADS}_RND${RND_SEED}start.gz.0

rm -f ${OUTPUT_DIR}/mallet_data/chain_states/chain-state_${DATASET_NAME}_${NUM_TOPICS}topics_A${ALPHA_PRIOR}_B${BETA_PRIOR}_NI${NUM_ITERATIONS}_NT${NUM_THREADS}_RND${RND_SEED}.gz.*
echo "Profiling Mallet training performance..."
for i in $(seq 1 ${NUM_RUNS});
do
    ${MALLET_DIR}/bin/mallet train-topics --input ${TRAININGSET_DIR}/training.mallet --num-topics ${NUM_TOPICS} --num-iterations ${NUM_ITERATIONS} --alpha ${ALPHA_SUM} --beta ${BETA_PRIOR} --num-threads ${NUM_THREADS} --random-seed ${RND_SEED} --show-topics-interval 0 | tee ${OUTPUT_DIR}/tmp_mallet_profile_${DATASET_NAME}_${NUM_TOPICS}topics_A${ALPHA_PRIOR}_B${BETA_PRIOR}_NI${NUM_ITERATIONS}_NT${NUM_THREADS}_RND${RND_SEED}.txt
    cat ${OUTPUT_DIR}/tmp_mallet_profile_${DATASET_NAME}_${NUM_TOPICS}topics_A${ALPHA_PRIOR}_B${BETA_PRIOR}_NI${NUM_ITERATIONS}_NT${NUM_THREADS}_RND${RND_SEED}.txt | grep -E "^#### " | sed -E "s/^#### /${i},/g" >> ${OUTPUT_DIR}/csv/${DATASET_NAME}_train_${NUM_TOPICS}topics_A${ALPHA_PRIOR}_B${BETA_PRIOR}_NI${NUM_ITERATIONS}_NT${NUM_THREADS}_RND${RND_SEED}_mallet_time.csv
    rm -f ${OUTPUT_DIR}/tmp_mallet_profile_${DATASET_NAME}_${NUM_TOPICS}topics_A${ALPHA_PRIOR}_B${BETA_PRIOR}_NI${NUM_ITERATIONS}_NT${NUM_THREADS}_RND${RND_SEED}.txt
done

for (( start=SAVE_EVERY; start<=NUM_ITERATIONS; start+=CHUNK_SIZE )); do
  chunk_end=$(( start + CHUNK_SIZE - SAVE_EVERY ))
  echo "chunk_start = ${start} , chunk_end= ${chunk_end} "
  echo "Cleaning up ..."
  #rm -f ${OUTPUT_DIR}/lda-evaluator.*.*

if (( start != SAVE_EVERY )); then
  mv ${OUTPUT_DIR}/mallet_data/chain_states/chain-state_${DATASET_NAME}_${NUM_TOPICS}topics_A${ALPHA_PRIOR}_B${BETA_PRIOR}_NI${NUM_ITERATIONS}_NT${NUM_THREADS}_RND${RND_SEED}.gz.$(( start - SAVE_EVERY )) \
     ${OUTPUT_DIR}/mallet_data/chain_states/chain-state_${DATASET_NAME}_${NUM_TOPICS}topics_A${ALPHA_PRIOR}_B${BETA_PRIOR}_NI${NUM_ITERATIONS}_NT${NUM_THREADS}_RND${RND_SEED}start.gz.0
fi

  rm -f ${OUTPUT_DIR}/mallet_data/evaluators/evaluator_${DATASET_NAME}_${NUM_TOPICS}topics_A${ALPHA_PRIOR}_B${BETA_PRIOR}_NI${NUM_ITERATIONS}_NT${NUM_THREADS}_RND${RND_SEED}.*
  rm -f ${OUTPUT_DIR}/mallet_data/topics_defs/topics-def_${DATASET_NAME}_${NUM_TOPICS}topics_A${ALPHA_PRIOR}_B${BETA_PRIOR}_NI${NUM_ITERATIONS}_NT${NUM_THREADS}_RND${RND_SEED}.*.txt
  

  # rm -f ${OUTPUT_DIR}/csv/${DATASET_NAME}_test_${NUM_TOPICS}topics_A${ALPHA_PRIOR}_B${BETA_PRIOR}_NI${NUM_ITERATIONS}_NT${NUM_THREADS}_RND${RND_SEED}_mallet_loglik.csv
  # rm -f ${OUTPUT_DIR}/csv/${DATASET_NAME}_train_${NUM_TOPICS}topics_A${ALPHA_PRIOR}_B${BETA_PRIOR}_NI${NUM_ITERATIONS}_NT${NUM_THREADS}_RND${RND_SEED}_mallet_loglik.csv

  # rm -f ${OUTPUT_DIR}/csv/${DATASET_NAME}_test_${NUM_TOPICS}topics_A${ALPHA_PRIOR}_B${BETA_PRIOR}_NI${NUM_ITERATIONS}_NT${NUM_THREADS}_RND${RND_SEED}_mallet_perplexity.csv
  # rm -f ${OUTPUT_DIR}/csv/${DATASET_NAME}_train_${NUM_TOPICS}topics_A${ALPHA_PRIOR}_B${BETA_PRIOR}_NI${NUM_ITERATIONS}_NT${NUM_THREADS}_RND${RND_SEED}_mallet_perplexity.csv

  # rm -f ${OUTPUT_DIR}/csv/${DATASET_NAME}_test_${NUM_TOPICS}topics_A${ALPHA_PRIOR}_B${BETA_PRIOR}_NI${NUM_ITERATIONS}_NT${NUM_THREADS}_RND${RND_SEED}_mallet_plotdata.csv
  # rm -f ${OUTPUT_DIR}/csv/${DATASET_NAME}_train_${NUM_TOPICS}topics_A${ALPHA_PRIOR}_B${BETA_PRIOR}_NI${NUM_ITERATIONS}_NT${NUM_THREADS}_RND${RND_SEED}_mallet_plotdata.csv

  if [ "$SKIP_TRAINING" == "FALSE" ]; then
    #rm -f ${OUTPUT_DIR}/chain_states/chain-state.*.gz*
 
    rm -f ${OUTPUT_DIR}/mallet_data/chain_states/chain-state_${DATASET_NAME}_${NUM_TOPICS}topics_A${ALPHA_PRIOR}_B${BETA_PRIOR}_NI${NUM_ITERATIONS}_NT${NUM_THREADS}_RND${RND_SEED}.gz.*
    
  fi

  rm -f ${OUTPUT_DIR}/tmp_mallet_profile_${DATASET_NAME}_${NUM_TOPICS}topics_A${ALPHA_PRIOR}_B${BETA_PRIOR}_NI${NUM_ITERATIONS}_NT${NUM_THREADS}_RND${RND_SEED}.txt
  # rm -f ${OUTPUT_DIR}/csv/${DATASET_NAME}_train_${NUM_TOPICS}topics_A${ALPHA_PRIOR}_B${BETA_PRIOR}_NI${NUM_ITERATIONS}_NT${NUM_THREADS}_RND${RND_SEED}_mallet_time.csv



  if [ "$SKIP_TRAINING" == "FALSE" ]; then

    if (( start == SAVE_EVERY )); then

        echo "Training LDA model for the first chunk ..."
        ${MALLET_DIR}/bin/mallet train-topics --input ${TRAININGSET_DIR}/training.mallet --num-topics ${NUM_TOPICS} --num-iterations ${CHUNK_SIZE} --alpha ${ALPHA_SUM} --beta ${BETA_PRIOR} --output-state ${OUTPUT_DIR}/mallet_data/chain_states/chain-state_${DATASET_NAME}_${NUM_TOPICS}topics_A${ALPHA_PRIOR}_B${BETA_PRIOR}_NI${NUM_ITERATIONS}_NT${NUM_THREADS}_RND${RND_SEED}.gz --output-state-interval ${SAVE_EVERY} --num-threads ${NUM_THREADS} --random-seed ${RND_SEED} --show-topics-interval 0

    else
        echo "Training LDA model for not the first chunk... ${start-SAVE_EVERY}"
        ${MALLET_DIR}/bin/mallet train-topics --input ${TRAININGSET_DIR}/training.mallet --num-topics ${NUM_TOPICS} --num-iterations ${CHUNK_SIZE} --alpha ${ALPHA_SUM} --beta ${BETA_PRIOR} --output-state ${OUTPUT_DIR}/mallet_data/chain_states/chain-state_${DATASET_NAME}_${NUM_TOPICS}topics_A${ALPHA_PRIOR}_B${BETA_PRIOR}_NI${NUM_ITERATIONS}_NT${NUM_THREADS}_RND${RND_SEED}.gz --output-state-interval ${SAVE_EVERY} --num-threads ${NUM_THREADS} --random-seed ${RND_SEED} --show-topics-interval 0 --input-state ${OUTPUT_DIR}/mallet_data/chain_states/chain-state_${DATASET_NAME}_${NUM_TOPICS}topics_A${ALPHA_PRIOR}_B${BETA_PRIOR}_NI${NUM_ITERATIONS}_NT${NUM_THREADS}_RND${RND_SEED}start.gz.0
  # Renaming generated states to keep their index in line with the global indexing
      for (( i=SAVE_EVERY; i<=CHUNK_SIZE; i+=SAVE_EVERY )); do
        echo "renamign LDA model ..."
        mv ${OUTPUT_DIR}/mallet_data/chain_states/chain-state_${DATASET_NAME}_${NUM_TOPICS}topics_A${ALPHA_PRIOR}_B${BETA_PRIOR}_NI${NUM_ITERATIONS}_NT${NUM_THREADS}_RND${RND_SEED}.gz.${i} ${OUTPUT_DIR}/mallet_data/chain_states/chain-state_${DATASET_NAME}_${NUM_TOPICS}topics_A${ALPHA_PRIOR}_B${BETA_PRIOR}_NI${NUM_ITERATIONS}_NT${NUM_THREADS}_RND${RND_SEED}.gz.$(( i-SAVE_EVERY+start ))
        echo "i-SAVE_EVERY+start = ${i-SAVE_EVERY+start}"
      done

    fi

  fi

  #removing that initial state from the last round that we used to kickstart the current round
  # rm ${OUTPUT_DIR}/mallet_data/chain_states/chain-state_${DATASET_NAME}_${NUM_TOPICS}topics_A${ALPHA_PRIOR}_B${BETA_PRIOR}_NI${NUM_ITERATIONS}_NT${NUM_THREADS}_RND${RND_SEED}.gz.0
  # echo "Finished training "



  ######################### MODEL EVALUATION ######################### 

  if [ "$PARALLEL_PERP_COMP" == "TRUE" ]; then
    echo "Evaluating the LDA model with PARALLEL_PERP_COMP==TRUE ..."

    startEvaluatorCreation=`date +%s.%N`

    for (( i=start; i<=chunk_end; i+=SAVE_EVERY ));
    do
      echo "Processing ${OUTPUT_DIR}/mallet_data/chain_states/chain-state_${DATASET_NAME}_${NUM_TOPICS}topics_A${ALPHA_PRIOR}_B${BETA_PRIOR}_NI${NUM_ITERATIONS}_NT${NUM_THREADS}_RND${RND_SEED}.gz.${i}"
      ${MALLET_DIR}/bin/mallet train-topics  --input ${TRAININGSET_DIR}/training.mallet --input-state ${OUTPUT_DIR}/mallet_data/chain_states/chain-state_${DATASET_NAME}_${NUM_TOPICS}topics_A${ALPHA_PRIOR}_B${BETA_PRIOR}_NI${NUM_ITERATIONS}_NT${NUM_THREADS}_RND${RND_SEED}.gz.${i} --no-inference --evaluator-filename ${OUTPUT_DIR}/mallet_data/evaluators/evaluator_${DATASET_NAME}_${NUM_TOPICS}topics_A${ALPHA_PRIOR}_B${BETA_PRIOR}_NI${NUM_ITERATIONS}_NT${NUM_THREADS}_RND${RND_SEED}.${i} --output-topic-keys ${OUTPUT_DIR}/mallet_data/topics_defs/topics-def_${DATASET_NAME}_${NUM_TOPICS}topics_A${ALPHA_PRIOR}_B${BETA_PRIOR}_NI${NUM_ITERATIONS}_NT${NUM_THREADS}_RND${RND_SEED}.${i}.txt --random-seed ${RND_SEED} &
    done
    wait
    endEvaluatorCreation=`date +%s`
    runtimeEvaluatorCreation=$( echo "$endEvaluatorCreation - $startEvaluatorCreation" | bc -l )
    echo "creating the evaluators parallel took: ${runtimeEvaluatorCreation} ms"

    startEval=`date +%s.%N`
    for (( i=start; i<=chunk_end; i+=SAVE_EVERY )); 
    do
      echo "Evaluating ${OUTPUT_DIR}/mallet_data/chain_states/chain-state_${DATASET_NAME}_${NUM_TOPICS}topics_A${ALPHA_PRIOR}_B${BETA_PRIOR}_NI${NUM_ITERATIONS}_NT${NUM_THREADS}_RND${RND_SEED}.gz.${i}"
        # wait
      printf "%i," $i >> ${OUTPUT_DIR}/csv/${DATASET_NAME}_TEMP_test_${NUM_TOPICS}topics_A${ALPHA_PRIOR}_B${BETA_PRIOR}_NI${NUM_ITERATIONS}_NT${NUM_THREADS}_RND${RND_SEED}_mallet_loglik.{$i}.csv 

      ${MALLET_DIR}/bin/mallet evaluate-topics --input ${TESTSET_DIR}/test.mallet --evaluator ${OUTPUT_DIR}/mallet_data/evaluators/evaluator_${DATASET_NAME}_${NUM_TOPICS}topics_A${ALPHA_PRIOR}_B${BETA_PRIOR}_NI${NUM_ITERATIONS}_NT${NUM_THREADS}_RND${RND_SEED}.${i} --use-resampling FALSE --random-seed ${RND_SEED} | tee -a ${OUTPUT_DIR}/csv/${DATASET_NAME}_TEMP_test_${NUM_TOPICS}topics_A${ALPHA_PRIOR}_B${BETA_PRIOR}_NI${NUM_ITERATIONS}_NT${NUM_THREADS}_RND${RND_SEED}_mallet_loglik.{$i}.csv &



      printf "%i," $i >> ${OUTPUT_DIR}/csv/${DATASET_NAME}_TEMP_train_${NUM_TOPICS}topics_A${ALPHA_PRIOR}_B${BETA_PRIOR}_NI${NUM_ITERATIONS}_NT${NUM_THREADS}_RND${RND_SEED}_mallet_loglik.{$i}.csv 

      ${MALLET_DIR}/bin/mallet evaluate-topics --input ${TRAININGSET_DIR}/training.mallet --evaluator ${OUTPUT_DIR}/mallet_data/evaluators/evaluator_${DATASET_NAME}_${NUM_TOPICS}topics_A${ALPHA_PRIOR}_B${BETA_PRIOR}_NI${NUM_ITERATIONS}_NT${NUM_THREADS}_RND${RND_SEED}.${i} --use-resampling FALSE --random-seed ${RND_SEED} | tee -a  ${OUTPUT_DIR}/csv/${DATASET_NAME}_TEMP_train_${NUM_TOPICS}topics_A${ALPHA_PRIOR}_B${BETA_PRIOR}_NI${NUM_ITERATIONS}_NT${NUM_THREADS}_RND${RND_SEED}_mallet_loglik.{$i}.csv &
      
    done
    wait

    echo "Evaluation parallel is done"
    python3 ${SCRIPTSDIR_ABS_PATH}/mergeLoglikFiles.py  --OUTPUT_DIR ${OUTPUT_DIR} --DATASET_NAME ${DATASET_NAME}  --TRAIN_OR_TEST train --NUM_TOPICS ${NUM_TOPICS} --ALPHA_PRIOR ${ALPHA_PRIOR} --BETA_PRIOR ${BETA_PRIOR} --NUM_ITERATIONS ${NUM_ITERATIONS} --NUM_THREADS ${NUM_THREADS} --RND_SEED ${RND_SEED} --MALLET_OR_GAMMAPDB mallet


    python3 ${SCRIPTSDIR_ABS_PATH}/mergeLoglikFiles.py  --OUTPUT_DIR ${OUTPUT_DIR} --DATASET_NAME ${DATASET_NAME}  --TRAIN_OR_TEST test --NUM_TOPICS ${NUM_TOPICS} --ALPHA_PRIOR ${ALPHA_PRIOR} --BETA_PRIOR ${BETA_PRIOR} --NUM_ITERATIONS ${NUM_ITERATIONS} --NUM_THREADS ${NUM_THREADS} --RND_SEED ${RND_SEED} --MALLET_OR_GAMMAPDB mallet

    rm  ${OUTPUT_DIR}/csv/${DATASET_NAME}_TEMP*mallet*

    endEval=`date +%s`
    runtimeEval=$( echo "$endEval - $startEval" | bc -l )

    echo "evaluate-topics parallel took ${runtimeEval} ms"

  else

    echo "Evaluating the LDA model with PARALLEL_PERP_COMP==FALSE ..."
    startEvaluatorCreation=`date +%s.%N`

    for (( i=start; i<=chunk_end; i+=SAVE_EVERY )); 
    do
      echo "Processing ${OUTPUT_DIR}/mallet_data/chain_states/chain-state_${DATASET_NAME}_${NUM_TOPICS}topics_A${ALPHA_PRIOR}_B${BETA_PRIOR}_NI${NUM_ITERATIONS}_NT${NUM_THREADS}_RND${RND_SEED}.gz.${i}"
      ${MALLET_DIR}/bin/mallet train-topics  --input ${TRAININGSET_DIR}/training.mallet --input-state ${OUTPUT_DIR}/mallet_data/chain_states/chain-state_${DATASET_NAME}_${NUM_TOPICS}topics_A${ALPHA_PRIOR}_B${BETA_PRIOR}_NI${NUM_ITERATIONS}_NT${NUM_THREADS}_RND${RND_SEED}.gz.${i} --no-inference --evaluator-filename ${OUTPUT_DIR}/mallet_data/evaluators/evaluator_${DATASET_NAME}_${NUM_TOPICS}topics_A${ALPHA_PRIOR}_B${BETA_PRIOR}_NI${NUM_ITERATIONS}_NT${NUM_THREADS}_RND${RND_SEED}.${i} --output-topic-keys ${OUTPUT_DIR}/mallet_data/topics_defs/topics-def_${DATASET_NAME}_${NUM_TOPICS}topics_A${ALPHA_PRIOR}_B${BETA_PRIOR}_NI${NUM_ITERATIONS}_NT${NUM_THREADS}_RND${RND_SEED}.${i}.txt --random-seed ${RND_SEED}
    done
    endEvaluatorCreation=`date +%s`
    runtimeEvaluatorCreation=$( echo "$endEvaluatorCreation - $startEvaluatorCreation" | bc -l )
    echo "creating the evaluators seq took: ${runtimeEvaluatorCreation} ms"


    startEval=`date +%s.%N`
    for (( i=start; i<=chunk_end; i+=SAVE_EVERY )); 
    do
      echo "Evaluating ${OUTPUT_DIR}/mallet_data/chain_states/chain-state_${DATASET_NAME}_${NUM_TOPICS}topics_A${ALPHA_PRIOR}_B${BETA_PRIOR}_NI${NUM_ITERATIONS}_NT${NUM_THREADS}_RND${RND_SEED}.gz.${i}"
      printf "%i," $i >> ${OUTPUT_DIR}/csv/${DATASET_NAME}_test_${NUM_TOPICS}topics_A${ALPHA_PRIOR}_B${BETA_PRIOR}_NI${NUM_ITERATIONS}_NT${NUM_THREADS}_RND${RND_SEED}_mallet_loglik.csv
      ${MALLET_DIR}/bin/mallet evaluate-topics --input ${TESTSET_DIR}/test.mallet --evaluator ${OUTPUT_DIR}/mallet_data/evaluators/evaluator_${DATASET_NAME}_${NUM_TOPICS}topics_A${ALPHA_PRIOR}_B${BETA_PRIOR}_NI${NUM_ITERATIONS}_NT${NUM_THREADS}_RND${RND_SEED}.${i} --use-resampling FALSE --random-seed ${RND_SEED} | tee -a ${OUTPUT_DIR}/csv/${DATASET_NAME}_test_${NUM_TOPICS}topics_A${ALPHA_PRIOR}_B${BETA_PRIOR}_NI${NUM_ITERATIONS}_NT${NUM_THREADS}_RND${RND_SEED}_mallet_loglik.csv
      printf "%i," $i >> ${OUTPUT_DIR}/csv/${DATASET_NAME}_train_${NUM_TOPICS}topics_A${ALPHA_PRIOR}_B${BETA_PRIOR}_NI${NUM_ITERATIONS}_NT${NUM_THREADS}_RND${RND_SEED}_mallet_loglik.csv
      ${MALLET_DIR}/bin/mallet evaluate-topics --input ${TRAININGSET_DIR}/training.mallet --evaluator ${OUTPUT_DIR}/mallet_data/evaluators/evaluator_${DATASET_NAME}_${NUM_TOPICS}topics_A${ALPHA_PRIOR}_B${BETA_PRIOR}_NI${NUM_ITERATIONS}_NT${NUM_THREADS}_RND${RND_SEED}.${i} --use-resampling FALSE --random-seed ${RND_SEED} | tee -a ${OUTPUT_DIR}/csv/${DATASET_NAME}_train_${NUM_TOPICS}topics_A${ALPHA_PRIOR}_B${BETA_PRIOR}_NI${NUM_ITERATIONS}_NT${NUM_THREADS}_RND${RND_SEED}_mallet_loglik.csv
    done
      endeval=`date +%s`
    runtimeEval=$( echo "$endeval - $startEval" | bc -l )
    echo "evaluate-topics seq took ${runtimeEval} ms"
  fi

  # computing perplexity for testset
  python3 ${SCRIPTSDIR_ABS_PATH}/loglik2perp.py --srcFile ${OUTPUT_DIR}/csv/${DATASET_NAME}_test_${NUM_TOPICS}topics_A${ALPHA_PRIOR}_B${BETA_PRIOR}_NI${NUM_ITERATIONS}_NT${NUM_THREADS}_RND${RND_SEED}_mallet_loglik.csv --outFile ${OUTPUT_DIR}/csv/${DATASET_NAME}_test_${NUM_TOPICS}topics_A${ALPHA_PRIOR}_B${BETA_PRIOR}_NI${NUM_ITERATIONS}_NT${NUM_THREADS}_RND${RND_SEED}_mallet_perplexity.csv --numTokens ${TESTSET_NUMTOKENS}

  # computing perplexity for trainingset
  python3 ${SCRIPTSDIR_ABS_PATH}/loglik2perp.py --srcFile ${OUTPUT_DIR}/csv/${DATASET_NAME}_train_${NUM_TOPICS}topics_A${ALPHA_PRIOR}_B${BETA_PRIOR}_NI${NUM_ITERATIONS}_NT${NUM_THREADS}_RND${RND_SEED}_mallet_loglik.csv --outFile ${OUTPUT_DIR}/csv/${DATASET_NAME}_train_${NUM_TOPICS}topics_A${ALPHA_PRIOR}_B${BETA_PRIOR}_NI${NUM_ITERATIONS}_NT${NUM_THREADS}_RND${RND_SEED}_mallet_perplexity.csv --numTokens ${TRAININGSET_NUMTOKENS}



done

# generating plot data for testset
python3 ${SCRIPTSDIR_ABS_PATH}/genplotdata.py --timingFile ${OUTPUT_DIR}/csv/${DATASET_NAME}_train_${NUM_TOPICS}topics_A${ALPHA_PRIOR}_B${BETA_PRIOR}_NI${NUM_ITERATIONS}_NT${NUM_THREADS}_RND${RND_SEED}_mallet_time.csv --perplexityFile ${OUTPUT_DIR}/csv/${DATASET_NAME}_test_${NUM_TOPICS}topics_A${ALPHA_PRIOR}_B${BETA_PRIOR}_NI${NUM_ITERATIONS}_NT${NUM_THREADS}_RND${RND_SEED}_mallet_perplexity.csv --outFile ${OUTPUT_DIR}/csv/${DATASET_NAME}_test_${NUM_TOPICS}topics_A${ALPHA_PRIOR}_B${BETA_PRIOR}_NI${NUM_ITERATIONS}_NT${NUM_THREADS}_RND${RND_SEED}_mallet_plotdata.csv

# generating plot data for trainingset
python3 ${SCRIPTSDIR_ABS_PATH}/genplotdata.py --timingFile ${OUTPUT_DIR}/csv/${DATASET_NAME}_train_${NUM_TOPICS}topics_A${ALPHA_PRIOR}_B${BETA_PRIOR}_NI${NUM_ITERATIONS}_NT${NUM_THREADS}_RND${RND_SEED}_mallet_time.csv --perplexityFile ${OUTPUT_DIR}/csv/${DATASET_NAME}_train_${NUM_TOPICS}topics_A${ALPHA_PRIOR}_B${BETA_PRIOR}_NI${NUM_ITERATIONS}_NT${NUM_THREADS}_RND${RND_SEED}_mallet_perplexity.csv --outFile ${OUTPUT_DIR}/csv/${DATASET_NAME}_train_${NUM_TOPICS}topics_A${ALPHA_PRIOR}_B${BETA_PRIOR}_NI${NUM_ITERATIONS}_NT${NUM_THREADS}_RND${RND_SEED}_mallet_plotdata.csv
echo "CHUNK IS DONE GOING TO THE NEXT CHUNK"