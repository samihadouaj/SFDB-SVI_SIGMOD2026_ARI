import numpy as np
import csv
from hmmlearn import hmm

def normalizeByRows(arr):
   return arr/(np.tile(sum(arr.transpose()),(arr.shape[1],1)).transpose())

def getModel(modelNo):
   if (modelNo==1):
      startprob = np.array([[50, 50, 1]])
      transmat = np.array([[30, 75, 1],[75, 30, 1],[1, 1, 98]]) 
      emissionprob = np.array([[1000, 1, 1],[500, 500, 1],[1, 1, 1000]])
      return normalizeByRows(startprob).flatten(), normalizeByRows(transmat), normalizeByRows(emissionprob)
   elif (modelNo==2):
      startprob = np.array([[1, 1, 1]])
      transmat = np.array([[10, 85, 5],[5, 70, 25],[85, 10, 5]])
      emissionprob = np.array([[1000, 1, 1],[1, 1000, 1],[1, 1, 1000]])
      return normalizeByRows(startprob).flatten(), normalizeByRows(transmat), normalizeByRows(emissionprob)
   elif (modelNo==3):
      startprob = np.array([[1, 1, 98]])
      transmat = np.array([[10, 5, 85],[85, 10, 5],[5, 85, 10]])
      emissionprob = np.array([[1000, 1, 1],[1, 1000, 1],[1, 1, 1000]])
      return normalizeByRows(startprob).flatten(), normalizeByRows(transmat), normalizeByRows(emissionprob)
   else:
      raise Exception("Invalid modelNo")

def readNpArrayFromCsv(filename):
   matr = []
   with open(filename, 'r') as f:
      reader = csv.reader(f)
      for row in reader:
         intlist = [int(i) for i in row]
         matr.append(intlist)
   arr = np.array(matr)
   return arr


def parametrizeHmmModel(T, E):
   model = hmm.CategoricalHMM(n_components=3, random_state=81)
   model.startprob_=normalizeByRows(np.array([[1, 1, 1]])).flatten()
   model.transmat_=T
   model.emissionprob_=E
   return model

def getHmmModel(modelNo, random_state=None):
   S,T,E = getModel(modelNo)
   model = hmm.CategoricalHMM(n_components=3, random_state=random_state)
   model.startprob_=S
   model.transmat_=T
   model.emissionprob_=E
   return model

def getSamples(modelNo, rnd_state=None, seqLength=10, numSamples=1000):
   """
   Samples a collection of sequences from one of the HMM models.

   :param modelNo: which model are we sampling from? (either 1, 2 or 3)
   :param seqLength: what is the desired length of the sequences?
   :param numSamples: how many sequences do we want to generate?
   :return: a tuple (X, Z, I), where X is a numpy.array with shape ((numSamples*seqLength),1) where all the sequences are concatenated, Z has the same shape as X and stores the hidden states associated with each observable symbol from X, I is a linear numpy.array with numSamples elements, that stores the lenght of each sequence in X (which is always equal to seqLength).
   """ 
   model = getHmmModel(modelNo, random_state=rnd_state)
   X0, Z0 = model.sample(seqLength, random_state=rnd_state)
   for i in range(numSamples-1):
      X, Z = model.sample(seqLength)
      X0 = np.concatenate((X0,X))
      Z0 = np.concatenate((Z0,Z))
   return X0, Z0, np.array([seqLength]*numSamples)

def saveSamplesAsCsv(X, I, n_components=3, filename="hmm_data.csv"):
   cursor = 0
   rowId = 0
   seqId = 0
   outfile = open(filename, "w")
   # print header
   outfile.write("c0")
   for i in range(1,17):
      outfile.write(",c")
      outfile.write(str(i))
   outfile.write("\n")
   # print content
   for seqLength in I:
      seq = X[cursor:(cursor+seqLength)].flatten()
      pos = 0
      for symb in seq:
         for chs in range(n_components):
            outfile.write(str(rowId))  #c00
            outfile.write(",")
            outfile.write(str(seqId))  #c01
            outfile.write(",")
            outfile.write(str(pos))    #c02
            outfile.write(",")
            outfile.write(str(symb))   #c03
            outfile.write(",")
            outfile.write(str(chs))    #c04
            outfile.write(",")
            ecode = (3*chs)+symb
            outfile.write(str(ecode))  #c05
            outfile.write(",")
            eobsid = (seqId*seqLength)+pos
            outfile.write(str(eobsid)) #c06
            outfile.write(",")
            if (pos==0):
               outfile.write("-1")     # c07
            else:
               tobsid = (seqId*(seqLength-1))+pos-1
               outfile.write(str(tobsid)) 
            for phs in range(n_components):
               outfile.write(",")
               outfile.write(str(phs)) #c08, #c11, #c14
               outfile.write(",")
               if (pos==0):
                  outfile.write("-1")  #c09, #c12, #c15
                  outfile.write(",")
                  outfile.write("-1")  #c10, #c13, #c16
               else:
                  tcode = (3*phs)+chs
                  outfile.write(str(tcode)) 
                  outfile.write(",")
                  outfile.write(str((n_components-phs)+chs))
            rowId = rowId + 1
            outfile.write("\n")
         pos = pos+1
         if (pos==seqLength):
            outfile.write(str(rowId))
            outfile.write(",")
            outfile.write(str(seqId))
            outfile.write(",")
            outfile.write("-1") # str(pos)
            outfile.write(",")
            outfile.write("-1") # str(symb)
            outfile.write(",")
            outfile.write("-1") # str(chs)
            outfile.write(",")
            outfile.write("-1") # str(ecode)
            outfile.write(",")
            outfile.write("-1") # str(eobsid)
            outfile.write(",")
            outfile.write("-1") # str(tcode)
            for phs in range(n_components):
               outfile.write(",")
               outfile.write(str(phs))
               #
               outfile.write(",")
               outfile.write("-1") # str(tobsid)
               outfile.write(",")
               outfile.write(str(n_components-phs))
            rowId = rowId + 1
            outfile.write("\n")
      # move to next sequence
      cursor = cursor + seqLength
      seqId = seqId + 1
   outfile.flush()
   outfile.close()

