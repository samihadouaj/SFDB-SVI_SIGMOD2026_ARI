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



cd ${REPORT_ABS_PATH}

source ${REPORT_ABS_PATH}/create20TopicsReport.sh| tee ${LOGS_ABS_PATH}/log_create20TopicsReport.txt

source ${REPORT_ABS_PATH}/create50TopicsReport.sh| tee ${LOGS_ABS_PATH}/log_create50TopicsReport.txt

source ${REPORT_ABS_PATH}/create100TopicsReport.sh| tee ${LOGS_ABS_PATH}/log_create100TopicsReport.txt
