#!/usr/bin/env python

# Select Theano as backend for Keras
from os import environ
environ['KERAS_BACKEND'] = 'theano'

# Set architecture of system (AVX instruction set is not supported on SWAN)
environ['THEANO_FLAGS'] = 'gcc.cxxflags=-march=corei7'


from ROOT import TMVA, TFile, TTree, TCut,TChain
from subprocess import call
from os.path import isfile

from keras.models import Sequential
from keras.layers.core import Dense
from keras.optimizers import Adam
from keras.layers import Dropout
# Setup TMVA
TMVA.PyMethodBase.PyInitialize()

output = TFile.Open('BinaryClassificationKeras.root', 'RECREATE')
factory = TMVA.Factory('TMVAClassification', output,
        '!V:!Silent:Color:DrawProgressBar:Transformations=I:AnalysisType=Classification')
#Load Data
flist_VBF_HH_2016 = "/uscms/home/guerrero/nobackup/Run2/HH2019/newcommit/CMSSW_10_2_5/src/bbbbAnalysis/plotterListFiles/MLNonresonant/2016ML/SKIM_VBFHHTo4B_CV_1_C2V_1_C3_1_13TeV-madgraph.txt";
flist_GGF_HH_2016 = "/uscms/home/guerrero/nobackup/Run2/HH2019/newcommit/CMSSW_10_2_5/src/bbbbAnalysis/plotterListFiles/MLNonresonant/2016ML/SKIM_GluGluToHHTo4B_node_SM_13TeV-madgraph.txt";
flist_VBF_HH_2017 = "/uscms/home/guerrero/nobackup/Run2/HH2019/newcommit/CMSSW_10_2_5/src/bbbbAnalysis/plotterListFiles/MLNonresonant/2017ML/SKIM_VBFHHTo4B_CV_1_C2V_1_C3_1_13TeV-madgraph.txt";
flist_GGF_HH_2017 = "/uscms/home/guerrero/nobackup/Run2/HH2019/newcommit/CMSSW_10_2_5/src/bbbbAnalysis/plotterListFiles/MLNonresonant/2017ML/SKIM_GluGluToHHTo4B_node_SM_13TeV-madgraph_correctedcfg.txt";
ch_sig = TChain("bbbbTree")
ch_bkg = TChain("bbbbTree")


for line in file(flist_VBF_HH_2016):
  print "Adding file: " + line
  ch_sig.AddFile(line[:-1])
for line in file(flist_VBF_HH_2017):
  print "Adding file: " + line
  ch_sig.AddFile(line[:-1])  
for line in file(flist_GGF_HH_2016):
  print "Adding file: " + line
  ch_bkg.AddFile(line[:-1])
for line in file(flist_GGF_HH_2017):
  print "Adding file: " + line
  ch_bkg.AddFile(line[:-1])  

dataloader = TMVA.DataLoader('BinaryClassificationKeras')
dataloader.AddVariable("H1_eta")
dataloader.AddVariable("H2_eta")
dataloader.AddVariable("H1_pt")
dataloader.AddVariable("H2_pt")
#dataloader.AddVariable("HH_b1_pt")
#dataloader.AddVariable("HH_b2_pt")
dataloader.AddVariable("JJ_j1_pt")
dataloader.AddVariable("JJ_j1_qgl")
#dataloader.AddVariable("JJ_j1_eta")
dataloader.AddVariable("JJ_j2_pt")
dataloader.AddVariable("JJ_j2_qgl")
#dataloader.AddVariable("JJ_j2_eta")
#dataloader.AddVariable("JJ_m")
#dataloader.AddVariable("JJ_pt")
dataloader.AddVariable("JJ_eta")
#dataloader.AddVariable("HH_pt")
#dataloader.AddVariable("j1j2_deltaEta")
dataloader.AddVariable("h1h2_deltaEta")
dataloader.AddVariable("h1j1_deltaR")
dataloader.AddVariable("h1j2_deltaR")
dataloader.AddVariable("h2j1_deltaR")
dataloader.AddVariable("h2j2_deltaR")
#dataloader.AddVariable("maxj1etaj2eta")
dataloader.AddVariable("j1etaj2eta")
#dataloader.AddVariable("etacentrality")

#dataloader.AddVariable("H1_bb_deltaR")
#dataloader.AddVariable("H1_bb_deltaEta")
#dataloader.AddVariable("H1_bb_deltaPhi")
#dataloader.AddVariable("H2_bb_deltaR")
#dataloader.AddVariable("H2_bb_deltaEta")
#dataloader.AddVariable("H2_bb_deltaPhi")

#dataloader.AddVariable("JJ_j1_deepCSV")
#dataloader.AddVariable("ptcentrality")
#dataloader.AddVariable("nExtraJetendcap")
#dataloader.AddVariable("ExtraVBFJetEta")
#dataloader.AddVariable("ExtraVBFJetPt")
#dataloader.AddVariable("ptbalance")
#dataloader.AddVariable("nExtraJetbarrel")
#dataloader.AddVariable("nExtraJet")
#dataloader.AddVariable("ExtraVBFJetDecision")
#dataloader.AddVariable("VBFEvent")
#dataloader.AddVariable("JJ_j2_deepCSV") 
#dataloader.AddVariable("HH_eta")
#dataloader.AddVariable("h2j1_deltaEta")
#dataloader.AddVariable("h1j2_deltaEta")
#dataloader.AddVariable("h1h2_deltaPhi")
#dataloader.AddVariable("h1h2_deltaR")
#dataloader.AddVariable("h1j1_deltaEta")
#dataloader.AddVariable("h2j2_deltaPhi") 
#dataloader.AddVariable("j1j2_deltaEta")

