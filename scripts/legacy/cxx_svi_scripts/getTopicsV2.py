import itertools
import numpy as np
import sys

PathToVocab=sys.argv[1]
# PathToData="ToRemove"

outputPath=sys.argv[2]
# outputPath = "ToRemove"

f = open(outputPath+"/topicProbs.txt","r")
topicProb=f.readlines()

f =open(outputPath+"/redDie.txt")
redDie = f.readlines()

lda_stateOK = open(outputPath+"/lda-state.OK",'r')
lda_stateOK = lda_stateOK.readlines()

vocab  = open(PathToVocab,'r')
vocab = vocab.readlines()

lda_state = open(outputPath+'/lda_state.txt','w+')
print("length of LDA_stateOK is" + str(len(lda_stateOK)))
print("length of topicProb is" + str(len(topicProb)))


countOfZeroThetas=0
for docID in range(len(redDie)):
    temp = redDie[docID].split(" ")[:-1]
    temp = [float(i) for i in temp]
    sumTemp = sum(temp)
    try:
        temp = [i/sumTemp for i in temp]
    except:
        countOfZeroThetas+=1
        temp = [0 for i in temp]
    redDie[docID] = temp
    
print("countOfZeroThetas= ",countOfZeroThetas) 

vocab = {v.replace("\n",""):k for k,v in enumerate(vocab)}
print(len(vocab))
for wid,el in enumerate(topicProb):
    el = el.split(" ")
    el.remove("\n")
    s = [float(i) for i in el]
    sumS = sum(s)
    s = [i/sumTemp for i in s]
    topicProb[wid] = s
    
    

c = 0
for i in range(len(topicProb)):
    if(sum(topicProb[i]) == 0):
        c+=1
print(c)




count = 0
for line in lda_stateOK[3:]:
    #if ("#" in line) :
     #   continue
#     print(line)
    temp = line.split(" ")
    try:
        # print(line)
        # print(temp)
        currentDocId = int(temp[0])
        currentWord = temp[4]
        probs = np.array(redDie[currentDocId])*np.array(topicProb[vocab[currentWord]])
        topic = np.argmax(probs)
    # zebi = zebi+1
    except Exception as e:
        # print(e)
        # print(currentDocId)
        # print(currentWord)
        count=count+1
        topic = 1
    lda_state.write('%d %s %d %d %s %d \n' %(int(temp[0]),temp[1], int(temp[2]), int(temp[3]),temp[4], int(topic)))
lda_state.close()

print ("had problems with :", count, "tokens out of ", len(lda_stateOK))
