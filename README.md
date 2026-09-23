<!-- gammapdb_arrow
==============

`gammapdb_arrow` is a vectorized query execution engine designed for [Gamma Probabilistic Databases](http://dx.doi.org/10.48786/edbt.2022.14). 
It is built on top of [Apache Arrow](https://arrow.apache.org) and [ClangJIT](https://github.com/hfinkel/llvm-project-cxxjit/wiki) ([paper](https://doi.ieeecomputersociety.org/10.1109/P3HPC49587.2019.00013)).

In this repository we have provide the implementation of StarfishDB. StarfishDB is a relational probabilistic programming query engine.
Internally, starfishDB supports the use of two inference engines: Collpased Gibbs Sampler and Stochastic Variational Inference.
The version published here ([paper](https://dl.acm.org/doi/10.1145/3654988)). Consists of the Collpased Gibbs Sampler of starfishDB.

This implementation builds on top of the existing StarfishDB implementation to provide support for Stochastic Variational Inference.

In order to run the LDA experiments, please follow these steps:


Create a docker container:
sudo ./scripts/build_devenv_docker_img.sh
sudo ./scripts/create_devenv_docker_cont.sh
 Run tmux to prevent disconnection issues: 
 tmux
sudo docker exec -it starfishdb_dev_env_container_ec2-user863a3113 bash
 \textbf{Note:} The exact container name may vary. Replace with the appropriate name if different.

 Download the required dependencies:
source scripts/get_deps.sh 2>&1 | tee ${LOGS_ABS_PATH}/log_get_deps.txt

Download and preprocess the required datasets:

source scripts/get_uci_datasets .sh 2>&1 | tee ${LOGS_ABS_PATH}/log_get_uci_datasets.txt
source scripts/get_wiki_dataset.sh 2>&1 | tee ${LOGS_ABS_PATH}/log_get_wiki_dataset.txt


Building starfishDB:
cd build

cmake3 -G Ninja -DCMAKE_BUILD_TYPE=RelWithDebInfo -DCMAKE_C_COMPILER=`readlink -f ../libs/llvm-project-cxxjit/bin`/clang -DCMAKE_CXX_COMPILER=`readlink -f ../libs/llvm-project-cxxjit/bin`/clang++ ..  2>&1 | tee ${LOGS_ABS_PATH}/log_cmake3.txt

ninja 2>&1 | tee ${LOGS_ABS_PATH}/log_ninja.txt

#Run 50 topics experimets starfishDB-CGS
source scripts/run_lda_benchmarksP50.sh 2>&1 | tee ${LOGS_ABS_PATH}/log_run_lda_benchmarksP50.txt


#Run 100 topics experimets starfishDB-CGS
source scripts/run_lda_benchmarksP100.sh 2>&1 | tee ${LOGS_ABS_PATH}/log_run_lda_benchmarksP100.txt

#Run 200 topics experimets starfishDB-CGS
source scripts/run_lda_benchmarksP200.sh 2>&1 | tee ${LOGS_ABS_PATH}/log_run_lda_benchmarksP200.txt




#Run 50 topics experimets starfishDB-SVI
source scripts/run_vi_benchmark50.sh 2>&1 | tee ${LOGS_ABS_PATH}/log_run_vi_benchmark50.txt


#Run 100 topics experimets starfishDB-SVI
source scripts/run_vi_benchmark100.sh 2>&1 | tee ${LOGS_ABS_PATH}/log_run_vi_benchmark100.txt

#Run 200 topics experimets starfishDB-SVI
source scripts/run_vi_benchmark200.sh 2>&1 | tee ${LOGS_ABS_PATH}/log_run_vi_benchmark200.txt -->




# gammapdb_arrow

`gammapdb_arrow` is a vectorized query execution engine designed for [Gamma Probabilistic Databases](http://dx.doi.org/10.48786/edbt.2022.14). It is built on top of [Apache Arrow](https://arrow.apache.org) and [ClangJIT](https://github.com/hfinkel/llvm-project-cxxjit/wiki) ([paper](https://doi.ieeecomputersociety.org/10.1109/P3HPC49587.2019.00013)).

---

## Repository Overview

This repository provides the implementation of **StarfishDB**, a relational probabilistic programming query engine. Internally, StarfishDB supports two inference engines:

- **Collapsed Gibbs Sampling (CGS)**
- **Stochastic Variational Inference (SVI)**

The version published here corresponds to the CGS implementation described in our SIGMOD paper ([paper](https://dl.acm.org/doi/10.1145/3654988)). This fork extends the original implementation with full SVI support.

---

## Running the LDA Experiments

Follow the steps below to build the development environment, fetch dependencies & datasets, compile the project, and run benchmarks.

### 1  Create and Enter the Docker Development Container

```bash
# Build the image
sudo ./scripts/build_devenv_docker_img.sh

# Create the container
sudo ./scripts/create_devenv_docker_cont.sh

# Attach via tmux (recommended to avoid disconnects)
tmux
sudo docker exec -it starfishdb_dev_env_container_ec2-user863a3113 bash
# Note: The exact container name may vary.
```

### 2  Install Dependencies

```bash
source scripts/get_deps.sh 2>&1 | tee "logs/log_get_deps.txt"
```

### 3  Download & Preprocess Datasets

```bash
source scripts/get_uci_datasets.sh 2>&1 | tee "logs/log_get_uci_datasets.txt"
source scripts/get_wiki_dataset.sh 2>&1 | tee "logs/log_get_wiki_dataset.txt"
```

### 4  Build StarfishDB

```bash
cd build
cmake3 -G Ninja -DCMAKE_BUILD_TYPE=RelWithDebInfo \
  -DCMAKE_C_COMPILER="$(readlink -f ../libs/llvm-project-cxxjit/bin)/clang" \
  -DCMAKE_CXX_COMPILER="$(readlink -f ../libs/llvm-project-cxxjit/bin)/clang++" \
  .. 2>&1 | tee "log_cmake3.txt"

ninja 2>&1 | tee "log_ninja.txt"
```

### 5  Run LDA Benchmarks

#### Collapsed Gibbs Sampling (CGS)

```bash
# 50 topics
source "scripts/run_lda_benchmarksP50.sh" 2>&1 | tee "logs/log_run_lda_benchmarksP50.txt"

# 100 topics
source "scripts/run_lda_benchmarksP100.sh" 2>&1 | tee "logs/log_run_lda_benchmarksP100.txt"

# 200 topics
source "scripts/run_lda_benchmarksP200.sh" 2>&1 | tee "logs/log_run_lda_benchmarksP200.txt"
```

#### Stochastic Variational Inference (SVI)

```bash
# 50 topics
source "scripts/run_vi_benchmark50.sh" 2>&1 | tee "logs/log_run_vi_benchmark50.txt"

# 100 topics
source "scripts/run_vi_benchmark100.sh" 2>&1 | tee "logs/log_run_vi_benchmark100.txt"

# 200 topics
source "scripts/run_vi_benchmark200.sh" 2>&1 | tee "logs/log_run_vi_benchmark200.txt"
```

---