#dataloader.AddVariable("h2j1_deltaEta") 
#dataloader.AddVariable("b1b2_deltaEta") 
#dataloader.AddVariable("b1b3_deltaEta") 
#dataloader.AddVariable("b1b4_deltaEta") 
#dataloader.AddVariable("b1j1_deltaEta") 
#dataloader.AddVariable("b1j2_deltaEta") 
#dataloader.AddVariable("b2b3_deltaEta") 
#dataloader.AddVariable("b2b4_deltaEta") 
#dataloader.AddVariable("b2j1_deltaEta") 
#dataloader.AddVariable("b2j2_deltaEta") 
#dataloader.AddVariable("b3b4_deltaEta") 
#dataloader.AddVariable("b3j1_deltaEta") 
#dataloader.AddVariable("b3j2_deltaEta") 
#dataloader.AddVariable("b4j1_deltaEta") 
#dataloader.AddVariable("b4j2_deltaEta") 
#dataloader.AddVariable("j1j2_deltaEta") 
#dataloader.AddVariable("h1h2_deltaEta") 
#dataloader.AddVariable("h1j1_deltaEta") 
#dataloader.AddVariable("h1j2_deltaEta")
#dataloader.AddVariable("h2j2_deltaEta") 
#dataloader.AddVariable("h1jj_deltaEta") 
#dataloader.AddVariable("h2jj_deltaEta") 
#dataloader.AddVariable("hhj1_deltaEta") 
#dataloader.AddVariable("hhj2_deltaEta") 
#dataloader.AddVariable("hhjj_deltaEta")  

dataloader.AddSignalTree(ch_sig, 1.0)
dataloader.AddBackgroundTree(ch_bkg, 1.0)
dataloader.PrepareTrainingAndTestTree(TCut('VBFEvent==1'),'nTrain_Signal=8988:nTrain_Background=12739:SplitMode=Random:!V')

#Dropout(0.01, noise_shape=None, seed=None)
# Define model
#nodes = [200,210,220,230,240]
#nodes =[70,80,100,110,120,130,140,150]
#nlayers = [1,2,3,4,5,6,7]
##nlayers = [1,2]
#for i in nodes:
#  for j in nlayers:
#       model = Sequential()
#       model.add(Dense(i, init='glorot_normal', activation='relu', input_dim=28))
#       #model.add(SimpleRNN(i, activation='tanh', use_bias=True, kernel_initializer='glorot_uniform', recurrent_initializer='orthogonal', bias_initializer='zeros', kernel_regularizer=None, recurrent_regularizer=None, bias_regularizer=None, activity_regularizer=None, kernel_constraint=None, recurrent_constraint=None, bias_constraint=None, dropout=0.0, recurrent_dropout=0.0, return_sequences=False, return_state=False, go_backwards=False, stateful=False, unroll=False))
#       count = 0 
#       while count < j-1: 
#         model.add(Dense(i, init='glorot_normal', activation='relu'))
#         count += 1
#       model.add(Dense(2, activation='softmax'))
#       # Set loss and optimizer
#       model.compile(loss='categorical_crossentropy', optimizer=Adam(lr=0.001), metrics=['accuracy',])
#       # Store model to file
#       model.save('model%i_%i.h5'%(j,i))
#       model.summary()
#       #Book DNN method
#       factory.BookMethod(dataloader, TMVA.Types.kPyKeras, 'DNN-%ilayers-%i'%(j,i) ,
#             'H:!V:VarTransform=G:FilenameModel=model%i_%i.h5:NumEpochs=10:BatchSize=500'%(j,i))

#Other other methods:BDT

#factory.BookMethod(dataloader, TMVA.Types.kBDT, 'BDT',
#        '!H:!V:NTrees=120:MinNodeSize=2.5%:BoostType=Grad:Shrinkage=0.05:UseBaggedBoost:BaggedSampleFraction=0.5:nCuts=1000:MaxDepth=3')

#nTrees =[140,160,180,200,220,240,260,280,300,320,340,360,380,400]
#nCuts = [500,600,700,800,900,1000,1100,1200,1300,1400]
#nDepth = [3,4]
#nTrees =[140,160,180,200,220,240,260,280]
#nCuts = [200,300,400,500,600,700]
#nDepth = [3]
nTrees =[280]
nCuts = [300]
nDepth = [3]

for i in nTrees:
  for j in nCuts:
    for k in nDepth:
        factory.BookMethod(dataloader, TMVA.Types.kBDT, 'BDT',
          '!H:!V:NTrees=%i:MinNodeSize=2.5:BoostType=Grad:Shrinkage=0.05:UseBaggedBoost:BaggedSampleFraction=0.5:nCuts=%i:MaxDepth=%i'%(i,j,k))

# Run training, test and evaluation
factory.TrainAllMethods()
factory.TestAllMethods()
factory.EvaluateAllMethods()
