import numpy 
import os
import root_numpy
import pandas
import glob
import argparse
import sys
import ast
from  ConfigParser import *
from hep_ml import reweight
from root_numpy import root2array
from numpy.lib.recfunctions import stack_arrays
import pickle
import multiprocessing as mp
from functools import partial
import time
#My modules
import modules.datatools as data
import modules.plotter as plotter
import modules.bdtreweighter as bdtreweighter
import modules.selections as selector


def SelectRegions(sample,btagregion,category,validation):
	datacr,samplerest = selector.eventselection(sample,btagregion,category,'CR',validation)
	del sample
	datasr,datarest   = selector.eventselection(samplerest,btagregion,category,'SR',validation)	
	del samplerest
	return datacr,datasr,datarest


def BuildReweightingModel(data_3b, data_4b,category,tag,bkgclassifierparams,bkgparams):
	print "[INFO] Processing predicted model for %s"%(tag)
	modelargs=bkgparams
	############################################################################
	##Let's slice data one more time to have the inputs for the bdt reweighting
	############################################################################
	if category =='GGF' or category =='GGF1' or category =='GGF2':
		 variablescr = ['H1_b1_ptRegressed','H1_b2_ptRegressed','H2_b1_ptRegressed','H2_b2_ptRegressed','H1_m','H2_m','HH_m','H1_pt','H2_pt','h1h2_deltaEta','H1_bb_deltaR','H2_bb_deltaR','abs_costh_H1_ggfcm','HH_btag_b3_bres','abs_costh_H1_b1_h1cm','sum_3b_bscore','sum_4b_pt','HH_pt']	
	elif category == 'VBF' or category =='VBF1' or category =='VBF2':
		 variablescr = ['H1_b1_ptRegressed','H1_b2_ptRegressed','H2_b1_ptRegressed','H2_b2_ptRegressed','H1_m','H2_m','HH_m','H1_pt','H2_pt','h1h2_deltaEta','h1h2_deltaPhi','JJ_m','j1j2_deltaEta','GGFKiller']
	else:
		 variablescr = ['HH_m']   		
	#######################################
	##Prepare data to create the model
	#######################################
	originalcr,targetcr,originalcr_weights,targetcr_weights,transferfactor = data.preparedataformodel(data_3b,data_4b,variablescr)
	#plotter.Draw1DHistosComparison(originalcr, targetcr, variablescr, originalcr_weights,True,"%s_original"%tag)

	#######################################
	##Grid search or main fit
	#######################################
	print "--------------------------Best-Hyperparameters-------------------------------------"
	print "-----------------------------------------------------------------------------------"
	print "Parameters: ",modelargs
	print "-----------------------------------------------------------------------------------"
	#######################################
	##Folding Gradient Boosted Reweighter (3-fold BDT reweighter)
	#######################################
	foldingcr_weights,reweightermodel = data.fitreweightermodel(originalcr,targetcr,originalcr_weights,targetcr_weights,transferfactor,modelargs)  
	#plotter.Draw1DHistosComparison(originalcr, targetcr, variablescr, foldingcr_weights,True,"%s_model"%tag)
	
	########################################
	## KS Test (as the developers of the method do), currently used for optimization/check of the parameters
	########################################
	ksresult_original = bdtreweighter.ks_test(originalcr, targetcr, variablescr, originalcr_weights)
	ksresult_model    = bdtreweighter.ks_test(originalcr, targetcr, variablescr, foldingcr_weights)	
	passedksresult    = bdtreweighter.ks_comparison(variablescr,ksresult_original,ksresult_model)
	###########################################################################################
	## GB ROC AUC Test Study (Very slow test, needs to train a classifier in cross-validation)
	############################################################################################
	roc_auc_nowts  = bdtreweighter.discrimination_test(originalcr,targetcr,originalcr_weights,bkgclassifierparams,"%s"%tag,"original",False)
	roc_auc_wts    = bdtreweighter.discrimination_test(originalcr,targetcr,foldingcr_weights,bkgclassifierparams,"%s"%tag,"model",False)
	passedresult   = bdtreweighter.discrimination_comparison(roc_auc_nowts,roc_auc_wts)

	return foldingcr_weights,reweightermodel,transferfactor

