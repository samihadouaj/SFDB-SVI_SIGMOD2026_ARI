#!/bin/bash

# List of file paths
files=(
../benchmarks/csv/NYTIMES_test_50topics_A0.2_B0.1_NI20_NT1_RND123_gammapdb_lda-inmemory-vrexpr_plotdata.csv
../benchmarks/csv/NYTIMES_test_50topics_A0.2_B0.1_NI20_NT1_RND123_mallet_plotdata.csv
../benchmarks/csv/NYTIMES_test_50topics_A0.2_B0.1_NI20_NT2_RND123_gammapdb_lda-inmemory-vrexprP_plotdata.csv
../benchmarks/csv/NYTIMES_test_50topics_A0.2_B0.1_NI20_NT2_RND123_mallet_plotdata.csv
../benchmarks/csv/NYTIMES_test_50topics_A0.2_B0.1_NI20_NT4_RND123_gammapdb_lda-inmemory-vrexprP_plotdata.csv
../benchmarks/csv/NYTIMES_test_50topics_A0.2_B0.1_NI20_NT4_RND123_mallet_plotdata.csv
../benchmarks/csv/NYTIMES_test_50topics_A0.2_B0.1_NI20_NT8_RND123_gammapdb_lda-inmemory-vrexprP_plotdata.csv
../benchmarks/csv/NYTIMES_test_50topics_A0.2_B0.1_NI20_NT8_RND123_mallet_plotdata.csv
../benchmarks/csv/NYTIMES_train_50topics_A0.2_B0.1_NI20_NT1_RND123_gammapdb_lda-inmemory-vrexpr_plotdata.csv
../benchmarks/csv/NYTIMES_train_50topics_A0.2_B0.1_NI20_NT1_RND123_mallet_plotdata.csv
../benchmarks/csv/NYTIMES_train_50topics_A0.2_B0.1_NI20_NT2_RND123_gammapdb_lda-inmemory-vrexprP_plotdata.csv
../benchmarks/csv/NYTIMES_train_50topics_A0.2_B0.1_NI20_NT2_RND123_mallet_plotdata.csv
../benchmarks/csv/NYTIMES_train_50topics_A0.2_B0.1_NI20_NT4_RND123_gammapdb_lda-inmemory-vrexprP_plotdata.csv
../benchmarks/csv/NYTIMES_train_50topics_A0.2_B0.1_NI20_NT4_RND123_mallet_plotdata.csv
../benchmarks/csv/NYTIMES_train_50topics_A0.2_B0.1_NI20_NT8_RND123_gammapdb_lda-inmemory-vrexprP_plotdata.csv
../benchmarks/csv/NYTIMES_train_50topics_A0.2_B0.1_NI20_NT8_RND123_mallet_plotdata.csv
../benchmarks/csv/PUBMED_test_50topics_A0.2_B0.1_NI20_NT1_RND123_gammapdb_lda-inmemory-vrexpr_plotdata.csv
../benchmarks/csv/PUBMED_test_50topics_A0.2_B0.1_NI20_NT1_RND123_mallet_plotdata.csv
../benchmarks/csv/PUBMED_test_50topics_A0.2_B0.1_NI20_NT2_RND123_gammapdb_lda-inmemory-vrexprP_plotdata.csv
../benchmarks/csv/PUBMED_test_50topics_A0.2_B0.1_NI20_NT2_RND123_mallet_plotdata.csv
../benchmarks/csv/PUBMED_test_50topics_A0.2_B0.1_NI20_NT4_RND123_gammapdb_lda-inmemory-vrexprP_plotdata.csv
../benchmarks/csv/PUBMED_test_50topics_A0.2_B0.1_NI20_NT4_RND123_mallet_plotdata.csv
../benchmarks/csv/PUBMED_test_50topics_A0.2_B0.1_NI20_NT8_RND123_gammapdb_lda-inmemory-vrexprP_plotdata.csv
../benchmarks/csv/PUBMED_test_50topics_A0.2_B0.1_NI20_NT8_RND123_mallet_plotdata.csv
../benchmarks/csv/PUBMED_train_50topics_A0.2_B0.1_NI20_NT1_RND123_gammapdb_lda-inmemory-vrexpr_plotdata.csv
../benchmarks/csv/PUBMED_train_50topics_A0.2_B0.1_NI20_NT1_RND123_mallet_plotdata.csv
../benchmarks/csv/PUBMED_train_50topics_A0.2_B0.1_NI20_NT2_RND123_gammapdb_lda-inmemory-vrexprP_plotdata.csv
../benchmarks/csv/PUBMED_train_50topics_A0.2_B0.1_NI20_NT2_RND123_mallet_plotdata.csv
../benchmarks/csv/PUBMED_train_50topics_A0.2_B0.1_NI20_NT4_RND123_gammapdb_lda-inmemory-vrexprP_plotdata.csv
../benchmarks/csv/PUBMED_train_50topics_A0.2_B0.1_NI20_NT4_RND123_mallet_plotdata.csv
../benchmarks/csv/PUBMED_train_50topics_A0.2_B0.1_NI20_NT8_RND123_gammapdb_lda-inmemory-vrexprP_plotdata.csv
../benchmarks/csv/PUBMED_train_50topics_A0.2_B0.1_NI20_NT8_RND123_mallet_plotdata.csv
)

# Flag to check if all files exist
all_files_exist=true

# Loop through each file and check if it exists
for file in "${files[@]}"
do
    if [ ! -f "$file" ]; then
        echo "The following csv file is missing: $file"
        all_files_exist=false
    fi
done

# If all files exist, run tt.sh
if $all_files_exist; then
    echo "All files are present. Running tt.sh..."
    pdflatex 50TopicsExpReport.tex
fi


rm *.log
rm *.aux