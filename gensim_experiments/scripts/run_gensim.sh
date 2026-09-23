#!/bin/bash

# On failure, terminate the script immediately
set -e

# Cleanup function for any errors
cleanup() {
  echo "An error occurred!"
  exit 1
}
trap 'cleanup' ERR

# Determine the script's current absolute path
SCRIPTSDIR_ABS_PATH=$(readlink -f "${BASH_SOURCE[0]}")
SCRIPTSDIR_ABS_PATH=$(dirname "${SCRIPTSDIR_ABS_PATH}")

# Determine the project's root directory (parent of the script's directory)
PROJECT_ROOT_ABS_PATH=$(readlink -f "${SCRIPTSDIR_ABS_PATH}/../")

# Now make all other paths absolute based on the project's root directory
EXTERNALLIBS_ABS_PATH=$(readlink -f "${PROJECT_ROOT_ABS_PATH}/external")
PATCHESDIR_ABS_PATH=$(readlink -f "${PROJECT_ROOT_ABS_PATH}/patches")
BUILDDIR_ABS_PATH=$(readlink -f "${PROJECT_ROOT_ABS_PATH}/build")
DATADIR_ABS_PATH=$(readlink -f "${PROJECT_ROOT_ABS_PATH}/data")
BENCHMARKSDIR_ABS_PATH=$(readlink -f "${PROJECT_ROOT_ABS_PATH}/benchmarks")
SCRIPTS_ABS_PATH=$(readlink -f "${PROJECT_ROOT_ABS_PATH}/scripts")

# DATASET_NAME doesn't need to be an absolute path, so you can leave it as is
DATASET_NAME="KOS"
echo "--------------------------------------------------------------------------------"
# Echo the absolute paths for verification
echo "Project root directory: ${PROJECT_ROOT_ABS_PATH}"
echo "External libs directory: ${EXTERNALLIBS_ABS_PATH}"
echo "Patches directory: ${PATCHESDIR_ABS_PATH}"
echo "Build directory: ${BUILDDIR_ABS_PATH}"
echo "Data directory: ${DATADIR_ABS_PATH}"
echo "Benchmarks directory: ${BENCHMARKSDIR_ABS_PATH}"
echo "--------------------------------------------------------------------------------"

# Function to simulate a loading bar
loading_bar() {
  local duration=30
  echo -n "Loading ["
  for i in $(seq 1 $duration); do
    sleep 0.1
    echo -n "="
  done
  echo "]"
}

# Load necessary software collections (SCL)
# source /opt/rh/devtoolset-11/enable
# source /opt/rh/llvm-toolset-11.0/enable
# source /opt/rh/rh-python36/enable
# source /opt/rh/rh-git29/enable

# # Check if yum is available
# if ! command -v yum &> /dev/null; then
#     echo "Error: yum is not available on this system."
#     exit 1
# fi

# Check if the Mallet directory exists
if [ ! -d "${EXTERNALLIBS_ABS_PATH}/mallet/Mallet" ]; then
    # Create the necessary directories
    mkdir -p ${EXTERNALLIBS_ABS_PATH}/mallet/Mallet
    
    # Clone the Mallet repository
    git clone --branch v202108 https://github.com/mimno/Mallet.git ${EXTERNALLIBS_ABS_PATH}/mallet/Mallet
    
    # Apply the patch
    git apply --unsafe-paths --directory=${EXTERNALLIBS_ABS_PATH}/mallet/Mallet ${PATCHESDIR_ABS_PATH}/mallet.patch
    
    # Build Mallet using Ant
    ant -buildfile ${EXTERNALLIBS_ABS_PATH}/mallet/Mallet/build.xml
else
    echo "Compiling Mallet... Nothing to do."
fi

# Set environment variables
export MALLET_MEMORY=16g

# Configuration variables
MALLET_DIR=${EXTERNALLIBS_ABS_PATH}/mallet/Mallet
OUTPUT_DIR=${BENCHMARKSDIR_ABS_PATH}
DATASET_NAME="KOS"
NUM_TOPICS=20
ALPHA_PRIOR=0.2
BETA_PRIOR=0.1
NUM_ITERATIONS=20
NUM_RUNS=1
SAVE_EVERY=2
RND_SEED=123
SKIP_TRAINING="FALSE"
NUM_THREADS=1

