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
import pickle
#My modules
import modules.datatools as data
import modules.plotter as plotter
import modules.bdtreweighter as bdtreweighter
import modules.selections as selector

def ReadReweightingModel(bkdtrainingfile):
	# open picked file and use it back again
	infile     = open(bkdtrainingfile,'rb')
	training   = pickle.load(infile)
	infile.close()
	#Get training information
	variables       = training.get('vars')
	bdtreweighter   = training.get('model')
	transferfactor  = training.get('tfactor')
	return variables,bdtreweighter,transferfactor

def ReadTransferFactorModel(bkdtrainingfile):
	# open picked file and use it back again
	infile     = open(bkdtrainingfile,'rb')
	training   = pickle.load(infile)
	infile.close()
	#Get training information
	variables       = training.get('vars')
	transferfactor  = training.get('tfactor')
	return variables,transferfactor

def SelectRegions(sample,btagregion,category,validation):
	datacr,samplerest = selector.eventselection(sample,btagregion,category,'CR',validation)
	del sample
	datasr,datarest   = selector.eventselection(samplerest,btagregion,category,'SR',validation)	
	del samplerest
	return datacr,datasr,datarest

def ApplyReweightingModel(reweightermodel,transferfactor,variables,mcdata):
	############################################################################
	##Let's slice data one more time to have the inputs for the bdt reweighting
	############################################################################    
	original,original_weights = data.preparemcforprediction(mcdata,variables)
	########################################3############
	##Folding Gradient Boosted Reweighter (and DQM plots)
	#####################################################
	folding_weights = data.getmodelweightsformc(original,original_weights,reweightermodel,transferfactor)

	return folding_weights

def ApplyTransferFactorModel(transferfactor,variables,mcdata):
	############################################################################
	##Let's slice data one more time to have the inputs for the bdt reweighting
	############################################################################    
	original,original_weights = data.preparemcforprediction(mcdata,variables)
	########################################3############
	##Transfer facotr weights
	#####################################################
	folding_weights = data.getmodeltransferfactorweightsformc(original,original_weights,transferfactor)

	return folding_weights

def AddModelWeights(dataset,weightname,categ,valflag,tag):
	#Slice the data sample to take only events with three/four b-tags among the two categories
	print "\n"*2
	print "[INFO] Preparing data and building BDT-reweighting model for %s using the 3b/4b CR data"%weightname
	data_cr_3b_categ,data_sr_3b_categ,data_rt_3b_categ = SelectRegions(dataset,'3b',categ,valflag)
	del dataset
	#Get weights ,model, transferfactor 
	if 'VBF2' in weightname:
		variables,transferfactor = ReadTransferFactorModel('bkgtraining/mymodels/%s_%s_bkgmodel.pkl'%(tag,weightname))
		#Get weights for the MC dataset
		print "[INFO] Calculating BDT-reweighting model prediction %s in the 3b SR MC"%weightname
		weights_sr_categ = ApplyTransferFactorModel(transferfactor,variables,data_sr_3b_categ)		
		print "[INFO] Calculating BDT-reweighting model prediction %s in the 3b CR MC"%weightname
		weights_cr_categ = ApplyTransferFactorModel(transferfactor,variables,data_cr_3b_categ)		
		print "[INFO] Calculating BDT-reweighting model prediction %s in the rest of the MC"%weightname
		weights_rt_categ = ApplyTransferFactorModel(transferfactor,variables,data_rt_3b_categ)
	else: 
		variables,bdtreweighter,transferfactor = ReadReweightingModel('bkgtraining/mymodels/%s_%s_bkgmodel.pkl'%(tag,weightname))
		#Get weights for the MC dataset
		print "[INFO] Calculating BDT-reweighting model prediction %s in the 3b SR MC"%weightname
		weights_sr_categ = ApplyReweightingModel(bdtreweighter,transferfactor,variables,data_sr_3b_categ)		
		print "[INFO] Calculating BDT-reweighting model prediction %s in the 3b CR MC"%weightname
		weights_cr_categ = ApplyReweightingModel(bdtreweighter,transferfactor,variables,data_cr_3b_categ)		
		print "[INFO] Calculating BDT-reweighting model prediction %s in the rest of the MC"%weightname
		weights_rt_categ = ApplyReweightingModel(bdtreweighter,transferfactor,variables,data_rt_3b_categ)
	#Add the weights to the CR & the rest
	print "[INFO] Adding weight %s"%weightname
	data_cr_3b_categ[weightname] = weights_cr_categ 
	data_sr_3b_categ[weightname] = weights_sr_categ
	data_rt_3b_categ[weightname] = weights_rt_categ
	#Merge (concatenate) them
	print "[INFO] Concatenating all events in the dataframe"
	datasetwithweight = pandas.concat( (data_cr_3b_categ,data_sr_3b_categ,data_rt_3b_categ), ignore_index=True   )
	del data_cr_3b_categ,data_sr_3b_categ,data_rt_3b_categ
	value  = numpy.ones(dtype='float64',shape=len(datasetwithweight))
	datasetwithweight[weightname+"_tfactor"] = numpy.multiply(value,transferfactor)
	datasetwithweight                        = datasetwithweight.reset_index(drop=True)
	return datasetwithweight

