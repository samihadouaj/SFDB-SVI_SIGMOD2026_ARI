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



export MALLET_MEMORY=16g
export MALLET_HEAP=16g  # Set heap to 16GB

MALLET_DIR=${EXTERNALLIBS_ABS_PATH}/mallet/Mallet
OUTPUT_DIR=${BENCHMARKSDIR_ABS_PATH}
DATASET_NAME="NYTIMES"
NUM_TOPICS=20
ALPHA_PRIOR=0.2
BETA_PRIOR=0.1
NUM_ITERATIONS=30
NUM_RUNS=0
SAVE_EVERY=1
RND_SEED=123
SKIP_TRAINING="FALSE"
NUM_THREADS=8
PARALLEL_PERP_COMP="TRUE"
POSITIONAL=()
LDA_VARIANT="VI"
BATCH_SIZE=1000
VI_ITER_PER_BATCH=6
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
    --PARALLEL_PERP_COMP)
      PARALLEL_PERP_COMP="$2"
      shift # past argument
      ;;
    --BATCH_SIZE)
      BATCH_SIZE="$2"
      shift # past argument
      ;;
    --VI_ITER_PER_BATCH)
      VI_ITER_PER_BATCH="$2"
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
TRAININGSET_NUMDOCS=$(echo "scale=4; `cat ${DATADIR_ABS_PATH}/${DATASET_NAME}_train/tsv/training.tsv | wc -l`-0" | bc)
TESTSET_NUMDOCS=$(echo "scale=4; `cat ${DATADIR_ABS_PATH}/${DATASET_NAME}_test/tsv/test.tsv | wc -l`-0" | bc)
TRAININGSET_VOCABSIZE=$(echo "scale=4; `cat ${DATADIR_ABS_PATH}/${DATASET_NAME}_train/csv2/${DATASET_NAME}_vocab.csv | wc -l`-1" | bc)
TESTSET_VOCABSIZE=$(echo "scale=4; `cat ${DATADIR_ABS_PATH}/${DATASET_NAME}_test/csv2/${DATASET_NAME}_vocab.csv | wc -l`-1" | bc)


 
echo "TRAININGSET_NUMSAT = ${TRAININGSET_NUMSAT}"
#ALPHA_SUM=$(($ALPHA_PRIOR*$NUM_TOPICS))
ALPHA_SUM=$(echo "scale=4; $ALPHA_PRIOR*$NUM_TOPICS" | bc)


lda_outfileId=${DATASET_NAME}_${NUM_TOPICS}topics_batchS${BATCH_SIZE}_totalNumIter${NUM_ITERATIONS}_NT${NUM_THREADS}_RND${RND_SEED}

TOTAL_ITER=$NUM_ITERATIONS
# TOTAL_ITER=${NUM_ITERATIONS}

echo "---------------------------------------------------"
echo "run_vi_benchmark.sh"
echo "---------------------------------------------------"
echo "LDA_VARIANT     = ${LDA_VARIANT}"
echo "MALLET_DIR      = ${MALLET_DIR}"
echo "OUTPUT_DIR      = ${OUTPUT_DIR}"
echo "DATASET_NAME    = ${DATASET_NAME}"
echo "TRAININGSET_DIR = ${TRAININGSET_DIR}"
echo "TESTSET_DIR     = ${TESTSET_DIR}"
echo "TRAIN_NUMTOKENS = ${TRAININGSET_NUMTOKENS}"
echo "TRAIN_NUMDOCS   = ${TRAININGSET_NUMDOCS}"
echo "TRAIN_VOCABSIZE = ${TRAININGSET_VOCABSIZE}"
echo "TEST_NUMTOKENS  = ${TESTSET_NUMTOKENS}"
echo "TEST_NUMDOCS    = ${TESTSET_NUMDOCS}"
echo "TEST_VOCABSIZE  = ${TESTSET_VOCABSIZE}"
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
echo "PARALLEL_PERP_COMP  = ${PARALLEL_PERP_COMP}"
echo "BATCH_SIZE        = ${BATCH_SIZE}"
echo "lda_outfileId  =    ${lda_outfileId}"
echo "TOTAL_ITER     =    ${TOTAL_ITER}"
echo "VI_ITER_PER_BATCH = ${VI_ITER_PER_BATCH}"
echo "Command       = svi"
echo "---------------------------------------------------"

