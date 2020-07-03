import numpy 
import os
import root_numpy
import pandas
import glob
import argparse
import sys
import ast
import ROOT
from  ConfigParser import *
from hep_ml import reweight
from root_numpy import root2array
from numpy.lib.recfunctions import stack_arrays
#My modules
import modules.datatools as data
import modules.selections as selector

def SelectRegionforBackgroundRejectionTraining(sample,btagregion,category,validation):
	datacr,datasrrest = selector.eventselection(sample,btagregion,category,'CR',validation)
	datasr,datarest   = selector.eventselection(datasrrest,btagregion,category,'SR',validation)
	return datacr,datasr

def SelectRegionforProductionModeTraining(sample,btagregion,category,validation):
	dataselected,datarest = selector.eventselection(sample,btagregion,category,'None',validation)
	return dataselected

def RunSignalSelection(signalsamples,lumi,case,directory,tag):
	os.system('mkdir mvatraining')
	os.system('mkdir mvatraining/%s'%case)
	os.system('mkdir mvatraining/%s/%s'%(case,directory) )	
	#First prepare data to train for background rejection
	for mcsample in signalsamples:
		#Get data and create panda dataframes with specific variables, a.k.a. slicing the data. Then, create background model based on control region data
		mcsampleframe = data.root2pandas(['outputskims/%s/%s/SKIM_%s.root'%(case,directory,mcsample)],'bbbbTree')
		#Add mixing weight to be used in the training
		mcsampleframe = data.AddMCWeight(mcsampleframe,lumi,'outputskims/%s/%s/SKIM_%s.root'%(case,directory,mcsample))
		if 'VBF_HH_CV_1_C2V_2_C3_1' in mcsample: mcsampleframe['MC_Weight']    = (2)*mcsampleframe['MC_Weight']
		if 'cHHH1' in mcsample: mcsampleframe['MC_Weight']                     = (1)*mcsampleframe['MC_Weight']
		if 'cHHH0' in mcsample: mcsampleframe['MC_Weight']                     = (0.47347)*mcsampleframe['MC_Weight']
		if 'cHHH2p45' in mcsample: mcsampleframe['MC_Weight']                  = (0.64026)*mcsampleframe['MC_Weight']
		if 'cHHH5' in mcsample: mcsampleframe['MC_Weight']                     =-(0.11373)*mcsampleframe['MC_Weight']
		#Slice the data sample to take only events with three/four b-tags among the two categories
		data_cr_4b_categ,data_sr_4b_categ = SelectRegionforBackgroundRejectionTraining(mcsampleframe,'4b','GGF',False)
		#Merge (concatenate) them
		print "[INFO] Saving mc signal sample for background rejection"
		data.pandas2root(data_sr_4b_categ,'bbbbTree', 'mvatraining/%s/%s/SKIM_%s_BR.root'%(case,directory,mcsample)  )
		del data_sr_4b_categ
	#Second prepare data to train for production mode
	for mcsample in signalsamples:
		#Get data and create panda dataframes with specific variables, a.k.a. slicing the data. Then, create background model based on control region data
		mcsampleframe = data.root2pandas(['outputskims/%s/%s/SKIM_%s.root'%(case,directory,mcsample)],'bbbbTree')
		#Add mixing weight to be used in the training
		mcsampleframe = data.AddMCWeight(mcsampleframe,lumi,'outputskims/%s/%s/SKIM_%s.root'%(case,directory,mcsample))
		if 'VBF_HH_CV_1_C2V_2_C3_1' in mcsample: mcsampleframe['MC_Weight']    = (2)*mcsampleframe['MC_Weight']
		if 'cHHH1' in mcsample: mcsampleframe['MC_Weight']                     = (1)*mcsampleframe['MC_Weight']
		if 'cHHH0' in mcsample: mcsampleframe['MC_Weight']                     = (0.47347)*mcsampleframe['MC_Weight']
		if 'cHHH2p45' in mcsample: mcsampleframe['MC_Weight']                  = (0.64026)*mcsampleframe['MC_Weight']
		if 'cHHH5' in mcsample: mcsampleframe['MC_Weight']                     =-(0.11373)*mcsampleframe['MC_Weight']		
		#Slice the data sample to take only events with three/four b-tags among the two categories
		data_4b_prevbf_categ = SelectRegionforProductionModeTraining(mcsampleframe,'4b','preVBF',False)
		#Merge (concatenate) them
		print "[INFO] Saving mc signal sample for production mode"
		data.pandas2root(data_4b_prevbf_categ,'bbbbTree', 'mvatraining/%s/%s/SKIM_%s_PM.root'%(case,directory,mcsample)  )
		del data_4b_prevbf_categ          