def BuildTransferFactorModel(data_3b, data_4b,category,tag):
	print "[INFO] Processing predicted model for %s"%(tag)
	#######################################
	##Prepare data to create the model
	#######################################
	variablescr = ['HH_m']
	originalcr,targetcr,originalcr_weights,targetcr_weights,transferfactor = data.preparedataformodel(data_3b,data_4b,variablescr)
	data.fittransferfactormodel(originalcr,targetcr,originalcr_weights,targetcr_weights,transferfactor)
	#plotter.Draw1DHistosComparison(originalcr, targetcr, variablescr, originalcr_weights,True,"%s_model"%tag)
	########################################
	## Save training information
	########################################
	SaveTransferFactorModel(variablescr,transferfactor,tag)

	return originalcr_weights,transferfactor


def ApplyReweightingModel(reweightermodel,transferfactor,data_3b,data_4b,tag,bkgclassifierparams,category,valflag,srflag):
	############################################################################
	##Let's slice data one more time to have the inputs for the bdt reweighting
	############################################################################
	if category =='GGF' or category =='GGF1' or category =='GGF2':
		 variables = ['H1_b1_ptRegressed','H1_b2_ptRegressed','H2_b1_ptRegressed','H2_b2_ptRegressed','H1_m','H2_m','HH_m','H1_pt','H2_pt','h1h2_deltaEta','H1_bb_deltaR','H2_bb_deltaR','abs_costh_H1_ggfcm','HH_btag_b3_bres','abs_costh_H1_b1_h1cm','sum_3b_bscore','sum_4b_pt','HH_pt']
	elif category == 'VBF' or category =='VBF1' or category =='VBF2':
		 variables = ['H1_b1_ptRegressed','H1_b2_ptRegressed','H2_b1_ptRegressed','H2_b2_ptRegressed','H1_m','H2_m','HH_m','H1_pt','H2_pt','h1h2_deltaEta','h1h2_deltaPhi','JJ_m','j1j2_deltaEta','GGFKiller']
	else:
		 variables = ['HH_m']         
	original,original_weights = data.preparedataforprediction(data_3b,transferfactor,variables)
	target,target_weights     = data.preparedataforprediction(data_4b,1,variables)

	########################################3############
	##Folding Gradient Boosted Reweighter (and DQM plots)
	#####################################################
	folding_weights= data.getmodelweights(original,original_weights,target,target_weights,reweightermodel,transferfactor,valflag,srflag)
	########################################
	## KS Test (as the developers of the method do), done just for the validation signal region as we are currently blinded
	########################################
	#if valflag==True and srflag==True:
		############################################################################
		##Individual and global tests of the validation signal region
		############################################################################
		#plotter.Draw1DHistosComparison(original, target, variables, original_weights,True,"%s_val%s_%ssr_originalprediction"%(tag,category,category) )
		#plotter.Draw1DHistosComparison(original, target, variables, folding_weights,True,"%s_val%s_%ssr_modelprediction"%(tag,category,category) )
		#ksresult_original = bdtreweighter.ks_test(original, target, variables, original_weights)
		#ksresult_model    = bdtreweighter.ks_test(original, target, variables, folding_weights)	
		#bdtreweighter.ks_comparison(variables,ksresult_original,ksresult_model)
		#bdtreweighter.discrimination_test(original,target,original_weights,bkgclassifierparams,"%s_val%s_%ssr"%(tag,category,category),"originalprediction")
		#bdtreweighter.discrimination_test(original,target,folding_weights,bkgclassifierparams,"%s_val%s_%ssr"%(tag,category,category),"modelprediction")
		############################################################################
		##Let's slice data one more time to have the observables tested
		############################################################################
		#if category == 'GGF1':
		#	observable = ['GGFMVA1'] 
		#	hfmt       = [50,0,1]
		#elif category == 'GGF2':
		#	observable = ['GGFMVA2'] 
		#	hfmt       = [50,0,1]
		#else:
		#	observable = ['HH_m'] 
		#	hfmt       = [80,200,1000]
		#ori_obs,ori_weights = data.preparedataforprediction(data_3b,transferfactor,observable)
		#tar_obs,tar_weights = data.preparedataforprediction(data_4b,1,observable)	    
		#plotter.Draw1DHisto(ori_obs, tar_obs, observable[0], folding_weights, hfmt, False,"%s_%s_model"%(tag,category) )
		#del ori_obs,tar_obs,ori_weights,tar_weights		
	return folding_weights


