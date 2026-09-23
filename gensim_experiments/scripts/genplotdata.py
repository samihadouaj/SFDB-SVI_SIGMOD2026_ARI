#!/bin/python3

import pandas as pd
import argparse

parser = argparse.ArgumentParser(description="...", formatter_class=argparse.ArgumentDefaultsHelpFormatter)
parser.add_argument("--timingFile", help="path to input CSV file with three columns (runId, iterationId, time)")
parser.add_argument("--perplexityFile", help="path to input CSV file with two columns ( iterationId, perplexity)")
parser.add_argument("--outFile", help="output file path")
args = parser.parse_args()
config = vars(args)

timingFile = config["timingFile"]
perplexityFile = config["perplexityFile"]
outFile = config["outFile"]

timecsv = pd.read_csv(timingFile, header=None, names=['runID','iterID','time'])
perplexitycsv = pd.read_csv(perplexityFile, header=None, names=['iterID','perplexity'])

timecsv['time_cumsum'] = timecsv.groupby('runID')['time'].transform(pd.Series.cumsum)
avgtime = timecsv.groupby(['iterID'])['time_cumsum'].mean()

plot_data = pd.merge(avgtime, perplexitycsv, on="iterID")

plot_data[['time_cumsum', 'perplexity']].to_csv(outFile, header=False, index=None)

#EOF

