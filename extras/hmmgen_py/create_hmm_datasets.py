import hmmgen
import os

dirname = os.path.dirname(__file__)
ts1_filename = os.path.join(dirname, '../../data/HmmSyn01_train.csv')
ts2_filename = os.path.join(dirname, '../../data/HmmSyn02_train.csv')
ts3_filename = os.path.join(dirname, '../../data/HmmSyn03_train.csv')

hmmgen.saveHmmTrainingSetAsCsv(ts1_filename, ts2_filename, ts3_filename)
