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



echo "PROJECT_ROOT_ABS_PATH =" ${PROJECT_ROOT_ABS_PATH}


CMAKECMD="cmake"
if hash cmake3 2>/dev/null; then
   CMAKECMD="cmake3"
fi
echo "CMAKECMD =" ${CMAKECMD}

#
# Download ClangJit sources, if necessary
#
# if [ ! -d "${EXTERNALTOOLS_ABS_PATH}/llvm-project-cxxjit" ]; then
#    echo "${EXTERNALTOOLS_ABS_PATH}/llvm-project-cxxjit does not exist. Downloading the sources..."
#    cd ${EXTERNALTOOLS_ABS_PATH}
#    git clone https://github.com/hfinkel/llvm-project-cxxjit.git
#    cd llvm-project-cxxjit
#    git checkout 91084ef018240bbb8e24235ff5cd8c355a9c1a1e
#    git apply ${PATCHESDIR_ABS_PATH}/clangjit.patch
#    cd ${SCRIPTSDIR_ABS_PATH}
# fi

#
# Recompile ClangJit, if necessary
#
# if [ ! -d "${EXTERNALLIBS_ABS_PATH}/llvm-project-cxxjit" ]; then
#    echo "${EXTERNALLIBS_ABS_PATH}/llvm-project-cxxjit does not exist. Recompiling..."
#    cd ${EXTERNALTOOLS_ABS_PATH}/
#    rm -rf ${EXTERNALTOOLS_ABS_PATH}/llvm-project-cxxjit/build
#    mkdir  ${EXTERNALTOOLS_ABS_PATH}/llvm-project-cxxjit/build
#    rm -rf ${EXTERNALLIBS_ABS_PATH}/llvm-project-cxxjit
#    mkdir  ${EXTERNALLIBS_ABS_PATH}/llvm-project-cxxjit
#    cd ${EXTERNALTOOLS_ABS_PATH}/llvm-project-cxxjit
#    ${CMAKECMD} -S llvm -B build -G Ninja -DCMAKE_BUILD_TYPE=RelWithDebInfo -DBUILD_SHARED_LIBS=ON -DLLVM_USE_SPLIT_DWARF=ON -DLLVM_ENABLE_PROJECTS="llvm;clang;openmp" -DCMAKE_INSTALL_PREFIX=${EXTERNALLIBS_ABS_PATH}/llvm-project-cxxjit
#    cd ${EXTERNALTOOLS_ABS_PATH}/llvm-project-cxxjit/build
#    ninja -j8
#    ninja install
#    cd ${SCRIPTSDIR_ABS_PATH}
# fi

#
# Download Arrow sources, if necessary
#
# if [ ! -d "${EXTERNALTOOLS_ABS_PATH}/apache-arrow-9.0.0" ]; then
#    echo "${EXTERNALTOOLS_ABS_PATH}/apache-arrow-9.0.0 does not exist. Downloading the sources..."
#    cd ${EXTERNALTOOLS_ABS_PATH}
#    #wget https://dlcdn.apache.org/arrow/arrow-9.0.0/apache-arrow-9.0.0.tar.gz
#    wget https://archive.apache.org/dist/arrow/arrow-9.0.0/apache-arrow-9.0.0.tar.gz
#    tar -xvzf apache-arrow-9.0.0.tar.gz
#    rm apache-arrow-9.0.0.tar.gz
#    cd ${SCRIPTSDIR_ABS_PATH}
# fi

#
# Recompile Arrow, if necessary
#
# if [ ! -d "${EXTERNALLIBS_ABS_PATH}/apache-arrow-9.0.0/cpp" ]; then
#    echo "${EXTERNALLIBS_ABS_PATH}/apache-arrow-9.0.0/cpp does not exist. Recompiling..."
#    cd ${EXTERNALTOOLS_ABS_PATH}/
#    rm -rf ${EXTERNALTOOLS_ABS_PATH}/apache-arrow-9.0.0/cpp/build
#    mkdir -p ${EXTERNALTOOLS_ABS_PATH}/apache-arrow-9.0.0/cpp/build
#    rm -rf ${EXTERNALLIBS_ABS_PATH}/apache-arrow-9.0.0
#    mkdir -p ${EXTERNALLIBS_ABS_PATH}/apache-arrow-9.0.0/cpp
#    cd ${EXTERNALTOOLS_ABS_PATH}/apache-arrow-9.0.0/cpp/build
#    ${CMAKECMD} -G Ninja -DARROW_PARQUET=ON -DARROW_ORC=ON -DARROW_FILESYSTEM=ON -DARROW_DATASET=ON -DARROW_CSV=ON -DARROW_COMPUTE=ON -DARROW_WITH_BZ2=ON -DARROW_WITH_ZLIB=ON -DARROW_WITH_LZ4=ON -DARROW_WITH_SNAPPY=ON -DCMAKE_BUILD_TYPE=RelWithDebInfo -DCMAKE_INSTALL_PREFIX=${EXTERNALLIBS_ABS_PATH}/apache-arrow-9.0.0/cpp -DCMAKE_C_COMPILER=${EXTERNALLIBS_ABS_PATH}/llvm-project-cxxjit/bin/clang -DCMAKE_CXX_COMPILER=${EXTERNALLIBS_ABS_PATH}/llvm-project-cxxjit/bin/clang++ ..
#    ninja -j8
#    ninja install
#    cd ${SCRIPTSDIR_ABS_PATH}
# fi

# recreate Makefiles

rm -rf ${BUILDDIR_ABS_PATH}/
mkdir ${BUILDDIR_ABS_PATH}
#cd ${BUILDDIR_ABS_PATH}
${CMAKECMD} -B ${BUILDDIR_ABS_PATH} -G Ninja -DCMAKE_BUILD_TYPE=RelWithDebInfo -DCMAKE_C_COMPILER=${EXTERNALLIBS_ABS_PATH}/llvm-project-cxxjit/bin/clang  -DCMAKE_CXX_COMPILER=${EXTERNALLIBS_ABS_PATH}/llvm-project-cxxjit/bin/clang++ ${PROJECT_ROOT_ABS_PATH}

cd ${SCRIPTSDIR_ABS_PATH} 


# 
# # install tex compiler
wget https://mirror.ctan.org/systems/texlive/tlnet/install-tl-unx.tar.gz --no-check-certificate
zcat < install-tl-unx.tar.gz | tar xf -
cd install-tl-*
yum install perl-Digest-MD5 -y
perl ./install-tl --profile=${CONFDIR_ABS_PATH}/texlive.profile
export PATH=$PATH:/usr/local/texlive/2024/bin/x86_64-linux
# echo 'export PATH=/usr/local/texlive/2024/bin/x86_64-linux:$PATH' >> ~/.bashrc
# source ~/.bashr
rm -rf install-tl-20240915
rm install-tl-unx.tar.gz
cd ${SCRIPTSDIR_ABS_PATH} 