# Parse command-line arguments for flexibility
while [[ $# -gt 0 ]]; do
  key="$1"
  case $key in
    -m|--malletDir)
      MALLET_DIR="$2"
      shift; shift ;;
    -o|--outputDir)
      OUTPUT_DIR="$2"
      shift; shift ;;
    -d|--datasetName)
      DATASET_NAME="$2"
      shift; shift ;;
    -t|--trainingSetDir)
      TRAININGSET_DIR="$2"
      shift; shift ;;
    -e|--testSetDir)
      TESTSET_DIR="$2"
      shift; shift ;;
    -k|--numTopics)
      NUM_TOPICS="$2"
      shift; shift ;;
    -a|--alpha)
      ALPHA_PRIOR="$2"
      shift; shift ;;
    -b|--beta)
      BETA_PRIOR="$2"
      shift; shift ;;
    -n|--numIterations)
      NUM_ITERATIONS="$2"
      shift; shift ;;
    --numRuns)
      NUM_RUNS="$2"
      shift; shift ;;
    -s|--saveEvery)
      SAVE_EVERY="$2"
      shift; shift ;;
    -r|--rndSeed)
      RND_SEED="$2"
      shift; shift ;;
    --numThreads)
      NUM_THREADS="$2"
      shift; shift ;;
    --skipTraining)
      SKIP_TRAINING="TRUE"
      shift ;;
    *) POSITIONAL+=("$1"); shift ;;
  esac
done

set -- "${POSITIONAL[@]}" # restore positional parameters

# Paths for chain state and timing
CHAIN_STATE_FILEPATH=${PROJECT_ROOT_ABS_PATH}/benchmarks/gensim/chain_states/
TIME_FILEPATH=${OUTPUT_DIR}/csv/${DATASET_NAME}_train_${NUM_TOPICS}topics_A${ALPHA_PRIOR}_B${BETA_PRIOR}_NI${NUM_ITERATIONS}_NT${NUM_THREADS}_RND${RND_SEED}_gensim_time.csv

# Create necessary directories
echo "--------------------------------------------------------------------------------"
mkdir -p ${OUTPUT_DIR}/gensim/chain_states && echo "Created directory: ${OUTPUT_DIR}/gensim/chain_states"
mkdir -p ${OUTPUT_DIR}/gensim/evaluators && echo "Created directory: ${OUTPUT_DIR}/gensim/evaluators"
mkdir -p ${OUTPUT_DIR}/gensim/topics_defs && echo "Created directory: ${OUTPUT_DIR}/gensim/topics_defs"
mkdir -p ${OUTPUT_DIR}/gensim/csv && echo "Created directory: ${OUTPUT_DIR}/gensim/csv"
mkdir -p ${OUTPUT_DIR}/csv && echo "Created directory: ${OUTPUT_DIR}/csv"
echo "--------------------------------------------------------------------------------"

# Cleanup existing generated files
for i in $(seq ${SAVE_EVERY} ${SAVE_EVERY} ${NUM_ITERATIONS}); do
  rm -f ${OUTPUT_DIR}/gensim/chain_states/chain-state_${DATASET_NAME}_${NUM_TOPICS}topics_A${ALPHA_PRIOR}_B${BETA_PRIOR}_NI${NUM_ITERATIONS}_NT${NUM_THREADS}_RND${RND_SEED}.gz.${i}
  rm -f ${OUTPUT_DIR}/gensim/evaluators/evaluator_${DATASET_NAME}_${NUM_TOPICS}topics_A${ALPHA_PRIOR}_B${BETA_PRIOR}_NI${NUM_ITERATIONS}_NT${NUM_THREADS}_RND${RND_SEED}.${i}
done

# Clean generated CSV files
rm -f ${OUTPUT_DIR}/csv/${DATASET_NAME}_test_*_gensim_loglik.csv
rm -f ${OUTPUT_DIR}/csv/${DATASET_NAME}_train_*_gensim_loglik.csv
rm -f ${OUTPUT_DIR}/csv/${DATASET_NAME}_*_gensim_perplexity.csv
rm -f ${OUTPUT_DIR}/csv/${DATASET_NAME}_*_gensim_plotdata.csv
echo "Generated files removed."

