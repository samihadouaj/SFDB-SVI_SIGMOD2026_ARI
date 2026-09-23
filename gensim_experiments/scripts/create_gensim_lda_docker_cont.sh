#!/bin/bash

# on failure, terminate the script immediately
set -e

# determine original username 
ORIG_USER=${SUDO_USER-${USER}}

# Determine the script's absolute path
SCRIPTSDIR_ABS_PATH=$(readlink -f "${BASH_SOURCE[0]}")
SCRIPTSDIR_ABS_PATH=$(dirname "${SCRIPTSDIR_ABS_PATH}")

# Determine the project's root path
PROJECT_ROOT_ABS_PATH=$(readlink -f "${SCRIPTSDIR_ABS_PATH}/../")
PROJECT_ROOT_ABS_PATH_HASH=$(echo "${PROJECT_ROOT_ABS_PATH}" | md5sum | cut -d' ' -f 1 | cut -c 25-32) 

# Determine commonly used directories
DATADIR_ABS_PATH=$(readlink -f "${PROJECT_ROOT_ABS_PATH}/data")

# Relocate <root>/data, if necessary
if [ $(grep -c "^[^#]" "${SCRIPTSDIR_ABS_PATH}/../conf/datadir_redirect.txt") != "0" ]; then
  mkdir -p $(grep "^[^#]" "${SCRIPTSDIR_ABS_PATH}/../conf/datadir_redirect.txt" | head -n 1)
  DATADIR_ABS_PATH=$(readlink -f $(grep "^[^#]" "${SCRIPTSDIR_ABS_PATH}/../conf/datadir_redirect.txt" | head -n 1))
  echo "NOTICE: <root>/data directory was relocated to ${DATADIR_ABS_PATH}"
fi

# Ensure the data directory exists
mkdir -p "${DATADIR_ABS_PATH}"

DISTRO="devenv"

# define the image's name
DOCKER_IMG_NAME="gensim_lda_centos7_docker_img" 
DOCKER_CONT_NAME="gensim_lda_container_${ORIG_USER}${PROJECT_ROOT_ABS_PATH_HASH}"
MOUNT_OPTIONS="${PROJECT_ROOT_ABS_PATH}:/app:Z ${DATADIR_ABS_PATH}:/data:Z"

echo "Mount options: ${MOUNT_OPTIONS}"
echo "Container name: ${DOCKER_CONT_NAME}"

docker run -v "${PROJECT_ROOT_ABS_PATH}:/app:Z" -v "${DATADIR_ABS_PATH}:/data:Z" --name "${DOCKER_CONT_NAME}" --detach --tty "${DOCKER_IMG_NAME}" /bin/bash

echo "Or run \"docker exec -it ${DOCKER_CONT_NAME} bash\" to access the container."