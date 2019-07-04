#!/usr/bin/env python

# Select Tensorflow as backend for Keras
from os import environ
import os 
environ['KERAS_BACKEND'] = 'tensorflow'
# Set architecture of system (AVX instruction set is not supported on SWAN)
environ['THEANO_FLAGS'] = 'gcc.cxxflags=-march=corei7'
from ROOT import TMVA, TFile, TTree, TCut,TChain
from subprocess import call
from os.path import isfile
from keras.models import Sequential
from keras.layers.core import Dense
from keras.optimizers import Adam
from keras.layers import Dropout
from keras.callbacks import EarlyStopping
from keras.utils.vis_utils import plot_model

# Setup TMVA
#os.system('rm -r mynnmodels')
#os.system('mkdir mynnmodels')
TMVA.PyMethodBase.PyInitialize()
output = TFile.Open('GGFKiller.root', 'RECREATE')
factory = TMVA.Factory('TMVAClassification', output,
        '!V:!Silent:Color:DrawProgressBar:Transformations=I:AnalysisType=Classification')
#Locate and add data files
file_VBF_HH_2016 = "../inputsamples/2016/SKIM_VBFHHTo4B_CV_1_C2V_1_C3_1_13TeV-madgraph.root"
file_VBF_HH_2017 = "../inputsamples/2017/SKIM_VBFHHTo4B_CV_1_C2V_1_C3_1_13TeV-madgraph.root"
file_GGF_HH_2016 = "../inputsamples/2016/SKIM_GluGluToHHTo4B_node_SM_13TeV-madgraph.root"
file_GGF_HH_2017 = "../inputsamples/2017/SKIM_GluGluToHHTo4B_node_SM_13TeV-madgraph_correctedcfg.root"
ch_sig = TChain("bbbbTree")
ch_bkg = TChain("bbbbTree")
ch_sig.AddFile(file_VBF_HH_2016)
ch_sig.AddFile(file_VBF_HH_2017) 
#ch_sig.AddFile(file_VBF_HH_BSM_2016)
#ch_sig.AddFile(file_VBF_HH_BSM_2017)   
ch_bkg.AddFile(file_GGF_HH_2016)
ch_bkg.AddFile(file_GGF_HH_2017)
#Load data to TMVA
dataloader = TMVA.DataLoader('GGFKiller')
dataloader.AddVariable("abs_H1_eta:=abs(H1_eta)")
dataloader.AddVariable("abs_H2_eta:=abs(H2_eta)")
dataloader.AddVariable("H1_pt")
dataloader.AddVariable("H2_pt")
dataloader.AddVariable("JJ_j1_pt")
dataloader.AddVariable("JJ_j2_pt")
dataloader.AddVariable("abs_JJ_eta:=abs(JJ_eta)")
dataloader.AddVariable("h1h2_deltaEta")
dataloader.AddVariable("h1j1_deltaR")
dataloader.AddVariable("h1j2_deltaR")
dataloader.AddVariable("h2j1_deltaR")
dataloader.AddVariable("h2j2_deltaR")
dataloader.AddVariable("abs_j1etaj2eta:=abs(j1etaj2eta)")
dataloader.AddVariable("abs_costh_HH_b1_cm:=abs(costh_HH_b1_cm)")
dataloader.AddVariable("abs_costh_HH_b2_cm:=abs(costh_HH_b2_cm)")

trainingsamplefraction = 0.50
nTrain_Signal          = int(ch_sig.GetEntries('VBFEvent==1')*trainingsamplefraction) 
nTrain_Background      = int(ch_bkg.GetEntries('VBFEvent==1')*trainingsamplefraction)

print("[INFO] ML TRAINING STARTING . . .")
print("[INFO] Signal/Background Training Fraction is %f"%trainingsamplefraction)
dataloader.AddSignalTree(ch_sig, 1.0)
dataloader.AddBackgroundTree(ch_bkg, 1.0)
dataloader.PrepareTrainingAndTestTree(TCut('VBFEvent==1'),'nTrain_Signal=%i:nTrain_Background=%i:SplitMode=Random:!V:SplitSeed=100'%(nTrain_Signal,nTrain_Background))

#print("[INFO] Deep Neural Network Training Starting . . .")
#Method: DNN
#nTrain_batchsize       = int(nTrain_Signal/10)
#nodes =[100]
#nlayers = [3]##
#for i in nodes:
#  for j in nlayers:
#       model = Sequential()
#       model.add(Dense(i, kernel_initializer='glorot_normal', activation='relu', input_dim=15))
#       count = 0 
#       while count < j-1: 
#       	 model.add( Dropout(0.20, noise_shape=None, seed=100)   ) 
#         model.add(Dense(i, kernel_initializer='glorot_normal', activation='relu'))
#         count += 1         
#       model.add( Dropout(0.20, noise_shape=None, seed=100)   ) 
#       model.add(Dense(2, activation='softmax'))
#       # Set loss and optimizer
#       model.compile(loss='categorical_crossentropy', optimizer=Adam(lr=0.001), metrics=['accuracy',])
#       # Store model to file
#       model.save('mynnmodels/model_nn_%i_%i.h5'%(j,i))
#       model.summary()
#       #Book DNN method
#       factory.BookMethod(dataloader, TMVA.Types.kPyKeras, 'NN-%ilayers-%i'%(j,i) ,
#             'H:!V:VarTransform=G:FilenameModel=mynnmodels/model_nn_%i_%i.h5:NumEpochs=50:TriesEarlyStopping=3:BatchSize=%i'%(j,i,nTrain_batchsize))
#       plot_model(model, 'mynnmodels/NN-%ilayers-%i.eps'%(j,i), show_shapes=True)

print("[INFO] Boosted Decision Tree Training Starting . . .")
#nTrees =[230,240]
#nCuts = [200,250]
#nDepth = [2]

nTrees =[240]
nCuts = [200]
nDepth = [2]
for i in nTrees:
  for j in nCuts:
    for k in nDepth:
        factory.BookMethod(dataloader, TMVA.Types.kBDT, 'BDT_%i_%i_%i'%(i,j,k),
          '!H:!V:NTrees=%i:MinNodeSize=2.5:BoostType=Grad:Shrinkage=0.05:UseBaggedBoost:BaggedSampleFraction=0.5:nCuts=%i:MaxDepth=%i'%(i,j,k))


# Run training, test and evaluation
factory.TrainAllMethods()
factory.TestAllMethods()
factory.EvaluateAllMethods()