# Define dataset paths and stats
TRAININGSET_DIR=${DATADIR_ABS_PATH}/${DATASET_NAME}_train/mallet
TESTSET_DIR=${DATADIR_ABS_PATH}/${DATASET_NAME}_test/mallet
TRAININGSET_NUMTOKENS=$(echo "scale=4; $(cat ${DATADIR_ABS_PATH}/${DATASET_NAME}_train/csv2/${DATASET_NAME}_train.csv | wc -l)-2" | bc)
TESTSET_NUMTOKENS=$(echo "scale=4; $(cat ${DATADIR_ABS_PATH}/${DATASET_NAME}_test/csv2/${DATASET_NAME}_test.csv | wc -l)-2" | bc)
TRAININGSET_NUMDOCS=$(echo "scale=4; $(cat ${DATADIR_ABS_PATH}/${DATASET_NAME}_train/tsv/training.tsv | wc -l)-0" | bc)
TESTSET_NUMDOCS=$(echo "scale=4; $(cat ${DATADIR_ABS_PATH}/${DATASET_NAME}_test/tsv/test.tsv | wc -l)-0" | bc)
TRAININGSET_VOCABSIZE=$(echo "scale=4; $(cat ${DATADIR_ABS_PATH}/${DATASET_NAME}_train/csv2/${DATASET_NAME}_vocab.csv | wc -l)-1" | bc)
TESTSET_VOCABSIZE=$(echo "scale=4; $(cat ${DATADIR_ABS_PATH}/${DATASET_NAME}_test/csv2/${DATASET_NAME}_vocab.csv | wc -l)-1" | bc)

# Display configuration
echo "--------------------------------------------------------------------------------------------------------------------------------------"
printf "| %-40s | %-80s |\n" "Variable" "Value"
echo "--------------------------------------------------------------------------------------------------------------------------------------"
printf "| %-40s | %-80s |\n" "DATASET_NAME" "${DATASET_NAME}"
printf "| %-40s | %-80s |\n" "NUM_TOPICS" "${NUM_TOPICS}"
printf "| %-40s | %-80s |\n" "ALPHA_PRIOR" "${ALPHA_PRIOR}"
printf "| %-40s | %-80s |\n" "BETA_PRIOR" "${BETA_PRIOR}"
printf "| %-40s | %-80s |\n" "NUM_ITERATIONS" "${NUM_ITERATIONS}"
printf "| %-40s | %-80s |\n" "NUM_THREADS" "${NUM_THREADS}"
printf "| %-40s | %-80s |\n" "RND_SEED" "${RND_SEED}"
printf "| %-40s | %-80s |\n" "OUTPUT_DIR" "${OUTPUT_DIR}"
printf "| %-40s | %-80s |\n" "TESTSET_DIR" "${TESTSET_DIR}"
printf "| %-40s | %-80s |\n" "TRAININGSET_DIR" "${TRAININGSET_DIR}"
printf "| %-40s | %-80s |\n" "SAVE_EVERY" "${SAVE_EVERY}"
printf "| %-40s | %-80s |\n" "MALLET_DIR" "${MALLET_DIR}"
echo "--------------------------------------------------------------------------------------------------------------------------------------"

# Run the LDA model with profiling
PROFILING="True"
python3 ${SCRIPTS_ABS_PATH}/gensim_lda_test.py \
    --dataset_name ${DATASET_NAME} \
    --num_topics ${NUM_TOPICS} \
    --alpha_prior ${ALPHA_PRIOR} \
    --beta_prior ${BETA_PRIOR} \
    --num_iterations ${NUM_ITERATIONS} \
    --num_threads ${NUM_THREADS} \
    --rnd_seed ${RND_SEED} \
    --SAVE_EVERY ${SAVE_EVERY} \
    --chain_state_filepath ${CHAIN_STATE_FILEPATH} \
    --time_filepath ${TIME_FILEPATH} \
    --profiling ${PROFILING}

