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





CMAKECMD="cmake"
if hash cmake3 2>/dev/null; then
        CMAKECMD="cmake3"
fi

export MALLET_MEMORY=16g

#
# Build <root>/extras/lda_preprocess, if necessary
#
if [ ! -d "${EXTRASDIR_ABS_PATH}/lda_preprocess/build" ]; then
   echo "${EXTRASDIR_ABS_PATH}/lda_preprocess/build does not exist. Recompiling..."
   ${CMAKECMD} -B ${EXTRASDIR_ABS_PATH}/lda_preprocess/build -G Ninja -D CMAKE_BUILD_TYPE=RelWithDebInfo -D CMAKE_C_COMPILER=gcc -D CMAKE_CXX_COMPILER=g++ ${EXTRASDIR_ABS_PATH}/lda_preprocess
   ninja -C ${EXTRASDIR_ABS_PATH}/lda_preprocess/build
else
   echo "Compiling <root>/extras/lda_preprocess ... Nothing to do."
fi


#
# Download & compile Mallet, if necessary
#
if [ ! -d "${EXTRASDIR_ABS_PATH}/mallet/Mallet" ]; then
   cwd=$(pwd)
   mkdir -p ${EXTRASDIR_ABS_PATH}/mallet/Mallet
   git clone --branch v202108 https://github.com/mimno/Mallet.git ${EXTRASDIR_ABS_PATH}/mallet/Mallet
   cd "${EXTRASDIR_ABS_PATH}/mallet/Mallet" 
   git apply --unsafe-paths --directory=${EXTRASDIR_ABS_PATH}/mallet/Mallet ${PATCHESDIR_ABS_PATH}/mallet.patch
   ant -buildfile ${EXTRASDIR_ABS_PATH}/mallet/Mallet/build.xml
   cd ${cwd}
else
   echo "Compiling Mallet... Nothing to do."
fi


