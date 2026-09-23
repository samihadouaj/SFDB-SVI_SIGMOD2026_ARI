# Author: Sami Hadouaj
# Date: 09/26/2023
# Description: This script take as input the temporary files produced by each process and merges them into a single file to create the final loglik file

import glob
import argparse

parser = argparse.ArgumentParser()
parser.add_argument('--OUTPUT_DIR', type=str, help='')
parser.add_argument('--DATASET_NAME', type=str, help='')
parser.add_argument('--TRAIN_OR_TEST', type=str, help='')
parser.add_argument('--NUM_TOPICS', type=int, help='')
parser.add_argument('--ALPHA_PRIOR', type=float,  help='')
parser.add_argument('--BETA_PRIOR', type=float,  help='')
parser.add_argument('--NUM_ITERATIONS',  help='')
parser.add_argument('--NUM_THREADS', type=int,  help='')
parser.add_argument('--RND_SEED', type=int,  help='')
parser.add_argument('--MALLET_OR_GAMMAPDB', type=str,  help='')


args = parser.parse_args()
OUTPUT_DIR = args.OUTPUT_DIR
DATASET_NAME = args.DATASET_NAME
TRAIN_OR_TEST = args.TRAIN_OR_TEST
NUM_TOPICS=args.NUM_TOPICS
ALPHA_PRIOR=args.ALPHA_PRIOR
BETA_PRIOR=args.BETA_PRIOR
NUM_ITERATIONS=args.NUM_ITERATIONS
NUM_THREADS=args.NUM_THREADS
RND_SEED=args.RND_SEED
MALLET_OR_GAMMAPDB=args.MALLET_OR_GAMMAPDB




dir = OUTPUT_DIR+"/csv/"+DATASET_NAME+"_TEMP_"+TRAIN_OR_TEST+"_"+str(NUM_TOPICS)+"topics_A"+str(ALPHA_PRIOR)+"_B"+str(BETA_PRIOR)+"_NI"+str(NUM_ITERATIONS)+"_NT"+str(NUM_THREADS)+"_RND"+str(RND_SEED)+"_"+str(MALLET_OR_GAMMAPDB)+"_loglik"

print(dir)
print(DATASET_NAME)
# Get list of all files you want to read
files = glob.glob(f'{dir}*')


data = []
for file_name in files:
    with open(file_name, 'r') as f:
        for line in f:
            # Split each line into its components parts
            parts = line.strip().split(',')
            # Append as a tuple pair to data
            data.append((int(parts[0]), float(parts[1])))

# Sort data based on first column
data.sort(key=lambda x: x[0])

# Write data to a single file
outputNAME = OUTPUT_DIR+"/csv/"+DATASET_NAME+"_"+TRAIN_OR_TEST+"_"+str(NUM_TOPICS)+"topics_A"+str(ALPHA_PRIOR)+"_B"+str(BETA_PRIOR)+"_NI"+str(NUM_ITERATIONS)+"_NT"+str(NUM_THREADS)+"_RND"+str(RND_SEED)+"_"+str(MALLET_OR_GAMMAPDB)+"_loglik.csv"
with open(outputNAME, 'w') as f:
    for item in data:
        f.write(f'{item[0]},{item[1]}\n')