def ApplyTransferFactorModel(transferfactor,data_3b,data_4b,tag,category,valflag,srflag):
	############################################################################
	##Let's slice data one more time to have the inputs for the bdt reweighting
	############################################################################
	variables = ['HH_m']          
	original,original_weights = data.preparedataforprediction(data_3b,transferfactor,variables)
	target,target_weights     = data.preparedataforprediction(data_4b,1,variables)
	########################################
	## Check the single bin, done just for the validation signal region as we are currently blinded
	########################################
	data.getmodeltransferfactorweights(original,original_weights,target,target_weights,transferfactor,valflag,srflag)	
	return original_weights


def AddModelWeights(dataset,bkgparams,bkgclassifierparams,weightname,categ,valflag,tag):
	print "\n"*2
	#Slice the data sample to take only events with three/four b-tags among the two categories
	print "[INFO] Preparing data and building BDT-reweighting model for %s using the 3b/4b CR data"%weightname
	data_cr_3b_categ,data_sr_3b_categ,data_rest_3b_categ        = SelectRegions(dataset,'3b',categ,valflag)
	del dataset
	data_cr_4b_categ,data_sr_4b_categ,data_rest_4b_categ        = SelectRegions(data_rest_3b_categ,'4b',categ,valflag)
	print "[INFO] Building BDT-reweighting or Transfer Factor model for %s using the 3b/4b CR data"%weightname

	#Get weights ,model, transferfactor for 3b CR data
	if 'VBF2' in weightname:
		weights_cr_categ,transferfactor_categ                       = BuildTransferFactorModel(data_cr_3b_categ,data_cr_4b_categ,categ,'%s_%s'%(tag,weightname))
	else:
		weights_cr_categ,reweightermodel_categ,transferfactor_categ = BuildReweightingModel(data_cr_3b_categ,data_cr_4b_categ,categ,'%s_%s'%(tag,weightname),bkgclassifierparams,bkgparams)
	del data_cr_4b_categ 		

	#Get weights for 3b SR data
	print "[INFO] Calculating BDT-reweighting model prediction %s in the 3b SR data"%weightname
	if 'VBF2' in weightname:
		weights_sr_categ   = ApplyTransferFactorModel(transferfactor_categ,data_sr_3b_categ,data_sr_4b_categ,tag,categ,valflag,True)		
	else:
		weights_sr_categ   = ApplyReweightingModel(reweightermodel_categ,transferfactor_categ,data_sr_3b_categ,data_sr_4b_categ,tag,bkgclassifierparams,categ,valflag,True)		

	#Get weights for the rest of the data
	print "[INFO] Calculating BDT-reweighting model prediction %s in the rest of the data"%weightname
	if 'VBF2' in weightname:
		weights_rest_categ = ApplyTransferFactorModel(transferfactor_categ,data_rest_3b_categ,data_rest_4b_categ,tag,categ,valflag,False)
	else:
		weights_rest_categ = ApplyReweightingModel(reweightermodel_categ,transferfactor_categ,data_rest_3b_categ,data_rest_4b_categ,tag,bkgclassifierparams,categ,valflag,False)
	del data_sr_4b_categ,data_rest_4b_categ

	#Add the weights to the dataframes
	print "[INFO] Adding weight %s"%weightname
	data_cr_3b_categ[weightname]      = weights_cr_categ 
	data_sr_3b_categ[weightname]      = weights_sr_categ
	data_rest_3b_categ[weightname]    = weights_rest_categ
	print "[INFO] Adding training id for MVA training"
	if weightname=='Weight_AnaGGF' or weightname=='Weight_AnaGGF1' or weightname=='Weight_AnaGGF2':
		l = int(len(data_sr_3b_categ.index) / 2)
		data_sr_3b_categ = data_sr_3b_categ.reset_index(drop=True)
		data_sr_3b_categ.loc[:l,"%s_MVA"%weightname] = 0.
		data_sr_3b_categ.loc[l:,"%s_MVA"%weightname] = 2.0  

	#Merge (concatenate) them and return the dataframe
	print "[INFO] Concatenating all events in the dataframe"
	datasetwithweight = pandas.concat( (data_cr_3b_categ,data_sr_3b_categ,data_rest_3b_categ), ignore_index=True   )
	del data_cr_3b_categ,data_sr_3b_categ,data_rest_3b_categ
	value  = numpy.ones(dtype='float64',shape=len(datasetwithweight))
	datasetwithweight[weightname+"_tfactor"] = numpy.multiply(value,transferfactor_categ)
	datasetwithweight                        = datasetwithweight.reset_index(drop=True)
	return datasetwithweight