mkdir -p ${OUTPUT_DIR}/gammapdb_data/${LDA_VARIANT}/chain_states
mkdir -p ${OUTPUT_DIR}/gammapdb_data/${LDA_VARIANT}/evaluators
mkdir -p ${OUTPUT_DIR}/gammapdb_data/${LDA_VARIANT}/topics_defs
mkdir -p ${OUTPUT_DIR}/csv

echo "Cleaning up ..."

rm -f ${OUTPUT_DIR}/gammapdb_data/${LDA_VARIANT}/evaluators/*

rm -f ${OUTPUT_DIR}/csv/${DATASET_NAME}_test_${NUM_TOPICS}topics_batchS${BATCH_SIZE}_totalNumIter${NUM_ITERATIONS}_NT${NUM_THREADS}_RND${RND_SEED}_gammapdb_${LDA_VARIANT}_loglik.csv
rm -f ${OUTPUT_DIR}/csv/${DATASET_NAME}_train_${NUM_TOPICS}topics_batchS${BATCH_SIZE}_totalNumIter${NUM_ITERATIONS}_NT${NUM_THREADS}_RND${RND_SEED}_gammapdb_${LDA_VARIANT}_loglik.csv

rm -f ${OUTPUT_DIR}/csv/${DATASET_NAME}_test_${NUM_TOPICS}topics_batchS${BATCH_SIZE}_totalNumIter${NUM_ITERATIONS}_NT${NUM_THREADS}_RND${RND_SEED}_gammapdb_${LDA_VARIANT}_perplexity.csv
rm -f ${OUTPUT_DIR}/csv/${DATASET_NAME}_train_${NUM_TOPICS}topics_batchS${BATCH_SIZE}_totalNumIter${NUM_ITERATIONS}_NT${NUM_THREADS}_RND${RND_SEED}_gammapdb_${LDA_VARIANT}_perplexity.csv

rm -f ${OUTPUT_DIR}/csv/${DATASET_NAME}_test_${NUM_TOPICS}topics_batchS${BATCH_SIZE}_totalNumIter${NUM_ITERATIONS}_NT${NUM_THREADS}_RND${RND_SEED}_gammapdb_${LDA_VARIANT}_plotdata.csv
rm -f ${OUTPUT_DIR}/csv/${DATASET_NAME}_train_${NUM_TOPICS}topics_batchS${BATCH_SIZE}_totalNumIter${NUM_ITERATIONS}_NT${NUM_THREADS}_RND${RND_SEED}_gammapdb_${LDA_VARIANT}_plotdata.csv

rm -f ${OUTPUT_DIR}/csv/${DATASET_NAME}_TEMP*

if [ "$SKIP_TRAINING" == "FALSE" ]; then
  #rm -f ${OUTPUT_DIR}/chain_states/chain-state.*.gz*
  for i in $(seq ${SAVE_EVERY} ${SAVE_EVERY} ${NUM_ITERATIONS})
  do
     rm -f ${OUTPUT_DIR}/gammapdb_data/VI/chain_states/chain-state_${lda_outfileId}.gz.${i}
  done
fi
rm -f ${OUTPUT_DIR}/gammapdb_data/VI/chain_states/chain-state_${DATASET_NAME}*
rm -f ${OUTPUT_DIR}/tmp_gammapdb_${LDA_VARIANT}_profile_${lda_outfileId}.txt
rm -f ${OUTPUT_DIR}/csv/${DATASET_NAME}_train_${NUM_TOPICS}topics_batchS${BATCH_SIZE}_totalNumIter${NUM_ITERATIONS}_NT${NUM_THREADS}_RND${RND_SEED}_gammapdb_${LDA_VARIANT}_time.csv

# cp ${PROJECT_ROOT_ABS_PATH}/out ${BUILDDIR_ABS_PATH}
echo "Profiling GammaPDB training performance..."
for i in $(seq 1 ${NUM_RUNS})
do
   ${BUILDDIR_ABS_PATH}/gammapdb_arrow --command svi  --lda_datasetName ${DATASET_NAME} --lda_numDocuments ${TRAININGSET_NUMDOCS} --lda_vocabSize ${TRAININGSET_VOCABSIZE} --lda_corpusSize ${TRAININGSET_NUMTOKENS} --lda_alpha ${ALPHA_PRIOR} --lda_beta ${BETA_PRIOR} --lda_numTopics ${NUM_TOPICS} --total_num_iter ${NUM_ITERATIONS} --rndSeed ${RND_SEED} --reportEvery -1 --outputDir ${OUTPUT_DIR} --lda_vi_dataset_dir ${DATADIR_ABS_PATH}/${DATASET_NAME}_train --lda_malletStateDir ${OUTPUT_DIR}/gammapdb_data/${LDA_VARIANT}/chain_states --lda_outfileId ${lda_outfileId} --numThreads ${NUM_THREADS} --batchSize ${BATCH_SIZE} --vi_iter_per_batch ${VI_ITER_PER_BATCH}| tee ${OUTPUT_DIR}/tmp_gammapdb_${LDA_VARIANT}_profile_${lda_outfileId}.txt

   cat ${OUTPUT_DIR}/tmp_gammapdb_${LDA_VARIANT}_profile_${lda_outfileId}.txt | grep -E "^#### " | sed -E "s/^#### /${i},/g" >> ${OUTPUT_DIR}/csv/${DATASET_NAME}_train_${NUM_TOPICS}topics_batchS${BATCH_SIZE}_totalNumIter${NUM_ITERATIONS}_NT${NUM_THREADS}_RND${RND_SEED}_gammapdb_${LDA_VARIANT}_time.csv

   rm -f ${OUTPUT_DIR}/tmp_gammapdb_${LDA_VARIANT}_profile_${lda_outfileId}.txt
done


if [ "$SKIP_TRAINING" == "FALSE" ]; then
  echo "Training LDA model ..."
  ${BUILDDIR_ABS_PATH}/gammapdb_arrow --command svi --lda_datasetName ${DATASET_NAME} --lda_numDocuments ${TRAININGSET_NUMDOCS} --lda_vocabSize ${TRAININGSET_VOCABSIZE} --lda_corpusSize ${TRAININGSET_NUMTOKENS} --lda_alpha ${ALPHA_PRIOR} --lda_beta ${BETA_PRIOR} --lda_numTopics ${NUM_TOPICS} --total_num_iter ${NUM_ITERATIONS} --rndSeed ${RND_SEED} --reportEvery ${SAVE_EVERY} --outputDir ${OUTPUT_DIR} --lda_vi_dataset_dir ${DATADIR_ABS_PATH}/${DATASET_NAME}_train --lda_malletStateDir ${OUTPUT_DIR}/gammapdb_data/${LDA_VARIANT}/chain_states --lda_outfileId ${lda_outfileId}  --numThreads ${NUM_THREADS} --batchSize ${BATCH_SIZE} --vi_iter_per_batch ${VI_ITER_PER_BATCH}
fi


############## PARALLEL COMPRESSION OF THE STATES GENERATED BY GAMMAPDB ##############
for i in $(seq ${SAVE_EVERY} ${SAVE_EVERY} ${TOTAL_ITER})
do
echo "compressing ${OUTPUT_DIR}/gammapdb_data/${LDA_VARIANT}/chain_states/chain-state_${lda_outfileId}.txt.${i}"
gzip -c  ${OUTPUT_DIR}/gammapdb_data/${LDA_VARIANT}/chain_states/chain-state_${lda_outfileId}.txt.${i} >  ${OUTPUT_DIR}/gammapdb_data/${LDA_VARIANT}/chain_states/chain-state_${lda_outfileId}.gz.${i} && rm ${OUTPUT_DIR}/gammapdb_data/${LDA_VARIANT}/chain_states/chain-state_${lda_outfileId}.txt.${i} &
done

wait
######################### MODEL EVALUATION ######################### 

if [ "$PARALLEL_PERP_COMP" == "TRUE" ]; then
  echo "Evaluating the LDA model with PARALLEL_PERP_COMP==TRUE ..."

  startEvaluatorCreation=`date +%s.%N`
  for i in $(seq ${SAVE_EVERY} ${SAVE_EVERY} ${TOTAL_ITER})
  do
    echo "Processing ${OUTPUT_DIR}/gammapdb_data/${LDA_VARIANT}/chain_states/chain-state_${lda_outfileId}.gz.${i}"

    ${MALLET_DIR}/bin/mallet train-topics  --input ${TRAININGSET_DIR}/training.mallet --input-state ${OUTPUT_DIR}/gammapdb_data/${LDA_VARIANT}/chain_states/chain-state_${lda_outfileId}.gz.${i} --no-inference --evaluator-filename ${OUTPUT_DIR}/gammapdb_data/${LDA_VARIANT}/evaluators/evaluator_${lda_outfileId}.${i} --output-topic-keys ${OUTPUT_DIR}/gammapdb_data/${LDA_VARIANT}/topics_defs/topics-def_${lda_outfileId}.${i}.txt --random-seed ${RND_SEED} 
  done
  # wait
  endEvaluatorCreation=`date +%s`
  runtimeEvaluatorCreation=$( echo "$endEvaluatorCreation - $startEvaluatorCreation" | bc -l )
  echo "creating the evaluators parallel took: ${runtimeEvaluatorCreation} ms"


  startEval=`date +%s.%N`
  for i in $(seq ${SAVE_EVERY} ${SAVE_EVERY} ${TOTAL_ITER})
  do
    echo "Evaluating ${OUTPUT_DIR}/gammapdb_data/${LDA_VARIANT}/chain_states/chain-state_${lda_outfileId}.gz.${i}"

    printf "%i," $i >> ${OUTPUT_DIR}/csv/${DATASET_NAME}_TEMP_test_${NUM_TOPICS}topics_batchS${BATCH_SIZE}_totalNumIter${NUM_ITERATIONS}_NT${NUM_THREADS}_RND${RND_SEED}_gammapdb_${LDA_VARIANT}_loglik.{$i}.csv 

    ${MALLET_DIR}/bin/mallet evaluate-topics --input ${TESTSET_DIR}/test.mallet --evaluator ${OUTPUT_DIR}/gammapdb_data/${LDA_VARIANT}/evaluators/evaluator_${lda_outfileId}.${i} --use-resampling FALSE --random-seed ${RND_SEED} | tee -a ${OUTPUT_DIR}/csv/${DATASET_NAME}_TEMP_test_${NUM_TOPICS}topics_batchS${BATCH_SIZE}_totalNumIter${NUM_ITERATIONS}_NT${NUM_THREADS}_RND${RND_SEED}_gammapdb_${LDA_VARIANT}_loglik.{$i}.csv 



    printf "%i," $i >> ${OUTPUT_DIR}/csv/${DATASET_NAME}_TEMP_train_${NUM_TOPICS}topics_batchS${BATCH_SIZE}_totalNumIter${NUM_ITERATIONS}_NT${NUM_THREADS}_RND${RND_SEED}_gammapdb_${LDA_VARIANT}_loglik.{$i}.csv 

    ${MALLET_DIR}/bin/mallet evaluate-topics --input ${TRAININGSET_DIR}/training.mallet --evaluator ${OUTPUT_DIR}/gammapdb_data/${LDA_VARIANT}/evaluators/evaluator_${lda_outfileId}.${i} --use-resampling FALSE --random-seed ${RND_SEED} | tee -a  ${OUTPUT_DIR}/csv/${DATASET_NAME}_TEMP_train_${NUM_TOPICS}topics_batchS${BATCH_SIZE}_totalNumIter${NUM_ITERATIONS}_NT${NUM_THREADS}_RND${RND_SEED}_gammapdb_${LDA_VARIANT}_loglik.{$i}.csv 
    
  done
  # wait
  echo "Evaluation parallel is done"
  python3 ${SCRIPTSDIR_ABS_PATH}/mergeLoglikFiles.py  --OUTPUT_DIR ${OUTPUT_DIR} --DATASET_NAME ${DATASET_NAME}  --TRAIN_OR_TEST train --NUM_TOPICS ${NUM_TOPICS} --ALPHA_PRIOR ${ALPHA_PRIOR} --BETA_PRIOR ${BETA_PRIOR} --NUM_ITERATIONS ${NUM_ITERATIONS} --NUM_THREADS ${NUM_THREADS} --RND_SEED ${RND_SEED} --MALLET_OR_GAMMAPDB gammapdb_${LDA_VARIANT} --BATCH_SIZE ${BATCH_SIZE} --VI_ITER_PER_BATCH ${VI_ITER_PER_BATCH}


  python3 ${SCRIPTSDIR_ABS_PATH}/mergeLoglikFiles.py  --OUTPUT_DIR ${OUTPUT_DIR} --DATASET_NAME ${DATASET_NAME}  --TRAIN_OR_TEST test --NUM_TOPICS ${NUM_TOPICS} --ALPHA_PRIOR ${ALPHA_PRIOR} --BETA_PRIOR ${BETA_PRIOR} --NUM_ITERATIONS ${NUM_ITERATIONS} --NUM_THREADS ${NUM_THREADS} --RND_SEED ${RND_SEED} --MALLET_OR_GAMMAPDB gammapdb_${LDA_VARIANT} --BATCH_SIZE ${BATCH_SIZE} --VI_ITER_PER_BATCH ${VI_ITER_PER_BATCH}

  rm  ${OUTPUT_DIR}/csv/${DATASET_NAME}_TEMP*gammapdb*


  endeval=`date +%s`
  runtimeEval=$( echo "$endeval - $startEval" | bc -l )
  echo "evaluate-topics parallel took ${runtimeEval} ms"

else

  echo "Evaluating the LDA model with PARALLEL_PERP_COMP==FALSE ..."
startEvaluatorCreation=`date +%s.%N`
  for i in $(seq ${SAVE_EVERY} ${SAVE_EVERY} ${TOTAL_ITER})
  do
    echo "Processing ${OUTPUT_DIR}/gammapdb_data/${LDA_VARIANT}/chain_states/chain-state_${lda_outfileId}.gz.${i}"
    ${MALLET_DIR}/bin/mallet train-topics  --input ${TRAININGSET_DIR}/training.mallet --input-state ${OUTPUT_DIR}/gammapdb_data/${LDA_VARIANT}/chain_states/chain-state_${lda_outfileId}.gz.${i} --no-inference --evaluator-filename ${OUTPUT_DIR}/gammapdb_data/${LDA_VARIANT}/evaluators/evaluator_${lda_outfileId}.${i} --output-topic-keys ${OUTPUT_DIR}/gammapdb_data/${LDA_VARIANT}/topics_defs/topics-def_${lda_outfileId}.${i}.txt --random-seed ${RND_SEED}
  done

  endEvaluatorCreation=`date +%s`
  runtimeEvaluatorCreation=$( echo "$endEvaluatorCreation - $startEvaluatorCreation" | bc -l )
  echo "creating the evaluators seq took: ${runtimeEvaluatorCreation} ms"


  startEval=`date +%s.%N`
  for i in $(seq ${SAVE_EVERY} ${SAVE_EVERY} ${TOTAL_ITER})
  do
    echo "Evaluating ${OUTPUT_DIR}/gammapdb_data/${LDA_VARIANT}/chain_states/chain-state_${lda_outfileId}.gz.${i}"
    printf "%i," $i >> ${OUTPUT_DIR}/csv/${DATASET_NAME}_test_${NUM_TOPICS}topics_batchS${BATCH_SIZE}_totalNumIter${NUM_ITERATIONS}_NT${NUM_THREADS}_RND${RND_SEED}_gammapdb_${LDA_VARIANT}_ViIterBatch${VI_ITER_PER_BATCH}_loglik.csv
    ${MALLET_DIR}/bin/mallet evaluate-topics --input ${TESTSET_DIR}/test.mallet --evaluator ${OUTPUT_DIR}/gammapdb_data/${LDA_VARIANT}/evaluators/evaluator_${lda_outfileId}.${i} --use-resampling FALSE --random-seed ${RND_SEED} | tee -a ${OUTPUT_DIR}/csv/${DATASET_NAME}_test_${NUM_TOPICS}topics_batchS${BATCH_SIZE}_totalNumIter${NUM_ITERATIONS}_NT${NUM_THREADS}_RND${RND_SEED}_gammapdb_${LDA_VARIANT}_ViIterBatch${VI_ITER_PER_BATCH}_loglik.csv
    printf "%i," $i >> ${OUTPUT_DIR}/csv/${DATASET_NAME}_train_${NUM_TOPICS}topics_batchS${BATCH_SIZE}_totalNumIter${NUM_ITERATIONS}_NT${NUM_THREADS}_RND${RND_SEED}_gammapdb_${LDA_VARIANT}_ViIterBatch${VI_ITER_PER_BATCH}_loglik.csv
    ${MALLET_DIR}/bin/mallet evaluate-topics --input ${TRAININGSET_DIR}/training.mallet --evaluator ${OUTPUT_DIR}/gammapdb_data/${LDA_VARIANT}/evaluators/evaluator_${lda_outfileId}.${i} --use-resampling FALSE --random-seed ${RND_SEED} | tee -a ${OUTPUT_DIR}/csv/${DATASET_NAME}_train_${NUM_TOPICS}topics_batchS${BATCH_SIZE}_totalNumIter${NUM_ITERATIONS}_NT${NUM_THREADS}_RND${RND_SEED}_gammapdb_${LDA_VARIANT}_ViIterBatch${VI_ITER_PER_BATCH}_loglik.csv
  done

  endeval=`date +%s`
  runtimeEval=$( echo "$endeval - $startEval" | bc -l )
  echo "evaluate-topics seq took ${runtimeEval} ms"
fi

#computing perplexity for testset
python3 ${SCRIPTSDIR_ABS_PATH}/loglik2perp.py --srcFile ${OUTPUT_DIR}/csv/${DATASET_NAME}_test_${NUM_TOPICS}topics_batchS${BATCH_SIZE}_totalNumIter${NUM_ITERATIONS}_NT${NUM_THREADS}_RND${RND_SEED}_gammapdb_${LDA_VARIANT}_ViIterBatch${VI_ITER_PER_BATCH}_loglik.csv --outFile ${OUTPUT_DIR}/csv/${DATASET_NAME}_test_${NUM_TOPICS}topics_batchS${BATCH_SIZE}_totalNumIter${NUM_ITERATIONS}_NT${NUM_THREADS}_RND${RND_SEED}_gammapdb_${LDA_VARIANT}_ViIterBatch${VI_ITER_PER_BATCH}_perplexity.csv --numTokens ${TESTSET_NUMTOKENS}

# computing perplexity for trainingset
python3 ${SCRIPTSDIR_ABS_PATH}/loglik2perp.py --srcFile ${OUTPUT_DIR}/csv/${DATASET_NAME}_train_${NUM_TOPICS}topics_batchS${BATCH_SIZE}_totalNumIter${NUM_ITERATIONS}_NT${NUM_THREADS}_RND${RND_SEED}_gammapdb_${LDA_VARIANT}_ViIterBatch${VI_ITER_PER_BATCH}_loglik.csv --outFile ${OUTPUT_DIR}/csv/${DATASET_NAME}_train_${NUM_TOPICS}topics_batchS${BATCH_SIZE}_totalNumIter${NUM_ITERATIONS}_NT${NUM_THREADS}_RND${RND_SEED}_gammapdb_${LDA_VARIANT}_ViIterBatch${VI_ITER_PER_BATCH}_perplexity.csv --numTokens ${TRAININGSET_NUMTOKENS}

# # generating plot data for testset
python3 ${SCRIPTSDIR_ABS_PATH}/genplotdata.py --timingFile ${OUTPUT_DIR}/csv/${DATASET_NAME}_train_${NUM_TOPICS}topics_batchS${BATCH_SIZE}_totalNumIter${NUM_ITERATIONS}_NT${NUM_THREADS}_RND${RND_SEED}_gammapdb_${LDA_VARIANT}_time.csv --perplexityFile ${OUTPUT_DIR}/csv/${DATASET_NAME}_test_${NUM_TOPICS}topics_batchS${BATCH_SIZE}_totalNumIter${NUM_ITERATIONS}_NT${NUM_THREADS}_RND${RND_SEED}_gammapdb_${LDA_VARIANT}_ViIterBatch${VI_ITER_PER_BATCH}_perplexity.csv --outFile ${OUTPUT_DIR}/csv/${DATASET_NAME}_test_${NUM_TOPICS}topics_batchS${BATCH_SIZE}_totalNumIter${NUM_ITERATIONS}_NT${NUM_THREADS}_RND${RND_SEED}_gammapdb_${LDA_VARIANT}_ViIterBatch${VI_ITER_PER_BATCH}_plotdata.csv

# generating plot data for trainingset
python3 ${SCRIPTSDIR_ABS_PATH}/genplotdata.py --timingFile ${OUTPUT_DIR}/csv/${DATASET_NAME}_train_${NUM_TOPICS}topics_batchS${BATCH_SIZE}_totalNumIter${NUM_ITERATIONS}_NT${NUM_THREADS}_RND${RND_SEED}_gammapdb_${LDA_VARIANT}_time.csv --perplexityFile ${OUTPUT_DIR}/csv/${DATASET_NAME}_train_${NUM_TOPICS}topics_batchS${BATCH_SIZE}_totalNumIter${NUM_ITERATIONS}_NT${NUM_THREADS}_RND${RND_SEED}_gammapdb_${LDA_VARIANT}_ViIterBatch${VI_ITER_PER_BATCH}_perplexity.csv --outFile ${OUTPUT_DIR}/csv/${DATASET_NAME}_train_${NUM_TOPICS}topics_batchS${BATCH_SIZE}_totalNumIter${NUM_ITERATIONS}_NT${NUM_THREADS}_RND${RND_SEED}_gammapdb_${LDA_VARIANT}_ViIterBatch${VI_ITER_PER_BATCH}_plotdata.csv