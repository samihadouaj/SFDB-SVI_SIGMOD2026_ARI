#
# Usage:
#
# *  For RELEASE builds: 
#    > cd build
#    > cmake3 -G Ninja -DCMAKE_BUILD_TYPE=RelWithDebInfo -DCMAKE_C_COMPILER=`readlink -f ../libs/llvm-project-cxxjit/bin`/clang -DCMAKE_CXX_COMPILER=`readlink -f ../libs/llvm-project-cxxjit/bin`/clang++ ..
#    > make clean && make
#
# *  For DEBUG builds: 
#    > cd build
#    > cmake3 -G Ninja -DCMAKE_BUILD_TYPE=Debug -DCMAKE_C_COMPILER=`readlink -f ../libs/llvm-project-cxxjit/bin`/clang -DCMAKE_CXX_COMPILER=`readlink -f ../libs/llvm-project-cxxjit/bin`/clang++ ..
#    > make clean && make
#
# 
#
#
cmake_minimum_required(VERSION 3.5.1)

project(gammapdb_arrow)

# when in doubt, do a RELEASE build but keep the debug symbols
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE RelWithDebInfo)
endif()

# clanjit-friendly compiler settings
#set(CMAKE_CXX_STANDARD 11)           # c++11
set(CMAKE_CXX_STANDARD 14)           # c++14
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# generate verbose Makefile
set(CMAKE_VERBOSE_MAKEFILE ON)

add_compile_options(
  -Wall -Wextra -ggdb3 -w #-rdynamic
  $<$<CONFIG:RELEASE>:-O3>
  $<$<CONFIG:RELWITHDEBINFO>:-O3>
  # $<$<CONFIG:DEBUG>:-O0>
)

add_compile_definitions(
  $<$<CONFIG:RELEASE>:NDEBUG>
  $<$<CONFIG:RELWITHDEBINFO>:NDEBUG>
  $<$<CONFIG:DEBUG>:DEBUG>
)

# declare source files
set(SRCS
# utils
  src/utils/argparse.hpp
  src/utils/DebugHelper.h src/utils/DebugHelper.cpp
  #
  #src/utils/RndUtils.cpp
  #src/utils/RndUtils.h
  #
  src/utils/TimeThis.cpp
  src/utils/TimeThis.h
# lda
  src/lda/Vocabulary.h src/lda/Vocabulary.cpp
  src/lda/GibbsSinkNodeConsumer.h
  src/lda/InMemoryGibbsPlanNode.h
  src/lda/LdaPlanBuilder.h 
  src/lda_svi/MappingReader.h src/lda_svi/MappingReader.cpp
  src/lda_svi/LdaSVIPlanBuilder.h
  src/lda_svi/InMemorySVIPlanNode.h
# hmm
  src/hmm/HmmPlanBuilder.h
  src/hmm/InMemoryHmmPlanNode.h
# src
  src/BDSet.h
  src/BDExpr.h src/BDExpr2.h src/BDExpr3.h
  src/SamplerExecutionContext.h
  src/VrdExprLiterals.h src/VrdExprOperators.h


    #SVI files
    src/main.cpp
    src/ReadData.h
    src/ReadData.cpp
    src/Tau.h
    src/Atom.h
    src/Internal_State.h
    src/VIDie.h
    src/ViUtils.h
    src/Svi_Template.h

#  src/FancyMacros.h
)

# declare test source files
set(TEST_SRCS
   test/test_BDSet.cpp
#   test/test_FancyMacros.cpp
)





#set(CMAKE_PREFIX_PATH
#  ${CMAKE_PREFIX_PATH}
#  ${PROJECT_SOURCE_DIR}/libs/apache-arrow-9.0.0/cpp/lib64/cmake/arrow
#  ${PROJECT_SOURCE_DIR}/libs/apache-arrow-9.0.0/cpp/lib/cmake/arrow
#)
list(APPEND CMAKE_PREFIX_PATH ${PROJECT_SOURCE_DIR}/libs/apache-arrow-9.0.0/cpp/lib/cmake/arrow)
list(APPEND CMAKE_PREFIX_PATH ${PROJECT_SOURCE_DIR}/libs/apache-arrow-9.0.0/cpp/lib64/cmake/arrow)

# declare dependencies
find_package(Arrow REQUIRED)
find_package(ArrowDataset REQUIRED)
find_package(Parquet REQUIRED)
find_package(OpenMP REQUIRED)
# declare executables

## gammapdb_arrow
add_executable(gammapdb_arrow src/gammapdb_arrow.cpp ${SRCS})

target_link_libraries(gammapdb_arrow PRIVATE arrow_shared)
target_link_libraries(gammapdb_arrow PRIVATE arrow_dataset_shared)
target_link_libraries(gammapdb_arrow PRIVATE parquet_shared)
target_link_libraries(gammapdb_arrow PRIVATE -fjit ${OpenMP_CXX_FLAGS})

target_compile_options(gammapdb_arrow PRIVATE -Wall -fjit ${OpenMP_CXX_FLAGS})
target_link_options(gammapdb_arrow PRIVATE ${OpenMP_CXX_FLAGS})

## gencode
# add_executable(gencode src/gencode.cpp src/utils/DebugHelper.h src/utils/DebugHelper.cpp)

## run_all_tests
# add_executable(run_all_tests test/run_all_tests.cpp ${SRCS} ${TEST_SRCS})

# target_link_libraries(run_all_tests PRIVATE arrow_shared)
# target_link_libraries(run_all_tests PRIVATE arrow_dataset_shared)

# target_compile_options(run_all_tests PUBLIC -Wall -fjit ${OpenMP_CXX_FLAGS})
# target_link_libraries(run_all_tests PUBLIC -fjit ${OpenMP_CXX_FLAGS})



# #jit_svi
# #SVI

# add_executable(jit_svi jit_svi/src/main.cpp ${jit_svi_SRCS})

# target_link_libraries(jit_svi PRIVATE arrow_shared)
# target_link_libraries(jit_svi PRIVATE arrow_dataset_shared)

# target_compile_options(jit_svi PRIVATE -Wall -fjit ${OpenMP_CXX_FLAGS})
# target_link_libraries(jit_svi PRIVATE -fjit ${OpenMP_CXX_FLAGS})