def RunReweightingModel(dataset,bkgparams,classifierparams,case,directory,tag,valflag,vbfflag,subcatflag):
	#Add weights for analysis
	if valflag==True  and vbfflag==False: #Validation GGF
		if subcatflag==1:
			dataset=AddModelWeights(dataset,bkgparams,classifierparams,'Weight_ValGGF1','GGF1',valflag,'%s'%(tag))
		elif subcatflag==2:
			dataset=AddModelWeights(dataset,bkgparams,classifierparams,'Weight_ValGGF2','GGF2',valflag,'%s'%(tag))
		else:
			dataset=AddModelWeights(dataset,bkgparams,classifierparams,'Weight_ValGGF','GGF',valflag,'%s'%(tag))
	elif valflag==True and vbfflag==True: #Validation VBF
		if subcatflag==1:
			dataset=AddModelWeights(dataset,bkgparams,classifierparams,'Weight_ValVBF1','VBF1',valflag,'%s'%(tag))
		elif subcatflag==2:
			dataset=AddModelWeights(dataset,bkgparams,classifierparams,'Weight_ValVBF2','VBF2',valflag,'%s'%(tag))
		else:
			dataset=AddModelWeights(dataset,bkgparams,classifierparams,'Weight_ValVBF','VBF',valflag,'%s'%(tag))
	elif valflag==False and vbfflag==False: #Analysis GGF
		if subcatflag==1: 
			dataset=AddModelWeights(dataset,bkgparams,classifierparams,'Weight_AnaGGF1','GGF1',valflag,'%s'%(tag))
		elif subcatflag==2:
			dataset=AddModelWeights(dataset,bkgparams,classifierparams,'Weight_AnaGGF2','GGF2',valflag,'%s'%(tag))
		else:
			dataset=AddModelWeights(dataset,bkgparams,classifierparams,'Weight_AnaGGF','GGF',valflag,'%s'%(tag))		
	elif valflag==False and vbfflag==True: #Analysis VBF
		if subcatflag==1: 
			dataset=AddModelWeights(dataset,bkgparams,classifierparams,'Weight_AnaVBF1','VBF1',valflag,'%s'%(tag))
		elif subcatflag==2:
			dataset=AddModelWeights(dataset,bkgparams,classifierparams,'Weight_AnaVBF2','VBF2',valflag,'%s'%(tag))
		else:
			dataset=AddModelWeights(dataset,bkgparams,classifierparams,'Weight_AnaVBF','VBF',valflag,'%s'%(tag))
	else:
		"[ERROR] No weights are calculated, incorrect selection!"   	
	return dataset

