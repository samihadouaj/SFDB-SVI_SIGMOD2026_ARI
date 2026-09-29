#!/bin/bash

# Stop at the first failing step
set -eo pipefail

# Paths; every step writes its log to logs/log_<script>.txt
SCRIPTSDIR_ABS_PATH=$(dirname $(readlink -f ${BASH_SOURCE[0]}))
PROJECT_ROOT_ABS_PATH=$(readlink -f ${SCRIPTSDIR_ABS_PATH}/..)
LOGS_ABS_PATH=${PROJECT_ROOT_ABS_PATH}/logs
mkdir -p ${LOGS_ABS_PATH}

# Use the GCC 11 toolchain and TeX Live (installed by get_deps.sh)
source /opt/rh/devtoolset-11/enable
export PATH=$PATH:/usr/local/texlive/2024/bin/x86_64-linux

# Run a script from scripts/ and save its output to its log file
run() {
  echo "==================== $1 ($(date)) ===================="
  bash ${SCRIPTSDIR_ABS_PATH}/$1 2>&1 | tee ${LOGS_ABS_PATH}/log_${1%.sh}.txt
}

# Download and build the dependencies (LLVM ClangJIT, Apache Arrow)
run get_deps.sh
# Install Mallet, then download and preprocess the UCI datasets (including PubMed)
run get_uci_datasets.sh
# Download and preprocess the Wikipedia dataset
run get_wiki_dataset.sh
# Build StarfishDB and C++SVI
run build_all.sh

# PubMed, 100 topics: SFDB-SVI
run run_pubmed_sfdb_svi.sh
# PubMed, 100 topics: Mallet and SFDB-CGS
run run_pubmed_mallet_cgs.sh
# PubMed, 100 topics: C++SVI
run run_pubmed_cxx_svi.sh
# Wikipedia, 200 topics: SFDB-SVI
run run_wiki_sfdb_svi.sh
# Wikipedia, 200 topics: Mallet and SFDB-CGS
run run_wiki_mallet_cgs.sh
# Wikipedia, 200 topics: C++SVI
run run_wiki_cxx_svi.sh

# Compute T1/T2 and print Table 1
python3 ${SCRIPTSDIR_ABS_PATH}/make_loglik_table.py 2>&1 | tee ${LOGS_ABS_PATH}/log_make_loglik_table.txt
# Write both tables as LaTeX and compile them to report/loglik_tables.pdf
python3 ${PROJECT_ROOT_ABS_PATH}/report/make_latex_table.py 2>&1 | tee ${LOGS_ABS_PATH}/log_make_latex_table.txt