def RunBkgModelSelection(bkgmodel,case,directory,tag):
	os.system('mkdir mvatraining')
	os.system('mkdir mvatraining/%s'%case)
	os.system('mkdir mvatraining/%s/%s'%(case,directory) )	
	#First prepare data to train for background rejection
	#Get data and create panda dataframes with specific variables, a.k.a. slicing the data. Then, create background model based on control region data
	print 'outputskims/%s/%s/SKIM_%s.root'%(case,directory,bkgmodel)
	sampleframe = data.root2pandas(['outputskims/%s/%s/SKIM_%s.root'%(case,directory,bkgmodel)],'bbbbTree')
	#Slice the data sample to take only events with three/four b-tags among the two categories
	data_cr_4b_categ,data_sr_4b_categ = SelectRegionforBackgroundRejectionTraining(sampleframe,'3b','GGF',False)

	print "[INFO] Saving mc signal sample for background rejection"
	data.pandas2root(data_sr_4b_categ,'bbbbTree', 'mvatraining/%s/%s/SKIM_%s_BR.root'%(case,directory,bkgmodel)  )
	del sampleframe 

#############COMMAND CODE IS BELOW ######################

###########OPTIONS
parser = argparse.ArgumentParser(description='Command line parser of skim options')
parser.add_argument('--config',   dest='cfgfile',   help='Name of config file',   required = True)
parser.add_argument('--sample',   dest='samplename',   help='Type of sample: 0 signal, 1 bkgmodel',   required = True)
parser.add_argument('--casename', dest='casename',  help='Case name',   required = True)
args = parser.parse_args()
configfilename = args.cfgfile
sampletorun    = args.samplename
case           = args.casename

###########Read Config file
print "[INFO] Reading skim configuration file . . ."
cfgparser = ConfigParser()
cfgparser.read('%s'%configfilename)
##########Get skim variables
print "[INFO] Getting configuration parameters . . ."
directory   = ast.literal_eval(cfgparser.get("configuration","directory"))
print "    -The directory:"
print "      *",directory
signalsamples  = ast.literal_eval(cfgparser.get("configuration","mvasigsamples"))
print "    -The list of signal samples:"
for x in range(len(signalsamples)):
  print "      *",signalsamples[x]
bdtbkgsamples  = ast.literal_eval(cfgparser.get("configuration","mvabkgsamples"))
print "    -The list of bdtbkg samples:"
print "      *",bdtbkgsamples[0] 
tag         = ast.literal_eval(cfgparser.get("configuration","tag"))
print "    -The tag:"
print "      *",tag  
lumi        = ast.literal_eval(cfgparser.get("configuration","lumi"))
print "    -The lumi:"
print "      *",lumi  

##########Make samples to train
if sampletorun=='0':   
   print "[INFO] Preparing MC signal model"
   #Create signal sample in signal region
   RunSignalSelection(signalsamples,lumi,case,directory,tag)
elif sampletorun=='1':
   print "[INFO] Preparing bdt-background model"
   #Create signal sample in signal region
   RunBkgModelSelection(bdtbkgsamples[0],case,directory,tag)
else:
   print "[ERROR] Type of sample is not specified"