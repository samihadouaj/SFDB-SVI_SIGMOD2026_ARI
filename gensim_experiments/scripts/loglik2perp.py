#!/bin/python3

from mpmath import mp
import csv
import argparse

parser = argparse.ArgumentParser(description="...", formatter_class=argparse.ArgumentDefaultsHelpFormatter)
parser.add_argument("--srcFile", help="path to input CSV file with two columns (iterationId, logLikelihood)")
parser.add_argument("--outFile", help="output file path")
parser.add_argument("--numTokens", type=int, help="number of tokens in the dataset of interest")
args = parser.parse_args()
config = vars(args)

srcFile = config["srcFile"]
outFile = config["outFile"]
#numTokens = mp.mpf(config["numTokens"])
numTokens = int(config["numTokens"])

if1 = open(srcFile)
csvreader = csv.reader(if1)
of1 = open(outFile, "w")
for row in csvreader:
    of1.write(row[0]+","+str(mp.exp(-mp.mpf(row[1])/numTokens))+"\n")
of1.close()
if1.close()

#EOF
