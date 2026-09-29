#!/usr/bin/env python3
"""Recreating the experiment from the paper using the  log-likelihood of SFDB-SVI, SFDB-CGS, C++SVI and Mallet at timestamps T1 and T2 (24 threads).

Usage: python3 scripts/make_loglik_table.py [csv_dir]   (default: benchmarks/csv)
"""
import glob
import os
import sys

import numpy as np
import pandas as pd

DEFAULT_CSV_DIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "benchmarks", "csv")

# (label, dataset name in file names, topics, SFDB-SVI iteration defining T1)
DATASETS = [("PubMed (100 topics)", "PUBMED", 100, 99),
            ("Wikipedia (200 topics)", "wiki08", 200, 54)]

# file-name patterns after "<dataset>_test_<K>topics_"
METHODS = {"SFDB-SVI": "batchS*_NT24_RND123_gammapdb_VI_ViIterBatch*",
           "SFDB-CGS": "A*_NT24_RND123_gammapdb_lda-inmemory-vrexprP",
           "C++SVI":   "batchS*_NT24_RND123_cxx_SVI_lda_vi_iter_per_batch*",
           "Mallet":   "A*_NT24_RND123_mallet"}

# Table 1 as published (x1e8): {dataset: {timestamp: (seconds, [values in METHODS order])}}
PAPER = {"PUBMED": {"T1": (773, [-4.0673, -4.0767, -4.0685, -4.0736]),
                    "T2": (971, [-4.0664, -4.0733, -4.0679, -4.0690])},
         "wiki08": {"T1": (1659, [-5.5353, -5.6240, -5.6567, -5.7077]),
                    "T2": (3281, [-5.5362, -5.6173, -5.6099, -5.6281])}}


def load_curve(csv_dir, dataset, topics, pattern):
    """(iterations, times, log-likelihoods) of the single matching run."""
    matches = glob.glob(os.path.join(csv_dir, f"{dataset}_test_{topics}topics_{pattern}_plotdata.csv"))
    if len(matches) != 1:
        sys.exit(f"expected 1 run for {dataset}_test_{topics}topics_{pattern}, found {len(matches)}: {matches}")
    plotdata = pd.read_csv(matches[0], header=None)
    loglik = pd.read_csv(matches[0].replace("_plotdata.csv", "_loglik.csv"), header=None)
    if len(plotdata) != len(loglik):
        sys.exit(f"mismatched number of rows in {matches[0]} and its _loglik.csv")
    return list(loglik.iloc[:, 0]), plotdata.iloc[:, 0].values, loglik.iloc[:, 1].values


def loglik_at(times, log_likelihoods, reference_time):
    """Log-likelihood at reference_time if the run lasts until then, otherwise the run's last value."""
    if times[-1] < reference_time:
        return float(log_likelihoods[-1])
    if reference_time < times[0]:  # before the first checkpoint: extend the first segment
        slope = (log_likelihoods[1] - log_likelihoods[0]) / (times[1] - times[0])
        return float(log_likelihoods[0] + (reference_time - times[0]) * slope)
    return float(np.interp(reference_time, times, log_likelihoods))


def render(headers, rows, n_text):
    """Aligned plain-text table: text columns left-aligned, numbers right-aligned, best value per row in bold
    (bold only on a terminal). rows: lists of [text cells..., values...], or None for a blank line."""
    bold = sys.stdout.isatty()
    cells = [r[:n_text] + [f"{v:.4f}" for v in r[n_text:]] if r else None for r in rows]
    widths = [max(len(c[i]) for c in [headers] + [c for c in cells if c]) for i in range(len(headers))]
    rule = "-" * (sum(widths) + 3 * (len(widths) - 1))
    out = [rule, "   ".join(h.ljust(w) if i < n_text else h.rjust(w) for i, (h, w) in enumerate(zip(headers, widths))), rule]
    for r, c in zip(rows, cells):
        if r is None:
            out.append("")
            continue
        best = max(r[n_text:])
        line = []
        for i, (text, w) in enumerate(zip(c, widths)):
            text = text.ljust(w) if i < n_text else text.rjust(w)
            line.append(f"\033[1m{text}\033[0m" if bold and i >= n_text and r[i] == best else text)
        out.append("   ".join(line).rstrip())
    return "\n".join(out + [rule])


def compute_results(csv_dir):
    """{(label, dataset, timestamp): (seconds, [log-likelihood / 1e8 in METHODS order])}"""
    results = {}
    for label, dataset, topics, t1_iter in DATASETS:
        curves = {m: load_curve(csv_dir, dataset, topics, p) for m, p in METHODS.items()}
        svi_iters, svi_times, _ = curves["SFDB-SVI"]
        if t1_iter not in svi_iters:
            sys.exit(f"{dataset}: SFDB-SVI has no checkpoint at iteration {t1_iter} (T1); checkpoints: {svi_iters}")
        timestamps = {"T1": svi_times[svi_iters.index(t1_iter)], "T2": curves["Mallet"][1][-1]}
        for ts, t in timestamps.items():
            results[(label, dataset, ts)] = (int(t // 1000), [loglik_at(times, ll, t) / 1e8 for _, times, ll in curves.values()])
    return results


def main():
    results = compute_results(sys.argv[1] if len(sys.argv) > 1 else DEFAULT_CSV_DIR)
    note = "Log-likelihood values are scaled by 10^8. Higher is better."

    rows = []
    for (label, _, ts), (s, values) in results.items():
        if ts == "T1" and rows:
            rows.append(None)
        rows.append([label if ts == "T1" else "", f"{ts} ({s} s)"] + values)
    print("Table 1 (reproduced)\n")
    print(render(["Dataset", "Timestamp"] + list(METHODS), rows, 2))
    print("Note: Log-likelihood values are scaled by 10^8.\n")
    print("Log-likelihood evaluated at timestamps T1 and T2 for PubMed and Wikipedia datasets. "
          "Higher log-likelihood values are more desirable.")

    rows = []
    for (label, dataset, ts), (s, values) in results.items():
        if rows:
            rows.append(None)
        paper_s, paper = PAPER[dataset][ts]
        rows.append([label if ts == "T1" else "", f"{ts} ({s} s)", "Reproduced"] + values)
        rows.append(["", f"{ts} ({paper_s} s)", "Paper"] + paper)
    print("\n\nTable 1: reproduced vs paper\n")
    print(render(["Dataset", "Timestamp", "Source"] + list(METHODS), rows, 3))
    print(note)


if __name__ == "__main__":
    main()