def getDataset(modelNo, random_state=None, seqLength=10, numTrainingSamples=1000, numTestSamples=10):
   Xtrain, Ztrain, Itrain = getSamples(modelNo, random_state, seqLength, numTrainingSamples)
   Xtest,  Ztest,  Itest  = getSamples(modelNo, random_state, seqLength, numTestSamples)
   return Xtrain, Itrain, Xtest, Itest


def getScores(model, Xtest, Itest):
   cursor=0
   scores = np.array([model.score(Xtest[cursor:Itest[0]])])
   for i in range(1,Itest.shape[0]):
      cursor = cursor + Itest[i-1]
      score = np.array([model.score(Xtest[cursor:(cursor+Itest[i])])])
      scores = np.concatenate((scores,score))
   return scores


def saveHmmTrainingSetAsCsv(ts1_filename, ts2_filename, ts3_filename):
   Xtrain1, Itrain1, Xtest1, Itest1 = getDataset(modelNo=1)
   Xtrain2, Itrain2, Xtest2, Itest2 = getDataset(modelNo=2)
   Xtrain3, Itrain3, Xtest3, Itest3 = getDataset(modelNo=3)
   #
   saveSamplesAsCsv(Xtrain1, Itrain1,  n_components=3, filename=ts1_filename)
   saveSamplesAsCsv(Xtrain2, Itrain2,  n_components=3, filename=ts2_filename)
   saveSamplesAsCsv(Xtrain3, Itrain3,  n_components=3, filename=ts3_filename)

def evalGammaPdbHmm(model1, model2, model3):
   Xtrain1, Itrain1, Xtest1, Itest1 = getDataset(modelNo=1)
   Xtrain2, Itrain2, Xtest2, Itest2 = getDataset(modelNo=2)
   Xtrain3, Itrain3, Xtest3, Itest3 = getDataset(modelNo=3)
   #
   scores1gt1 = getScores(model1, Xtest1, Itest1)
   scores2gt1 = getScores(model2, Xtest1, Itest1)
   scores3gt1 = getScores(model3, Xtest1, Itest1)
   #
   scores1gt2 = getScores(model1, Xtest2, Itest2)
   scores2gt2 = getScores(model2, Xtest2, Itest3)
   scores3gt2 = getScores(model3, Xtest2, Itest3)
   #
   scores1gt3 = getScores(model1, Xtest3, Itest3)
   scores2gt3 = getScores(model2, Xtest3, Itest3)
   scores3gt3 = getScores(model3, Xtest3, Itest3)
   #
   allScores1 = np.concatenate((scores1gt1,scores1gt2,scores1gt3))
   allScores2 = np.concatenate((scores2gt1,scores2gt2,scores2gt3))
   allScores3 = np.concatenate((scores3gt1,scores3gt2,scores3gt3))
   allScores = np.tile((allScores1, allScores2, allScores3), 1).transpose()
   pred = np.argmax(allScores, 1)+1
   #
   gt1 = np.array([1]*Itest1.shape[0])
   gt2 = np.array([2]*Itest2.shape[0])
   gt3 = np.array([3]*Itest3.shape[0])
   gt = np.concatenate((gt1, gt2, gt3))
   #
   GTvsPred = np.tile((gt, pred),1).transpose()
   _, C = np.unique((gt==pred), return_counts=True)
   precision = 1.0
   if (C.size>1):
      precision = C[1]/C.sum()
   return allScores, GTvsPred, precision