#############COMMAND CODE IS BELOW ######################

###########OPTIONS
parser = argparse.ArgumentParser(description='Command line parser of skim options')
parser.add_argument('--config',  dest='cfgfile',  help='Name of config file',   required = True)
parser.add_argument('--casename',dest='casename', help='Case name',   required = True)
#parser.add_argument('--weight',  dest='weight',   help='Weights: (1) Ana-VBF, (2) Val-VBF, (3) Ana-GGF1, (4) Val-GGF1, (5) Ana-GGF2, (6) Val-GGF2',   required = True)
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
ggf1anabkgparams = ast.literal_eval(cfgparser.get("configuration","ggf1anabkgparams"))
print "    -The ggf-hh1 ana background parameters (trees, learning rate, max depth, min sample leaf, subsample, randomseed):"
print "      *",ggf1anabkgparams
ggf2anabkgparams = ast.literal_eval(cfgparser.get("configuration","ggf2anabkgparams"))
print "    -The ggf-hh2 ana background parameters (trees, learning rate, max depth, min sample leaf, subsample, randomseed):"
print "      *",ggf2anabkgparams
vbfanabkgparams = ast.literal_eval(cfgparser.get("configuration","vbfanabkgparams"))
print "    -The vbf-hh ana background parameters (trees, learning rate, max depth, min sample leaf, subsample, randomseed):"
print "      *",vbfanabkgparams

ggf1valbkgparams = ast.literal_eval(cfgparser.get("configuration","ggf1valbkgparams"))
print "    -The ggf-hh1 ana background parameters (trees, learning rate, max depth, min sample leaf, subsample, randomseed):"
print "      *",ggf1valbkgparams
ggf2valbkgparams = ast.literal_eval(cfgparser.get("configuration","ggf2valbkgparams"))
print "    -The ggf-hh2 ana background parameters (trees, learning rate, max depth, min sample leaf, subsample, randomseed):"
print "      *",ggf2valbkgparams
vbfvalbkgparams = ast.literal_eval(cfgparser.get("configuration","vbfvalbkgparams"))
print "    -The vbf-hh ana background parameters (trees, learning rate, max depth, min sample leaf, subsample, randomseed):"
print "      *",vbfvalbkgparams

##########Make background miodel
print "[INFO] Making background model . . . "
#Create folder to save training information
os.system("mkdir bkgtraining")
os.system("mkdir bkgtraining/mymodels")

#Get data and create panda dataframes with specific variables, a.k.a. slicing the data. Then, create background model based on control region data
datalist = []
datalist.append('outputskims/%s/%s/SKIM_Data.root'%(case,directory))
dataset  = data.root2pandas(datalist,'bbbbTree')

#classifierparameters
classifierparams=[40,0.3,2020]

starttime = time.time()
dataset = RunReweightingModel(dataset,vbfanabkgparams,classifierparams,case,directory,tag,False,True,0)

##Save everything in a root file
print "[INFO] Saving background model weight in file . . . "
data.pandas2root(dataset,'bbbbTree',   'outputskims/%s/%s/SKIM_GGFKILLER_BKG_MODEL_tree.root'%(case,directory)  )
data.roothist2root(datalist,'eff_histo','outputskims/%s/%s/SKIM_GGFKILLER_BKG_MODEL_hist.root'%(case,directory)  )
os.system("hadd -f outputskims/%s/%s/SKIM_GGFKILLER_BKG_MODEL.root outputskims/%s/%s/SKIM_GGFKILLER_BKG_MODEL_tree.root outputskims/%s/%s/SKIM_GGFKILLER_BKG_MODEL_hist.root"%(case,directory,case,directory,case,directory))
os.system("rm outputskims/%s/%s/SKIM_GGFKILLER_BKG_MODEL_tree.root outputskims/%s/%s/SKIM_GGFKILLER_BKG_MODEL_hist.root"%(case,directory,case,directory) )			

print('Running the background model took {} seconds'.format(time.time() - starttime))