# After profiling, run training with the final configuration
# PROFILING="False"
# python3 gensim_lda_test.py \
#     --dataset_name ${DATASET_NAME} \
#     --num_topics ${NUM_TOPICS} \
#     --alpha_prior ${ALPHA_PRIOR} \
#     --beta_prior ${BETA_PRIOR} \
#     --num_iterations ${NUM_ITERATIONS} \
#     --num_threads ${NUM_THREADS} \
#     --rnd_seed ${RND_SEED} \
#     --SAVE_EVERY ${SAVE_EVERY} \
#     --chain_state_filepath ${CHAIN_STATE_FILEPATH} \
#     --time_filepath ${TIME_FILEPATH} \
#     --profiling ${PROFILING}

# Evaluate LDA model
for i in $(seq ${SAVE_EVERY} ${SAVE_EVERY} ${NUM_ITERATIONS})
do
   echo "Processing chain-state for iteration ${i}"
   ${MALLET_DIR}/bin/mallet train-topics --input ${TRAININGSET_DIR}/training.mallet \
       --input-state ${OUTPUT_DIR}/gensim/chain_states/chain-state_${DATASET_NAME}_${NUM_TOPICS}topics_A${ALPHA_PRIOR}_B${BETA_PRIOR}_NI${NUM_ITERATIONS}_NT${NUM_THREADS}_RND${RND_SEED}.gz.${i} \
       --no-inference \
       --evaluator-filename ${OUTPUT_DIR}/gensim/evaluators/evaluator_${DATASET_NAME}_${NUM_TOPICS}topics_A${ALPHA_PRIOR}_B${BETA_PRIOR}_NI${NUM_ITERATIONS}_NT${NUM_THREADS}_RND${RND_SEED}.${i} \
       --output-topic-keys ${OUTPUT_DIR}/gensim/topics_defs/topics-def_${DATASET_NAME}_${NUM_TOPICS}topics_A${ALPHA_PRIOR}_B${BETA_PRIOR}_NI${NUM_ITERATIONS}_NT${NUM_THREADS}_RND${RND_SEED}.${i}.txt \
       --random-seed ${RND_SEED}
done

# Compute log-likelihood and perplexity
for i in $(seq ${SAVE_EVERY} ${SAVE_EVERY} ${NUM_ITERATIONS})
do
   echo "Evaluating log-likelihood for iteration ${i}"
   # Evaluate test set log-likelihood
   printf "%i," $i >> ${OUTPUT_DIR}/csv/${DATASET_NAME}_test_${NUM_TOPICS}topics_A${ALPHA_PRIOR}_B${BETA_PRIOR}_NI${NUM_ITERATIONS}_NT${NUM_THREADS}_RND${RND_SEED}_gensim_loglik.csv
   ${MALLET_DIR}/bin/mallet evaluate-topics --input ${TESTSET_DIR}/test.mallet \
       --evaluator ${OUTPUT_DIR}/gensim/evaluators/evaluator_${DATASET_NAME}_${NUM_TOPICS}topics_A${ALPHA_PRIOR}_B${BETA_PRIOR}_NI${NUM_ITERATIONS}_NT${NUM_THREADS}_RND${RND_SEED}.${i} \
       --use-resampling FALSE --random-seed ${RND_SEED} | tee -a ${OUTPUT_DIR}/csv/${DATASET_NAME}_test_${NUM_TOPICS}topics_A${ALPHA_PRIOR}_B${BETA_PRIOR}_NI${NUM_ITERATIONS}_NT${NUM_THREADS}_RND${RND_SEED}_gensim_loglik.csv
   
   # Evaluate training set log-likelihood
   printf "%i," $i >> ${OUTPUT_DIR}/csv/${DATASET_NAME}_train_${NUM_TOPICS}topics_A${ALPHA_PRIOR}_B${BETA_PRIOR}_NI${NUM_ITERATIONS}_NT${NUM_THREADS}_RND${RND_SEED}_gensim_loglik.csv
   ${MALLET_DIR}/bin/mallet evaluate-topics --input ${TRAININGSET_DIR}/training.mallet \
       --evaluator ${OUTPUT_DIR}/gensim/evaluators/evaluator_${DATASET_NAME}_${NUM_TOPICS}topics_A${ALPHA_PRIOR}_B${BETA_PRIOR}_NI${NUM_ITERATIONS}_NT${NUM_THREADS}_RND${RND_SEED}.${i} \
       --use-resampling FALSE --random-seed ${RND_SEED} | tee -a ${OUTPUT_DIR}/csv/${DATASET_NAME}_train_${NUM_TOPICS}topics_A${ALPHA_PRIOR}_B${BETA_PRIOR}_NI${NUM_ITERATIONS}_NT${NUM_THREADS}_RND${RND_SEED}_gensim_loglik.csv
