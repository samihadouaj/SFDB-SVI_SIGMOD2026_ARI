#!/bin/bash

# on failure, terminate the script immediately
set -e

# Determine the script's absolute path
SCRIPTSDIR_ABS_PATH=$(readlink -f ${BASH_SOURCE[0]})
SCRIPTSDIR_ABS_PATH=$(dirname ${SCRIPTSDIR_ABS_PATH})
echo "SCRIPTSDIR_ABS_PATH = ${SCRIPTSDIR_ABS_PATH}"

# Determine the project's root path
PROJECT_ROOT_ABS_PATH=$(readlink -f ${SCRIPTSDIR_ABS_PATH}/../)
echo "PROJECT_ROOT_ABS_PATH = ${PROJECT_ROOT_ABS_PATH}"

# Determine commonly used directories
EXTERNALTOOLS_ABS_PATH=$(readlink -f ${PROJECT_ROOT_ABS_PATH}/external)
echo "EXTERNALTOOLS_ABS_PATH = ${EXTERNALTOOLS_ABS_PATH}"

EXTERNALLIBS_ABS_PATH=$(readlink -f ${PROJECT_ROOT_ABS_PATH}/libs)
echo "EXTERNALLIBS_ABS_PATH = ${EXTERNALLIBS_ABS_PATH}"

PATCHESDIR_ABS_PATH=$(readlink -f ${PROJECT_ROOT_ABS_PATH}/patches)
echo "PATCHESDIR_ABS_PATH = ${PATCHESDIR_ABS_PATH}"

BUILDDIR_ABS_PATH=$(readlink -f ${PROJECT_ROOT_ABS_PATH}/build)
echo "BUILDDIR_ABS_PATH = ${BUILDDIR_ABS_PATH}"

DATADIR_ABS_PATH=$(readlink -f ${PROJECT_ROOT_ABS_PATH}/data)
echo "DATADIR_ABS_PATH = ${DATADIR_ABS_PATH}"

CONFDIR_ABS_PATH=$(readlink -f ${PROJECT_ROOT_ABS_PATH}/conf)
echo "CONFDIR_ABS_PATH = ${CONFDIR_ABS_PATH}"

EXTRASDIR_ABS_PATH=$(readlink -f ${PROJECT_ROOT_ABS_PATH}/extras)
echo "EXTRASDIR_ABS_PATH = ${EXTRASDIR_ABS_PATH}"

BENCHMARKSDIR_ABS_PATH=$(readlink -f ${PROJECT_ROOT_ABS_PATH}/benchmarks)
echo "BENCHMARKSDIR_ABS_PATH = ${BENCHMARKSDIR_ABS_PATH}"

# Relocate <root>/data, if necessary
if [ $(grep -c "^[^#]" $CONFDIR_ABS_PATH/datadir_redirect.txt) != "0" ]; then
  mkdir -p $(grep "^[^#]" $CONFDIR_ABS_PATH/datadir_redirect.txt | head -n 1)
  DATADIR_ABS_PATH=$(readlink -f $(grep "^[^#]" ${CONFDIR_ABS_PATH}/datadir_redirect.txt | head -n 1))
  echo "NOTICE: <root>/data directory was relocated to ${DATADIR_ABS_PATH}"
fi

DISTRO="devenv"
echo "DISTRO = ${DISTRO}"

# define the image's name
DOCKER_IMG_NAME="gensim_lda_centos7_docker_img" 
echo "DOCKER_IMG_NAME = ${DOCKER_IMG_NAME}"

echo "Building docker image ${DOCKER_IMG_NAME}"

# build the image
cwd=$(pwd)
echo "Current working directory before change = ${cwd}"

cd ${PROJECT_ROOT_ABS_PATH}
echo "Changed directory to = ${PROJECT_ROOT_ABS_PATH}"

echo "Docker build command = docker build -t ${DOCKER_IMG_NAME} -f ${SCRIPTSDIR_ABS_PATH}/dockerfiles/${DISTRO}/gensim_Dockerfile ."
docker build -t ${DOCKER_IMG_NAME} -f ${SCRIPTSDIR_ABS_PATH}/dockerfiles/${DISTRO}/gensim_Dockerfile .

cd ${cwd}
echo "Changed back to original directory = ${cwd}"