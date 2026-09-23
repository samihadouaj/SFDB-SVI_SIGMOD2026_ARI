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
# Build <root>/extras/lda_wiki_preprocess, if necessary
#
if [ ! -d "${EXTRASDIR_ABS_PATH}/lda_wiki_preprocess/build" ]; then
   echo "${EXTRASDIR_ABS_PATH}/lda_wiki_preprocess/build does not exist. Recompiling..."
   ${CMAKECMD} -B ${EXTRASDIR_ABS_PATH}/lda_wiki_preprocess/build -G Ninja -D CMAKE_BUILD_TYPE=RelWithDebInfo -D CMAKE_C_COMPILER=gcc -D CMAKE_CXX_COMPILER=g++ ${EXTRASDIR_ABS_PATH}/lda_wiki_preprocess
   ninja -C ${EXTRASDIR_ABS_PATH}/lda_wiki_preprocess/build
else
   echo "Compiling <root>/extras/lda_wiki_preprocess ... Nothing to do."
fi


#
# Build <root>/extras/svi_lda_preprocess, if necessary
#extras/svi_lda_preprocess
if [ ! -d "${EXTRASDIR_ABS_PATH}/svi_lda_preprocess/build" ]; then
   echo "${EXTRASDIR_ABS_PATH}/svi_lda_preprocess/build does not exist. Recompiling..."
   ${CMAKECMD} -B ${EXTRASDIR_ABS_PATH}/svi_lda_preprocess/build -G Ninja -D CMAKE_BUILD_TYPE=RelWithDebInfo -D CMAKE_C_COMPILER=gcc -D CMAKE_CXX_COMPILER=g++ ${EXTRASDIR_ABS_PATH}/svi_lda_preprocess
   ninja -C ${EXTRASDIR_ABS_PATH}/svi_lda_preprocess/build
else
   echo "Compiling <root>/extras/svi_lda_preprocess ... Nothing to do."
fi






mkdir -p "${DATADIR_ABS_PATH}/raw"

# Download wiki08.txt if it doesn't exist.
if [ ! -f "${DATADIR_ABS_PATH}/raw/wiki08.txt" ]; then
    echo "${DATADIR_ABS_PATH}/raw/wiki08.txt does not exist. Downloading wiki08.txt from scratch..."
    URL="https://www.dropbox.com/scl/fi/bhrw04r7c7udscf2g2ajm/wiki08.txt?rlkey=73wk3aslae4weicop5m6cjy8d&st=hvgkp4y2&dl=1"
    OUTPUT_FILE="${DATADIR_ABS_PATH}/raw/wiki08.txt"
    echo "Downloading wiki08.txt from Dropbox using wget..."
    wget -O "$OUTPUT_FILE" "$URL"
    echo "Download complete: $OUTPUT_FILE"
else
    echo "wiki08.txt already exists. Nothing to do for wiki08.txt."
fi

# Download stopwords.txt if it doesn't exist.
if [ ! -f "${DATADIR_ABS_PATH}/raw/stopwords.txt" ]; then
    echo "${DATADIR_ABS_PATH}/raw/stopwords.txt does not exist. Downloading stopwords.txt from scratch..."
    URL="https://www.dropbox.com/scl/fi/zn0dcn527s0sa36pl8fbi/stopwords.txt?rlkey=vq9oba0rfc0ix4upmy2g2xa0l&st=euaqdgsh&dl=1"
    OUTPUT_FILE="${DATADIR_ABS_PATH}/raw/stopwords.txt"
    echo "Downloading stopwords.txt from Dropbox using wget..."
    wget -O "$OUTPUT_FILE" "$URL"
    echo "Download complete: $OUTPUT_FILE"
else
    echo "stopwords.txt already exists. Nothing to do for stopwords.txt."
fi

# #######################################
# wiki DATASET
# #######################################

#
# Run lda_wiki_preprocess on the wiki dataset, if necessary
# if the input dataset is not clean, you can set doCleaning=1 to start a cleaning process before processing the dataset for LDA
#

DO_CLEANING=0
if [ ! -f "${DATADIR_ABS_PATH}/raw/wiki08_clean.txt" ]; then
   echo "wiki08_clean.txt does not exist. Will proceed with the cleaning."
   DO_CLEANING=1
else
   echo "wiki08_clean.txt exists. No cleaning needed."
   DO_CLEANING=0
fi


echo "Running lda_wiki_preprocess on the wiki dataset..."
if [ ! -d "${DATADIR_ABS_PATH}/wiki08_train" ]; then
   echo "Converting wiki dataset into CSV2 format..."
   ${EXTRASDIR_ABS_PATH}/lda_wiki_preprocess/build/lda_wiki_preprocess --doCleaning ${DO_CLEANING}  --outputDir ${DATADIR_ABS_PATH} --wikiDatasetsDir ${DATADIR_ABS_PATH}/raw --dataset wiki08 --totalNbOfDocs 4000000  --reportEvery 1000 --holdOutFreq 0.05 
   echo "Converting wiki dataset into CSV2 format... Done."
   echo "Starting to convert the wiki dataset into svi format..."
   ${EXTRASDIR_ABS_PATH}/svi_lda_preprocess/build/svi_lda_preprocess --outputDir ${DATADIR_ABS_PATH}/wiki08_train/csv2 --datasetsDir ${DATADIR_ABS_PATH}/wiki08_train/csv2 --dataset wiki08
   echo "Converting the wiki dataset into svi format... Done."
else
   echo "Converting wiki dataset into CSV2 and SVI format... Nothing to do."
fi


#
# Importing wiki dataset into Mallet, if necessary
#
if [ ! -f "${DATADIR_ABS_PATH}/wiki08_train/mallet/training.mallet" ]; then
   echo "Converting wiki08 training set into Mallet format..."
   mkdir -p "${DATADIR_ABS_PATH}/wiki08_train/mallet"
   ${EXTRASDIR_ABS_PATH}/mallet/Mallet/bin/mallet import-file --keep-sequence --preserve-case TRUE --token-regex "[\p{L}\p{N}_\p{P}\p{Punct}]+" --input ${DATADIR_ABS_PATH}/wiki08_train/tsv/training.tsv --output ${DATADIR_ABS_PATH}/wiki08_train/mallet/training.mallet
else
   echo "Converting wiki08 training set into Mallet format... Nothing to do."
fi

if [ ! -f "${DATADIR_ABS_PATH}/wiki08_test/mallet/test.mallet" ]; then
   echo "Converting wiki08 test set into Mallet format..."
   mkdir -p "${DATADIR_ABS_PATH}/wiki08_test/mallet"
   ${EXTRASDIR_ABS_PATH}/mallet/Mallet/bin/mallet import-file --keep-sequence --preserve-case TRUE --token-regex "[\p{L}\p{N}_\p{P}\p{Punct}]+" --input ${DATADIR_ABS_PATH}/wiki08_test/tsv/test.tsv --output ${DATADIR_ABS_PATH}/wiki08_test/mallet/test.mallet
else
   echo "Converting wiki08 test set into Mallet format... Nothing to do."
fi

# echo ${DATADIR_ABS_PATH} 
#    ${EXTRASDIR_ABS_PATH}/svi_lda_preprocess/build/svi_lda_preprocess --outputDir ${DATADIR_ABS_PATH}/wiki08_train/csv2 --datasetsDir ${DATADIR_ABS_PATH}/wiki08_train/csv2 --dataset wiki08
