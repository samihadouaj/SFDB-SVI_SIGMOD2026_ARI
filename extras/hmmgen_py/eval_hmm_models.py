import hmmgen
import os
import numpy as np
from hmmlearn import hmm

dirname = os.path.dirname(__file__)

tdie1_filename = os.path.join(dirname, '../../benchmarks/gammapdb_hmm_model_01_tdie.csv')
tdie2_filename = os.path.join(dirname, '../../benchmarks/gammapdb_hmm_model_02_tdie.csv')
tdie3_filename = os.path.join(dirname, '../../benchmarks/gammapdb_hmm_model_03_tdie.csv')

edie1_filename = os.path.join(dirname, '../../benchmarks/gammapdb_hmm_model_01_edie.csv')
edie2_filename = os.path.join(dirname, '../../benchmarks/gammapdb_hmm_model_02_edie.csv')
edie3_filename = os.path.join(dirname, '../../benchmarks/gammapdb_hmm_model_03_edie.csv')

transmat1 = hmmgen.normalizeByRows(hmmgen.readNpArrayFromCsv(tdie1_filename))
transmat2 = hmmgen.normalizeByRows(hmmgen.readNpArrayFromCsv(tdie2_filename))
transmat3 = hmmgen.normalizeByRows(hmmgen.readNpArrayFromCsv(tdie3_filename))

emissionprob1 = hmmgen.normalizeByRows(hmmgen.readNpArrayFromCsv(edie1_filename))
emissionprob2 = hmmgen.normalizeByRows(hmmgen.readNpArrayFromCsv(edie2_filename))
emissionprob3 = hmmgen.normalizeByRows(hmmgen.readNpArrayFromCsv(edie3_filename))

model1 = hmmgen.parametrizeHmmModel(transmat1, emissionprob1)
model2 = hmmgen.parametrizeHmmModel(transmat2, emissionprob2)
model3 = hmmgen.parametrizeHmmModel(transmat3, emissionprob3)

allScores_gpdb, GTvsPred_gpdb, precision_gpdb = hmmgen.evalGammaPdbHmm(model1, model2, model3)
print("Round1: StarfishDB accuracy: %.3f" % precision_gpdb)
allScores_gpdb, GTvsPred_gpdb, precision_gpdb = hmmgen.evalGammaPdbHmm(model1, model2, model3)
print("Round2: StarfishDB accuracy: %.3f" % precision_gpdb)
allScores_gpdb, GTvsPred_gpdb, precision_gpdb = hmmgen.evalGammaPdbHmm(model1, model2, model3)
print("Round3: StarfishDB accuracy: %.3f" % precision_gpdb)

