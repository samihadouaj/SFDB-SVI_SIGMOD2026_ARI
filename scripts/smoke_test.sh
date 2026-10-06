#!/bin/bash

# Smoke test: runs the four approaches of Table 1 (SFDB-SVI, Mallet, SFDB-CGS, C++SVI) on the
# small KOS dataset with 20 topics, through the same run_*.sh scripts as the real experiments,
# and checks that every run completes and produces well-formed results. 

# Requires the KOS dataset (get_uci_datasets.sh) and the builds (build_all.sh).
# Everything is written to benchmarks/smoketest/<date>_<time>/, one sub-directory per approach;
# benchmarks/csv, logs/ and the other experiment outputs are left untouched.
#
# Usage (in the container): bash /gammapdb_arrow/scripts/smoke_test.sh

set -o pipefail

# Paths
SCRIPTSDIR_ABS_PATH=$(dirname $(readlink -f ${BASH_SOURCE[0]}))
PROJECT_ROOT_ABS_PATH=$(readlink -f ${SCRIPTSDIR_ABS_PATH}/..)
CONFDIR_ABS_PATH=${PROJECT_ROOT_ABS_PATH}/conf
DATADIR_ABS_PATH=${PROJECT_ROOT_ABS_PATH}/data
MALLET_DIR=${PROJECT_ROOT_ABS_PATH}/extras/mallet/Mallet
SMOKETEST_DIR=${PROJECT_ROOT_ABS_PATH}/benchmarks/smoketest/$(date +%Y%m%d_%H%M%S)

# Relocate <root>/data, if necessary (as the run_*.sh scripts do)
if [ $(grep -c "^[^#]" ${CONFDIR_ABS_PATH}/datadir_redirect.txt) != "0" ]; then
  DATADIR_ABS_PATH=$(readlink -f $(grep "^[^#]" ${CONFDIR_ABS_PATH}/datadir_redirect.txt | head -n 1))
fi

# Use the GCC 11 toolchain (installed by get_deps.sh), as main_script.sh does
[ -f /opt/rh/devtoolset-11/enable ] && source /opt/rh/devtoolset-11/enable

#########################################################
################ KOS, 20 topics #########################
#########################################################
# 30 checkpoints per run (40 for Mallet). KOS has 3268 training documents and 337020
# distinct (document, word) pairs: the SVI batch sizes below split them into 15 minibatches,
# so 30 SVI iterations go over the training data twice.
DATASET=KOS
NUM_TOPICS=20
ALPHA_PRIOR=0.2
BETA_PRIOR=0.1
RND_SEED=123
NUM_THREADS=24
PARALLEL_PERP_COMP=TRUE

# SFDB-SVI (batch size in (document, word) pairs)
SVI_NUM_ITERATIONS=30
SVI_SAVE_EVERY=1
SVI_NUM_RUNS=2
SVI_BATCH_SIZE=22468
SVI_VI_ITER_PER_BATCH=3

# Mallet
MALLET_NUM_ITERATIONS=40
MALLET_SAVE_EVERY=1
MALLET_NUM_RUNS=2

# SFDB-CGS (iteration 1 is the random initialization, which the engine does not save,
# so it needs SAVE_EVERY >= 2, as in the real experiments)
CGS_NUM_ITERATIONS=60
CGS_SAVE_EVERY=2
CGS_NUM_RUNS=2

# C++SVI (batch size in documents)
CXX_NUM_ITERATIONS=30
CXX_SAVE_EVERY=1
CXX_NUM_RUNS=1
CXX_BATCH_SIZE=218
CXX_VI_ITER_PER_BATCH=4

# Check the prerequisites
missing=0
for f in ${PROJECT_ROOT_ABS_PATH}/build/gammapdb_arrow \
         ${PROJECT_ROOT_ABS_PATH}/cxx_svi/build/svi \
         ${MALLET_DIR}/bin/mallet \
         ${DATADIR_ABS_PATH}/${DATASET}_train/mallet/training.mallet \
         ${DATADIR_ABS_PATH}/${DATASET}_test/mallet/test.mallet \
         ${DATADIR_ABS_PATH}/${DATASET}_train/csv2/${DATASET}_train_svi.csv \
         ${DATADIR_ABS_PATH}/${DATASET}_train/cxx_svi/csv2/${DATASET}_train_svi.csv; do
  if [ ! -e "$f" ]; then echo "MISSING: $f"; missing=1; fi
