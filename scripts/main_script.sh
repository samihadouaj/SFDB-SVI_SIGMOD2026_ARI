#!/bin/bash

# on failure, terminate the script immediately
# set -e

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
REPORT_ABS_PATH=$(readlink -f ${PROJECT_ROOT_ABS_PATH}/report)
LOGS_ABS_PATH=$(readlink -f ${PROJECT_ROOT_ABS_PATH}/logs)





# 1run the scripts responsible for downloading and compiling all the necessary dependencies  (approx 2 hours ):

source ${SCRIPTSDIR_ABS_PATH}/get_deps.sh | tee ${LOGS_ABS_PATH}/log_get_deps.txt

# run the script to download the data and preprocess them into the required format (approx 1 hours )
 
source ${SCRIPTSDIR_ABS_PATH}/get_uci_datasets.sh | tee ${LOGS_ABS_PATH}/log_get_uci_datasets.txt


# compile starfishDB
cd ${BUILDDIR_ABS_PATH} 

cmake3 -G Ninja -DCMAKE_BUILD_TYPE=RelWithDebInfo -DCMAKE_C_COMPILER=`readlink -f ../libs/llvm-project-cxxjit/bin`/clang -DCMAKE_CXX_COMPILER=`readlink -f ../libs/llvm-project-cxxjit/bin`/clang++ ..  | tee ${LOGS_ABS_PATH}/log_cmake3.txt

ninja | tee ${LOGS_ABS_PATH}/log_ninja.txt



#Run experiments

#Run 20 topics experimets
source ${SCRIPTSDIR_ABS_PATH}/run_lda_benchmarks.sh| tee ${LOGS_ABS_PATH}/log_run_lda_benchmarks.txt
#Cleaning
rm -rf ${BENCHMARKSDIR_ABS_PATH}/gammapdb_data/lda-inmemory-vrexpr/chain_states/*
rm -rf ${BENCHMARKSDIR_ABS_PATH}/mallet_data/chain_states/*

#Run 50 topics experimets
source ${SCRIPTSDIR_ABS_PATH}/run_lda_benchmarksP50.sh| tee ${LOGS_ABS_PATH}/log_run_lda_benchmarksP50.txt
#Cleaning
rm -rf ${BENCHMARKSDIR_ABS_PATH}/gammapdb_data/lda-inmemory-vrexprP/chain_states/*
rm -rf ${BENCHMARKSDIR_ABS_PATH}/mallet_data/chain_states/*

#Run 50 topics experimets
source ${SCRIPTSDIR_ABS_PATH}/run_lda_benchmarksP100.sh| tee ${LOGS_ABS_PATH}/log_run_lda_benchmarksP100.txt
#Cleaning
rm -rf ${BENCHMARKSDIR_ABS_PATH}/gammapdb_data/lda-inmemory-vrexprP/chain_states/*
rm -rf ${BENCHMARKSDIR_ABS_PATH}/mallet_data/chain_states/*


# creating the reports


# cd ${REPORT_ABS_PATH}

# source ${REPORT_ABS_PATH}/create20TopicsReport.sh| tee ${LOGS_ABS_PATH}/log_create20TopicsReport.txt

# source ${REPORT_ABS_PATH}/create50TopicsReport.sh| tee ${LOGS_ABS_PATH}/log_create50TopicsReport.txt

# source ${REPORT_ABS_PATH}/create100TopicsReport.sh| tee ${LOGS_ABS_PATH}/log_create100TopicsReport.txt
