#!/usr/bin/env python3
"""Writes the log-likelihood tables computed by make_loglik_table.py as LaTeX and compiles them to PDF.

  * Table 1: reproduced values, in the layout of the paper's Table 1
  * Table 2: reproduced values next to the values reported in the paper

Usage:  python3 report/make_latex_table.py [csv_dir]   (default: benchmarks/csv)
Output: report/loglik_tables.tex and report/loglik_tables.pdf
"""
import os
import shutil
import subprocess
import sys

REPORT_DIR = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(REPORT_DIR, "..", "scripts"))
from make_loglik_table import DEFAULT_CSV_DIR, METHODS, PAPER, compute_results  # noqa: E402

CAPTION_1 = (r"Log-likelihood evaluated at timestamps \textit{T1} and \textit{T2} for PubMed and Wikipedia datasets. "
             r"Higher log-likelihood values are more desirable.")
CAPTION_2 = (r"Reproduced log-likelihood values of Table~1 compared with the values reported in the paper. "
             r"\textit{T1} and \textit{T2} are derived from the measured running times of each set of runs. "
             r"Higher log-likelihood values are more desirable.")
NOTE = r"\multicolumn{%d}{l}{\footnotesize Note: Log-likelihood values are scaled by $10^8$.}"

PREAMBLE = r"""\documentclass{article}
\usepackage[margin=2cm]{geometry}
\usepackage{booktabs}
\usepackage{multirow}
\usepackage[labelfont=bf,labelsep=period]{caption}
\pagestyle{empty}
\begin{document}
"""


def cells(values):
    """Values formatted like the paper: 4 decimals, typographic minus, best value in bold."""
    best = max(values)
    out = []
    for v in values:
        s = ("$-$" if v < 0 else "") + f"{abs(v):.4f}"
        out.append("\\textbf{" + s + "}" if v == best else s)
    return " & ".join(out)


def header(first_columns):
    methods = ["\\textbf{" + m + "}" if m == "SFDB-SVI" else m for m in METHODS]
    return " & ".join(["\\textbf{" + c + "}" for c in first_columns] + methods) + r" \\"


def reproduced_table(results):
    lines = [r"\begin{tabular}{ll" + "r" * len(METHODS) + "}", r"\toprule", header(["Dataset", "Timestamp"]), r"\midrule"]
    for i, ((label, _, ts), (s, values)) in enumerate(results.items()):
        if ts == "T1" and i > 0:
            lines.append(r"\addlinespace")
        first = r"\multirow{2}{*}{" + label + "}" if ts == "T1" else ""
        lines.append(f"{first} & {ts} ({s} s) & {cells(values)} \\\\")
    lines += [r"\bottomrule", NOTE % (2 + len(METHODS)), r"\end{tabular}"]
    return lines


def comparison_table(results):
    lines = [r"\begin{tabular}{lll" + "r" * len(METHODS) + "}", r"\toprule", header(["Dataset", "Timestamp", "Source"]), r"\midrule"]
    for i, ((label, dataset, ts), (s, values)) in enumerate(results.items()):
        if ts == "T1" and i > 0:
            lines.append(r"\midrule")
        elif ts == "T2":
            lines.append(r"\addlinespace")
        paper_s, paper = PAPER[dataset][ts]
        first = r"\multirow{4}{*}{" + label + "}" if ts == "T1" else ""
        lines.append(f"{first} & {ts} ({s} s) & Reproduced & {cells(values)} \\\\")
        lines.append(f" & {ts} ({paper_s} s) & Paper & {cells(paper)} \\\\")
    lines += [r"\bottomrule", NOTE % (3 + len(METHODS)), r"\end{tabular}"]
    return lines


def table(tabular, caption):
    return [r"\begin{table}[h]", r"\centering"] + tabular + [r"\caption{" + caption + "}", r"\end{table}"]


def main():
    csv_dir = sys.argv[1] if len(sys.argv) > 1 else DEFAULT_CSV_DIR
    results = compute_results(csv_dir)

    out_dir = REPORT_DIR
    tex = os.path.join(out_dir, "loglik_tables.tex")
    body = table(reproduced_table(results), CAPTION_1) + [r"\vspace{1cm}"] + table(comparison_table(results), CAPTION_2)
    with open(tex, "w") as f:
        f.write(PREAMBLE + "\n".join(body) + "\n\\end{document}\n")
    print(f"Wrote {tex}")

    if shutil.which("pdflatex") is None:
        print("pdflatex not found: compile the .tex file with pdflatex to get the PDF")
        return
    run = subprocess.run(["pdflatex", "-interaction=nonstopmode", "-halt-on-error", "loglik_tables.tex"],
                         cwd=out_dir, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    if run.returncode != 0:
        sys.exit(f"pdflatex failed, see {os.path.join(out_dir, 'loglik_tables.log')}")
    for ext in (".aux", ".log"):
        os.remove(os.path.join(out_dir, "loglik_tables" + ext))
    print(f"Wrote {os.path.join(out_dir, 'loglik_tables.pdf')}")


if __name__ == "__main__":
    main()