done
if [ $missing -ne 0 ]; then
  echo "Run get_uci_datasets.sh and build_all.sh first."
  exit 1
fi

# Timings are part of the real experiments: do not run alongside them
if pgrep -f "build/gammapdb_arrow|cxx_svi/build/svi|cc.mallet" > /dev/null; then
  echo "WARNING: another StarfishDB, C++SVI or Mallet process is running; it will slow down both runs."
fi

mkdir -p ${SMOKETEST_DIR}
echo "Smoke test outputs: ${SMOKETEST_DIR}"

NAMES=()
STATUS=()
DURATIONS=()
TRAIN_PERPLEXITIES=()

# run <name> <expected checkpoints, e.g. "20 40 ... 200"> <script> <args...>
# Runs one approach in ${SMOKETEST_DIR}/<name>, then checks its outputs.
run() {
  local name=$1 checkpoints=$2 script=$3
  shift 3
  local outdir=${SMOKETEST_DIR}/${name}
  local log=${SMOKETEST_DIR}/log_${name}.txt
  mkdir -p ${outdir}

  echo "==================== ${name} ($(date)) ===================="
  local start=${SECONDS}
  bash ${SCRIPTSDIR_ABS_PATH}/${script} --outputDir ${outdir} "$@" 2>&1 | tee ${log}
  local rc=${PIPESTATUS[0]}
  local elapsed=$(( SECONDS - start ))

  # The run_*.sh scripts pipe their binaries into tee and run Mallet in the background,
  # so a crash does not always change their exit code: check the outputs as well.
  local errors=()
  [ $rc -ne 0 ] && errors+=("${script} exited with code ${rc}")

  local expected=$(echo ${checkpoints} | tr ' ' '\n')
  for split in test train; do
    local files=( ${outdir}/csv/${DATASET}_${split}_*_loglik.csv )
    if [ ${#files[@]} -ne 1 ] || [ ! -s "${files[0]}" ]; then
      errors+=("expected one non-empty ${split} _loglik.csv, found: ${files[*]}")
      continue
    fi
    local f=${files[0]}
    if [ "$(cut -d, -f1 ${f})" != "${expected}" ]; then
      errors+=("$(basename ${f}): checkpoints are [$(cut -d, -f1 ${f} | tr '\n' ' ')], expected [${checkpoints}]")
    fi
    if grep -qvE '^[0-9]+,-?[0-9.]+([eE][-+]?[0-9]+)?$' ${f}; then
      errors+=("$(basename ${f}): non-numeric log-likelihood (e.g. nan or inf)")
    fi
  done
  for kind in train_*_time test_*_plotdata train_*_plotdata; do
    local files=( ${outdir}/csv/${DATASET}_${kind}.csv )
    if [ ${#files[@]} -ne 1 ] || [ ! -s "${files[0]}" ]; then
      errors+=("expected one non-empty ${kind}.csv, found: ${files[*]}")
    fi
  done

  # Perplexity at the first and last checkpoint of the training-set plot data
  local plotdata=( ${outdir}/csv/${DATASET}_train_*_plotdata.csv )
  local train_perplexity="-"
  if [ ${#plotdata[@]} -eq 1 ] && [ -s "${plotdata[0]}" ]; then
    train_perplexity=$(awk -F, 'NR==1 {f=$2} {l=$2} END {printf "%.2f -> %.2f", f, l}' ${plotdata[0]})
  fi

  local suspicious=$(grep -nE "Exception|Error|error:|Segmentation fault|core dumped|Killed|Aborted|Traceback|No space left|No such file|command not found|Permission denied" ${log} | head -n 5)
  [ -n "${suspicious}" ] && errors+=("errors in ${log}:"$'\n'"${suspicious}")

  NAMES+=("${name}")
  DURATIONS+=("${elapsed}s")
  TRAIN_PERPLEXITIES+=("${train_perplexity}")
  if [ ${#errors[@]} -eq 0 ]; then
    STATUS+=("OK")
    echo ">>> ${name}: OK (${elapsed}s)"
  else
    STATUS+=("FAILED")
    echo ">>> ${name}: FAILED (${elapsed}s)"
    printf '    - %s\n' "${errors[@]}"
  fi
}

COMMON="--datasetName ${DATASET} --numTopics ${NUM_TOPICS} --alpha ${ALPHA_PRIOR} --beta ${BETA_PRIOR} --malletDir ${MALLET_DIR} --trainingSetDir ${DATADIR_ABS_PATH}/${DATASET}_train/mallet --testSetDir ${DATADIR_ABS_PATH}/${DATASET}_test/mallet --rndSeed ${RND_SEED} --numThreads ${NUM_THREADS} --PARALLEL_PERP_COMP ${PARALLEL_PERP_COMP}"

# SFDB-SVI
run sfdb_svi "$(seq -s ' ' ${SVI_SAVE_EVERY} ${SVI_SAVE_EVERY} ${SVI_NUM_ITERATIONS})" run_vi.sh ${COMMON} \
  --numIterations ${SVI_NUM_ITERATIONS} --saveEvery ${SVI_SAVE_EVERY} --numRuns ${SVI_NUM_RUNS} \
  --BATCH_SIZE ${SVI_BATCH_SIZE} --VI_ITER_PER_BATCH ${SVI_VI_ITER_PER_BATCH}

# Mallet
run mallet "$(seq -s ' ' ${MALLET_SAVE_EVERY} ${MALLET_SAVE_EVERY} ${MALLET_NUM_ITERATIONS})" run_mallet.sh ${COMMON} \
  --numIterations ${MALLET_NUM_ITERATIONS} --saveEvery ${MALLET_SAVE_EVERY} --numRuns ${MALLET_NUM_RUNS} \
  --CHUNK_SIZE ${MALLET_NUM_ITERATIONS}

# SFDB-CGS
run sfdb_cgs "$(seq -s ' ' ${CGS_SAVE_EVERY} ${CGS_SAVE_EVERY} ${CGS_NUM_ITERATIONS})" run_gammapdb_lda.sh ${COMMON} \
  --ldaVariant lda-inmemory-vrexprP \
  --numIterations ${CGS_NUM_ITERATIONS} --saveEvery ${CGS_SAVE_EVERY} --numRuns ${CGS_NUM_RUNS} \
  --CHUNK_SIZE ${CGS_NUM_ITERATIONS}

# C++SVI
run cxx_svi "$(seq -s ' ' ${CXX_SAVE_EVERY} ${CXX_SAVE_EVERY} ${CXX_NUM_ITERATIONS})" run_cxx_svi.sh ${COMMON} \
  --numIterations ${CXX_NUM_ITERATIONS} --saveEvery ${CXX_SAVE_EVERY} --numRuns ${CXX_NUM_RUNS} \
  --BATCH_SIZE ${CXX_BATCH_SIZE} --VI_ITER_PER_BATCH ${CXX_VI_ITER_PER_BATCH}

# Summary
echo
echo "==================== Smoke test summary (${DATASET}, ${NUM_TOPICS} topics) ===================="
echo "Training set perplexity (plotdata.csv), first -> last checkpoint:"
printf "%-10s %-8s %-9s %s\n" "Approach" "Status" "Time" "Perplexity"
failed=0
for i in ${!NAMES[@]}; do
  printf "%-10s %-8s %-9s %s\n" "${NAMES[$i]}" "${STATUS[$i]}" "${DURATIONS[$i]}" "${TRAIN_PERPLEXITIES[$i]}"
  [ "${STATUS[$i]}" != "OK" ] && failed=1
done
echo "Outputs and logs: ${SMOKETEST_DIR}"
exit ${failed}