def RunReweightingModel(dataset,case,directory,tag,valflag,catflag,subcatflag):
	#Add weights for analysis
	if valflag==True and catflag==False:
		if subcatflag==1: 
			dataset=AddModelWeights(dataset,'Weight_ValGGF1', 'GGF1',valflag,'%s'%(tag))
		elif subcatflag==2:
			dataset=AddModelWeights(dataset,'Weight_ValGGF2','GGF2',valflag,'%s'%(tag))
		else:
			dataset=AddModelWeights(dataset,'Weight_ValGGF','GGF',valflag,'%s'%(tag))

	elif valflag==True and catflag==True:
		if subcatflag==1:
			dataset=AddModelWeights(dataset,'Weight_ValVBF1','VBF1',valflag,'%s'%(tag))
		elif subcatflag==2:
			dataset=AddModelWeights(dataset,'Weight_ValVBF2','VBF2',valflag,'%s'%(tag))
		else:
			dataset=AddModelWeights(dataset,'Weight_ValVBF','VBF',valflag,'%s'%(tag))

	elif valflag==False and catflag==False:
		if subcatflag==1: 
			dataset=AddModelWeights(dataset,'Weight_AnaGGF1','GGF1',valflag,'%s'%(tag))
		elif subcatflag==2:
			dataset=AddModelWeights(dataset,'Weight_AnaGGF2','GGF2',valflag,'%s'%(tag))
		else:
			dataset=AddModelWeights(dataset,'Weight_AnaGGF','GGF',valflag,'%s'%(tag))		
	elif valflag==False and catflag==True:
		if subcatflag==1: 
			dataset=AddModelWeights(dataset,'Weight_AnaVBF1','VBF1',valflag,'%s'%(tag))
		elif subcatflag==2:
			dataset=AddModelWeights(dataset,'Weight_AnaVBF2','VBF2',valflag,'%s'%(tag))
		else:
			dataset=AddModelWeights(dataset,'Weight_AnaVBF','VBF',valflag,'%s'%(tag))			
	else:
		print "[ERROR] No weights are calculated, incorrect selection!"   	
	return dataset

#############COMMAND CODE IS BELOW ######################

###########OPTIONS
parser = argparse.ArgumentParser(description='Command line parser of skim options')
parser.add_argument('--config',  dest='cfgfile',  help='Name of config file',   required = True)
parser.add_argument('--casename',dest='casename', help='Case name',   required = True)
args = parser.parse_args()
configfilename = args.cfgfile
case           = args.casename
#weight         = args.weight
###########Read Config file
print "[INFO] Reading skim configuration file . . ."
cfgparser = ConfigParser()
cfgparser.read('%s'%configfilename)
##########Get skim variables
print "[INFO] Getting configuration parameters . . ."
directory   = ast.literal_eval(cfgparser.get("configuration","directory"))
print "    -The directory:"
print "      *",directory
tag         = ast.literal_eval(cfgparser.get("configuration","tag"))
print "    -The tag name:"
print "      *",tag 
sigsamples  = ast.literal_eval(cfgparser.get("configuration","sigsamplesforselfbias"))
print "    -The list of MC signal samples:"
for x in range(len(sigsamples)):
  print "      *",sigsamples[x]
