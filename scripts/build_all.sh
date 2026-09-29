#!/bin/bash

ROOT=$(readlink -f $(dirname $(readlink -f ${BASH_SOURCE[0]}))/..)
source /opt/rh/devtoolset-11/enable

(
  mkdir -p ${ROOT}/build && cd ${ROOT}/build &&
  cmake3 -G Ninja -DCMAKE_BUILD_TYPE=RelWithDebInfo -DCMAKE_C_COMPILER=${ROOT}/libs/llvm-project-cxxjit/bin/clang -DCMAKE_CXX_COMPILER=${ROOT}/libs/llvm-project-cxxjit/bin/clang++ .. &&
  ninja
) && [ -x ${ROOT}/build/gammapdb_arrow ]
STARFISHDB=$?

(
  mkdir -p ${ROOT}/cxx_svi/build && cd ${ROOT}/cxx_svi/build &&
  cmake3 -DCMAKE_BUILD_TYPE=Debug -D CMAKE_C_COMPILER=gcc -D CMAKE_CXX_COMPILER=g++ .. &&
  make clean && make
) && [ -x ${ROOT}/cxx_svi/build/svi ]
CXXSVI=$?

echo
[ $STARFISHDB -eq 0 ] && echo "StarfishDB build: SUCCEEDED" || echo "StarfishDB build: FAILED"
[ $CXXSVI -eq 0 ] && echo "C++SVI build:     SUCCEEDED" || echo "C++SVI build:     FAILED"
[ $STARFISHDB -eq 0 ] && [ $CXXSVI -eq 0 ]
