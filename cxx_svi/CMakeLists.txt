#
# Usage:
#
# *  For DEBUG builds:
#    > mkdir -p build && cd build
#    > cmake3 -DCMAKE_BUILD_TYPE=Debug -D CMAKE_C_COMPILER=gcc -D CMAKE_CXX_COMPILER=g++ ..
#    > make clean && make
#
# *  For RELEASE builds:
#    > mkdir -p build && cd build
#    > cmake3 -DCMAKE_BUILD_TYPE=RelWithDebInfo -D CMAKE_C_COMPILER=gcc -D CMAKE_CXX_COMPILER=g++ ..
#    > make clean && make
#

cmake_minimum_required(VERSION 3.5.1)

project(svi)

# when in doubt, do a RELEASE build but keep the debug symbols
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE RelWithDebInfo)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# generate verbose Makefile
set(CMAKE_VERBOSE_MAKEFILE ON)

add_compile_options(
  -Wall -Wextra -ggdb3 -march=native # -rdynamic
  $<$<CONFIG:RELEASE>:-O3>
  $<$<CONFIG:RELWITHDEBINFO>:-O3>
  $<$<CONFIG:DEBUG>:-O0>
)

add_compile_definitions(
  $<$<CONFIG:RELEASE>:NDEBUG>
  $<$<CONFIG:RELWITHDEBINFO>:NDEBUG>
  $<$<CONFIG:DEBUG>:DEBUG>
)

# ---- sources (excluding main.cpp) ----
set(SRCS
#   src/getTopics.cpp
  src/MappingReader.cpp
  src/ReadData.cpp
  src/Vocabulary.cpp
  src/argparse.hpp
  src/main.cpp
  src/Svi_lda.cpp
  src/ViUtils.h
  src/InternalState.h
  src/SaveAsMallet.h
  src/Svi_ldaP.cpp
)

# ---- executable target ----
add_executable(svi
  src/main.cpp
  ${SRCS}
)

# headers live in src/
target_include_directories(svi PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# ---- dependencies (optional) ----
# If you need OpenMP, uncomment:
find_package(OpenMP REQUIRED)
target_link_libraries(svi PUBLIC OpenMP::OpenMP_CXX)