siginjs     = ast.literal_eval(cfgparser.get("configuration","siginjs"))
print "    -The injected signal strenghts (u_inj):"
for x in range(len(siginjs)):
  print "      *",siginjs[x]
lumi         = ast.literal_eval(cfgparser.get("configuration","lumi"))
print "    -The lumi:"
print "      *",lumi   
##########Appy background miodel to MC sample
print "[INFO] Apply background model in MC sample  ... "
#Create folder to save training information
for sample in sigsamples:
	 #Get data and create panda dataframes with specific variables, a.k.a. slicing the data. Then, create background model based on control region data
	 datalist = []
	 datalist.append('outputskims/%s/%s/SKIM_%s.root'%(case,directory,sample))
	 dataset = data.root2pandas(datalist,'bbbbTree')
	 #Add MC event weight to have right normalization afterwards
	 dataset = data.AddMCWeight(dataset,lumi,'outputskims/%s/%s/SKIM_%s.root'%(case,directory,sample))

	 #Add GGF1 weights for analysis/validation region
	 dataset = RunReweightingModel(dataset,case,directory,tag,False,False,1)
	 dataset = RunReweightingModel(dataset,case,directory,tag,True,False,1)
	 #Add GGF2 weights for analysis/validation region
	 dataset = RunReweightingModel(dataset,case,directory,tag,False,False,2)
	 dataset = RunReweightingModel(dataset,case,directory,tag,True,False,2)
	 #Add VBF1 weights for analysis/validation region
	 dataset = RunReweightingModel(dataset,case,directory,tag,False,True,1)
	 dataset = RunReweightingModel(dataset,case,directory,tag,True,True,1)
	 #Add VBF2 weights for analysis/validation region
	 dataset = RunReweightingModel(dataset,case,directory,tag,False,True,2)
	 dataset = RunReweightingModel(dataset,case,directory,tag,True,True,2)

	 #Before saving it, reset the MC weights=1, the bkg-model weight already includes the correct normalization. Then, pass it as data-driven estimate
	 weights = ['XS','bTagScaleFactor_central','PUWeight','genWeight']
	 value   = numpy.ones(dtype='float64',shape=len(dataset))
	 for w in weights: dataset[w]=value
	 #Save everything in a root file with several u_inj scenarios
	 for u_inj in siginjs:
		 print "[INFO] Saving MC background model (u_inj=%.1f) in a root file"%u_inj
		 dataset_uinj       = dataset.copy()
		 dataset_uinj['XS'] = dataset_uinj['XS'].apply(lambda x: x*u_inj)
		 data.pandas2root(dataset_uinj,'bbbbTree',    'outputskims/%s/%s/SKIM_%s_MODEL_%.1f_tree.root'%(case,directory,sample,u_inj)  )
		 data.roothist2root(datalist,'eff_histo','outputskims/%s/%s/SKIM_%s_MODEL_%.1f_hist.root'%(case,directory,sample,u_inj)  )
		 os.system("hadd -f outputskims/%s/%s/SKIM_%s_MODEL_%.1f_uinj.root outputskims/%s/%s/SKIM_%s_MODEL_%.1f_tree.root outputskims/%s/%s/SKIM_%s_MODEL_%.1f_hist.root"%(case,directory,sample,u_inj,case,directory,sample,u_inj,case,directory,sample,u_inj))
		 os.system("rm outputskims/%s/%s/SKIM_%s_MODEL_%.1f_tree.root outputskims/%s/%s/SKIM_%s_MODEL_%.1f_hist.root"%(case,directory,sample,u_inj,case,directory,sample,u_inj) )	
		 del dataset_uinj