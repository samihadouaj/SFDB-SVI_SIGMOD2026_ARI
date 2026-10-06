# StarfishDB with Stochastic Variational Inference

This repository contains the code of the paper [*Variational Inference for De Finetti Logic*](https://dl.acm.org/doi/10.1145/3802114) and the scripts to reproduce its experiments.

The code extends **StarfishDB**, a relational probabilistic programming query engine built on [Apache Arrow](https://arrow.apache.org) and [ClangJIT](https://github.com/hfinkel/llvm-project-cxxjit/wiki), whose Collapsed Gibbs Sampling (CGS) engine is described in our [SIGMOD paper](https://dl.acm.org/doi/10.1145/3654988), with a Stochastic Variational Inference (SVI) engine.

The scripts below reproduce **Table 1** of the paper: the test log-likelihood of LDA trained with four approaches, evaluated at two timestamps T1 and T2.

| Approach | Implementation |
|---|---|
| SFDB-SVI | StarfishDB, SVI engine (`build/gammapdb_arrow`) |
| SFDB-CGS | StarfishDB, CGS engine (`build/gammapdb_arrow`) |
| C++SVI | Standalone C++ SVI implementation (`cxx_svi/`) |
| Mallet | [Mallet](https://github.com/mimno/Mallet) v202108 with `patches/mallet.patch` |

Datasets: PubMed (100 topics) and Wikipedia (200 topics). All experiments use 24 threads.

---

## Requirements

Running the experiments requires a Linux machine with Docker, at least 24 hardware threads, around **1 TB of disk space** and around **500 GB of RAM**. The whole pipeline takes about 4 days.

Reviewers who do not have such a machine can use [Chameleon Cloud](https://www.chameleoncloud.org) if they have access to it, or contact us to get access to our own server.

## 1. Create the container (on the host)

```bash
./scripts/build_devenv_docker_img.sh
./scripts/create_devenv_docker_cont.sh
```

The second script prints the container name. The repository is mounted in the container at `/gammapdb_arrow`. The experiments run for several days, so start a `tmux` session first and open the container from it:

```bash
tmux
docker exec -it <container name> bash
```

On our server (ada), the two scripts and `docker exec` must be run with `sudo`.

## 2. Run everything (in the container)

```bash
bash /gammapdb_arrow/scripts/main_script.sh
```

`main_script.sh` runs the steps below in order and stops at the first failing step. The output of each step is saved to `logs/log_<script name>.txt`.

| Step | Script (in `scripts/`) | What it does | Duration* |
|---|---|---|---|
| 1 | `get_deps.sh` | Downloads and builds LLVM ClangJIT and Apache Arrow; installs TeX Live | ~1 h |
| 2 | `get_uci_datasets.sh` | Builds Mallet; downloads and preprocesses the UCI datasets (including PubMed) | ~2 h |
| 3 | `get_wiki_dataset.sh` | Downloads and preprocesses the Wikipedia dataset | ~1.4 h |
| 4 | `build_all.sh` | Builds StarfishDB (`build/`) and C++SVI (`cxx_svi/build/`) | ~1 min |
| 5 | `run_pubmed_sfdb_svi.sh` | PubMed: SFDB-SVI | ~7.5 h |
| 6 | `run_pubmed_mallet_cgs.sh` | PubMed: Mallet, then SFDB-CGS | ~18 h |
| 7 | `run_pubmed_cxx_svi.sh` | PubMed: C++SVI | ~7 h |
| 8 | `run_wiki_sfdb_svi.sh` | Wikipedia: SFDB-SVI | ~12 h |
| 9 | `run_wiki_mallet_cgs.sh` | Wikipedia: Mallet, then SFDB-CGS | ~31 h |
| 10 | `run_wiki_cxx_svi.sh` | Wikipedia: C++SVI | ~14.5 h |
| 11 | `make_loglik_table.py` | Computes T1, T2 and the log-likelihoods; prints Table 1 and the comparison with the paper | seconds |
| 12 | `../report/make_latex_table.py` | Writes both tables to `report/loglik_tables.tex` and compiles `report/loglik_tables.pdf` | seconds |

\*Measured on a machine with 2 × Intel Xeon E5-2683 v4 (32 cores, 64 hardware threads) and 503 GB of RAM. Durations include the evaluation of all checkpoints. The whole pipeline took about 4 days.

Run only one experiment at a time: T1 and T2 are derived from measured running times.

## 3. Run the steps individually (optional)

The steps can also be run one by one from `/gammapdb_arrow`, in the order of the table above:

```bash
source /opt/rh/devtoolset-11/enable
export PATH=$PATH:/usr/local/texlive/2024/bin/x86_64-linux

bash scripts/get_deps.sh            2>&1 | tee logs/log_get_deps.txt
bash scripts/get_uci_datasets.sh    2>&1 | tee logs/log_get_uci_datasets.txt
bash scripts/get_wiki_dataset.sh    2>&1 | tee logs/log_get_wiki_dataset.txt
bash scripts/build_all.sh           2>&1 | tee logs/log_build_all.txt

bash scripts/run_pubmed_sfdb_svi.sh   2>&1 | tee logs/log_run_pubmed_sfdb_svi.txt
bash scripts/run_pubmed_mallet_cgs.sh 2>&1 | tee logs/log_run_pubmed_mallet_cgs.txt
bash scripts/run_pubmed_cxx_svi.sh    2>&1 | tee logs/log_run_pubmed_cxx_svi.txt
bash scripts/run_wiki_sfdb_svi.sh     2>&1 | tee logs/log_run_wiki_sfdb_svi.txt
bash scripts/run_wiki_mallet_cgs.sh   2>&1 | tee logs/log_run_wiki_mallet_cgs.txt
bash scripts/run_wiki_cxx_svi.sh      2>&1 | tee logs/log_run_wiki_cxx_svi.txt

python3 scripts/make_loglik_table.py
python3 report/make_latex_table.py
```

The parameters of each experiment are set in its `run_*.sh` script.

---

## Outputs

| Location | Content |
|---|---|
| `benchmarks/csv/` | Per-run results: `*_loglik.csv` (log-likelihood per checkpoint), `*_perplexity.csv`, `*_time.csv` (training time per iteration and timing run), `*_plotdata.csv` (checkpoint times and perplexity) |
| terminal / `logs/log_make_loglik_table.txt` | Table 1 (reproduced) and the comparison with the values reported in the paper |
| `report/loglik_tables.pdf` | The same two tables, typeset |
| `logs/` | Log of every step |

`make_loglik_table.py` and `make_latex_table.py` can be re-run at any time; they expect exactly one run per approach and dataset in `benchmarks/csv/`.

## Table 1 in the paper

| Dataset | Timestamp | SFDB-SVI | SFDB-CGS | C++SVI | Mallet |
|---|---|---:|---:|---:|---:|
| PubMed (100 topics) | T1 (773 s) | **−4.0673** | −4.0767 | −4.0685 | −4.0736 |
| | T2 (971 s) | **−4.0664** | −4.0733 | −4.0679 | −4.0690 |
| Wikipedia (200 topics) | T1 (1659 s) | **−5.5353** | −5.6240 | −5.6567 | −5.7077 |
| | T2 (3281 s) | **−5.5362** | −5.6173 | −5.6099 | −5.6281 |

Log-likelihood values are scaled by 10<sup>8</sup>. Higher is better.

The reproduced table should support the same answers to the two questions Table 1 addresses in the paper:

1. **Can a PPDL program generate an inference method that competes with a hard-coded implementation?** At both timestamps, the log-likelihoods of the models trained with StarfishDB (SFDB-SVI and SFDB-CGS) are largely comparable to those of the specialized tools (C++SVI and Mallet).
2. **How does the new variational method compare with the Monte Carlo method of StarfishDB?** SFDB-SVI converges faster: its log-likelihood is essentially stable from T1 onward, while the Monte Carlo methods (SFDB-CGS and Mallet) keep improving between T1 and T2.

The exact values, and T1 and T2 themselves, may differ slightly depending on the machine.

---

## Repository layout

| Path | Content |
|---|---|
| `src/` | StarfishDB source code |
| `cxx_svi/` | C++SVI source code |
| `extras/` | Dataset preprocessing tools and other utilities; Mallet is installed in `extras/mallet/` |
| `scripts/` | Setup, build and experiment scripts |
| `scripts/legacy/` | Earlier experiment scripts, not used for Table 1 |
| `report/` | LaTeX table generator and its output |
| `conf/` | UCI dataset URLs, optional data directory redirect, TeX Live installation profile |
| `patches/` | Patches applied to ClangJIT and Mallet |