#
# Download the UCI "bag-of-words" data sets, if necessary
#
if [ ! -d "${DATADIR_ABS_PATH}/raw/uci" ]; then
   echo "${DATADIR_ABS_PATH}/raw/uci does not exist. Downloading the data sets froms scratch..."
   mkdir -p ${DATADIR_ABS_PATH}/raw/uci
   # download UCI datasets
   wget -P ${DATADIR_ABS_PATH}/raw/uci -nc -i ${CONFDIR_ABS_PATH}/uci_datasets.txt
   for filename in ${DATADIR_ABS_PATH}/raw/uci/*.gz
   do
      if [ ! -f "$(echo ${filename} | sed -e 's/\.[^.]*$//')" ]
         then
            gunzip -c -v "${filename}" > $(echo ${filename} | sed -e 's/\.[^.]*$//')  # x.txt.gz >x.txt
      fi
   done
else
   echo "Downloading UCI datasets... Nothing to do."
fi

# #######################################
# KOS DATASET
# #######################################

#
# Run lda_preprocess on the KOS dataset, if necessary
#
if [ ! -d "${DATADIR_ABS_PATH}/KOS_train/tsv" ]; then
   echo "Converting KOS dataset into TSV format..."
   ${EXTRASDIR_ABS_PATH}/lda_preprocess/build/lda_preprocess --outputDir ${DATADIR_ABS_PATH} --uciDatasetsDir ${DATADIR_ABS_PATH}/raw/uci --dataset KOS --format TSV 
else
   echo "Converting KOS dataset into TSV format... Nothing to do."
fi

if [ ! -d "${DATADIR_ABS_PATH}/KOS_train/csv2" ]; then
   echo "Converting KOS dataset into CSV2 format..."
   ${EXTRASDIR_ABS_PATH}/lda_preprocess/build/lda_preprocess --outputDir ${DATADIR_ABS_PATH} --uciDatasetsDir ${DATADIR_ABS_PATH}/raw/uci --dataset KOS --format CSV2
else
   echo "Converting KOS dataset into CSV2 format... Nothing to do."
fi

#
# Importing KOS dataset into Mallet, if necessary
#
if [ ! -f "${DATADIR_ABS_PATH}/KOS_train/mallet/training.mallet" ]; then
   echo "Converting KOS training set into Mallet format..."
   mkdir -p "${DATADIR_ABS_PATH}/KOS_train/mallet"
   ${EXTRASDIR_ABS_PATH}/mallet/Mallet/bin/mallet import-file --keep-sequence --preserve-case TRUE --token-regex "[\p{L}\p{N}_\p{P}\p{Punct}]+" --input ${DATADIR_ABS_PATH}/KOS_train/tsv/training.tsv --output ${DATADIR_ABS_PATH}/KOS_train/mallet/training.mallet
else
   echo "Converting KOS training set into Mallet format... Nothing to do."
fi

if [ ! -f "${DATADIR_ABS_PATH}/KOS_test/mallet/test.mallet" ]; then
   echo "Converting KOS test set into Mallet format..."
   mkdir -p "${DATADIR_ABS_PATH}/KOS_test/mallet"
   ${EXTRASDIR_ABS_PATH}/mallet/Mallet/bin/mallet import-file --keep-sequence --preserve-case TRUE --token-regex "[\p{L}\p{N}_\p{P}\p{Punct}]+" --input ${DATADIR_ABS_PATH}/KOS_test/tsv/test.tsv --output ${DATADIR_ABS_PATH}/KOS_test/mallet/test.mallet
else
   echo "Converting KOS test set into Mallet format... Nothing to do."
fi

# #######################################
# NYTIMES DATASET
# #######################################

#
# Run lda_preprocess on the NYTIMES dataset, if necessary
#
if [ ! -d "${DATADIR_ABS_PATH}/NYTIMES_train/tsv" ]; then
   echo "Converting NYTIMES dataset into TSV format..."
   ${EXTRASDIR_ABS_PATH}/lda_preprocess/build/lda_preprocess --outputDir ${DATADIR_ABS_PATH} --uciDatasetsDir ${DATADIR_ABS_PATH}/raw/uci --dataset NYTIMES --format TSV 
else
   echo "Converting NYTIMES dataset into TSV format... Nothing to do."
fi

if [ ! -d "${DATADIR_ABS_PATH}/NYTIMES_train/csv2" ]; then
   echo "Converting NYTIMES dataset into CSV2 format..."
   ${EXTRASDIR_ABS_PATH}/lda_preprocess/build/lda_preprocess --outputDir ${DATADIR_ABS_PATH} --uciDatasetsDir ${DATADIR_ABS_PATH}/raw/uci --dataset NYTIMES --format CSV2
else
   echo "Converting NYTIMES dataset into CSV2 format... Nothing to do."
fi

#
# Importing NYTIMES dataset into Mallet, if necessary
#
if [ ! -f "${DATADIR_ABS_PATH}/NYTIMES_train/mallet/training.mallet" ]; then
   echo "Converting NYTIMES training set into Mallet format..."
   mkdir -p "${DATADIR_ABS_PATH}/NYTIMES_train/mallet"
   ${EXTRASDIR_ABS_PATH}/mallet/Mallet/bin/mallet import-file --keep-sequence --preserve-case TRUE --token-regex "[\p{L}\p{N}_\p{P}\p{Punct}]+" --input ${DATADIR_ABS_PATH}/NYTIMES_train/tsv/training.tsv --output ${DATADIR_ABS_PATH}/NYTIMES_train/mallet/training.mallet
else
   echo "Converting NYTIMES training set into Mallet format... Nothing to do."
fi

if [ ! -f "${DATADIR_ABS_PATH}/NYTIMES_test/mallet/test.mallet" ]; then
   echo "Converting NYTIMES test set into Mallet format..."
   mkdir -p "${DATADIR_ABS_PATH}/NYTIMES_test/mallet"
   ${EXTRASDIR_ABS_PATH}/mallet/Mallet/bin/mallet import-file --keep-sequence --preserve-case TRUE --token-regex "[\p{L}\p{N}_\p{P}\p{Punct}]+" --input ${DATADIR_ABS_PATH}/NYTIMES_test/tsv/test.tsv --output ${DATADIR_ABS_PATH}/NYTIMES_test/mallet/test.mallet
else
   echo "Converting NYTIMES test set into Mallet format... Nothing to do."
fi



# #######################################
# PUBMED DATASET
# #######################################

#
# Run lda_preprocess on the PUBMED dataset, if necessary
#
if [ ! -d "${DATADIR_ABS_PATH}/PUBMED_train/tsv" ]; then
   echo "Converting PUBMED dataset into TSV format..."
   ${EXTRASDIR_ABS_PATH}/lda_preprocess/build/lda_preprocess --outputDir ${DATADIR_ABS_PATH} --uciDatasetsDir ${DATADIR_ABS_PATH}/raw/uci --dataset PUBMED --format TSV 
else
   echo "Converting PUBMED dataset into TSV format... Nothing to do."
fi

if [ ! -d "${DATADIR_ABS_PATH}/PUBMED_train/csv2" ]; then
   echo "Converting PUBMED dataset into CSV2 format..."
   ${EXTRASDIR_ABS_PATH}/lda_preprocess/build/lda_preprocess --outputDir ${DATADIR_ABS_PATH} --uciDatasetsDir ${DATADIR_ABS_PATH}/raw/uci --dataset PUBMED --format CSV2
else
   echo "Converting PUBMED dataset into CSV2 format... Nothing to do."
fi

#
# Importing PUBMED dataset into Mallet, if necessary
#
if [ ! -f "${DATADIR_ABS_PATH}/PUBMED_train/mallet/training.mallet" ]; then
   echo "Converting PUBMED training set into Mallet format..."
   mkdir -p "${DATADIR_ABS_PATH}/PUBMED_train/mallet"
   ${EXTRASDIR_ABS_PATH}/mallet/Mallet/bin/mallet import-file --keep-sequence --preserve-case TRUE --token-regex "[\p{L}\p{N}_\p{P}\p{Punct}]+" --input ${DATADIR_ABS_PATH}/PUBMED_train/tsv/training.tsv --output ${DATADIR_ABS_PATH}/PUBMED_train/mallet/training.mallet
else
   echo "Converting PUBMED training set into Mallet format... Nothing to do."
fi

if [ ! -f "${DATADIR_ABS_PATH}/PUBMED_test/mallet/test.mallet" ]; then
   echo "Converting PUBMED test set into Mallet format..."
   mkdir -p "${DATADIR_ABS_PATH}/PUBMED_test/mallet"
   ${EXTRASDIR_ABS_PATH}/mallet/Mallet/bin/mallet import-file --keep-sequence --preserve-case TRUE --token-regex "[\p{L}\p{N}_\p{P}\p{Punct}]+" --input ${DATADIR_ABS_PATH}/PUBMED_test/tsv/test.tsv --output ${DATADIR_ABS_PATH}/PUBMED_test/mallet/test.mallet
else
   echo "Converting PUBMED test set into Mallet format... Nothing to do."
fi





# bin/mallet import-file --keep-sequence --input sample-data/stackexchange/tsv/training.tsv --output tsv_dataset.mallet

# 
# /home/niccolom/vs_projects/gammapdb_arrow/libs/mallet/Mallet/bin/mallet train-topics --input /home/niccolom/vs_projects/gammapdb_arrow/data/NYTIMES_train/mallet/training.mallet --num-topics 20 --num-iterations 20 --output-topic-keys /home/niccolom/vs_projects/gammapdb_arrow/benchmarks/NYTIMES_train_mallet_topics-def.txt --output-state /home/niccolom/vs_projects/gammapdb_arrow/benchmarks/NYTIMES_train_mallet_topics-state.gz