done

# Compute perplexity for test and train sets
python3 ${SCRIPTS_ABS_PATH}/loglik2perp.py --srcFile ${OUTPUT_DIR}/csv/${DATASET_NAME}_test_${NUM_TOPICS}topics_A${ALPHA_PRIOR}_B${BETA_PRIOR}_NI${NUM_ITERATIONS}_NT${NUM_THREADS}_RND${RND_SEED}_gensim_loglik.csv \
    --outFile ${OUTPUT_DIR}/csv/${DATASET_NAME}_test_${NUM_TOPICS}topics_A${ALPHA_PRIOR}_B${BETA_PRIOR}_NI${NUM_ITERATIONS}_NT${NUM_THREADS}_RND${RND_SEED}_gensim_perplexity.csv \
    --numTokens ${TESTSET_NUMTOKENS}

python3 ${SCRIPTS_ABS_PATH}/loglik2perp.py --srcFile ${OUTPUT_DIR}/csv/${DATASET_NAME}_train_${NUM_TOPICS}topics_A${ALPHA_PRIOR}_B${BETA_PRIOR}_NI${NUM_ITERATIONS}_NT${NUM_THREADS}_RND${RND_SEED}_gensim_loglik.csv \
    --outFile ${OUTPUT_DIR}/csv/${DATASET_NAME}_train_${NUM_TOPICS}topics_A${ALPHA_PRIOR}_B${BETA_PRIOR}_NI${NUM_ITERATIONS}_NT${NUM_THREADS}_RND${RND_SEED}_gensim_perplexity.csv \
    --numTokens ${TRAININGSET_NUMTOKENS}

# Generate plot data
python3 ${SCRIPTS_ABS_PATH}/genplotdata.py --timingFile ${OUTPUT_DIR}/csv/${DATASET_NAME}_train_${NUM_TOPICS}topics_A${ALPHA_PRIOR}_B${BETA_PRIOR}_NI${NUM_ITERATIONS}_NT${NUM_THREADS}_RND${RND_SEED}_gensim_time.csv \
    --perplexityFile ${OUTPUT_DIR}/csv/${DATASET_NAME}_test_${NUM_TOPICS}topics_A${ALPHA_PRIOR}_B${BETA_PRIOR}_NI${NUM_ITERATIONS}_NT${NUM_THREADS}_RND${RND_SEED}_gensim_perplexity.csv \
    --outFile ${OUTPUT_DIR}/csv/${DATASET_NAME}_test_${NUM_TOPICS}topics_A${ALPHA_PRIOR}_B${BETA_PRIOR}_NI${NUM_ITERATIONS}_NT${NUM_THREADS}_RND${RND_SEED}_gensim_plotdata.csv

python3 ${SCRIPTS_ABS_PATH}/genplotdata.py --timingFile ${OUTPUT_DIR}/csv/${DATASET_NAME}_train_${NUM_TOPICS}topics_A${ALPHA_PRIOR}_B${BETA_PRIOR}_NI${NUM_ITERATIONS}_NT${NUM_THREADS}_RND${RND_SEED}_gensim_time.csv \
    --perplexityFile ${OUTPUT_DIR}/csv/${DATASET_NAME}_train_${NUM_TOPICS}topics_A${ALPHA_PRIOR}_B${BETA_PRIOR}_NI${NUM_ITERATIONS}_NT${NUM_THREADS}_RND${RND_SEED}_gensim_perplexity.csv \
    --outFile ${OUTPUT_DIR}/csv/${DATASET_NAME}_train_${NUM_TOPICS}topics_A${ALPHA_PRIOR}_B${BETA_PRIOR}_NI${NUM_ITERATIONS}_NT${NUM_THREADS}_RND${RND_SEED}_gensim_plotdata.csv

# Done
echo "LDA training and evaluation complete."
loading_bar