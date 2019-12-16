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

def BuildReweightingModel(data_3b, data_4b,category,tag,bkgparams,bkgclassifierparams):
	print "[INFO] Processing predicted model for %s"%(tag)
	############################################################################
	##Let's slice data one more time to have the inputs for the bdt reweighting#700/600/300/200
	############################################################################
	modelargs = bkgparams
	if category =='GGF':
		 variablescr = ['nJet_ec','H1_m','H2_m','H1_pt','H2_pt','h1h2_deltaEta','H1_bb_deltaR','H2_bb_deltaR','HH_m','costh_HH_b1_ggfcm','HH_btag_b3_bscore','HH_btag_b3_bres']
	elif category == 'VBF':
		 variablescr = ['HH_m','H1_m','H2_m','H1_pt','H2_pt','h1h2_deltaEta','h1h2_deltaPhi','nJet_ec','JJ_m','j1j2_deltaEta']
	else:
		 variablescr = ['H1_pt', 'H2_pt','H1_eta', 'H2_eta']  	
	#######################################
	##Gradient Boosted Reweighter
	#######################################
	originalcr,targetcr,originalcr_weights,targetcr_weights,transferfactor = data.preparedataformodel(data_3b,data_4b,variablescr)
	ksresult_original = plotter.Draw1DHistosComparison(originalcr, targetcr, variablescr, originalcr_weights,True,"%s_original"%tag)
	#######################################
	##Folding Gradient Boosted Reweighter
	#######################################
	foldingcr_weights,reweightermodel,renormtransferfactor = data.fitreweightermodel(originalcr,targetcr,originalcr_weights,targetcr_weights,transferfactor,modelargs)  
	ksresult_model   = plotter.Draw1DHistosComparison(originalcr, targetcr, variablescr, foldingcr_weights,True,"%s_model"%tag)
	########################################
	## KS Test (as the developers do) / GB ROC AUC Test Study (Very slow, needs to train a classifier in cross-validation)
	########################################
	bdtreweighter.ks_measurement(variablescr,ksresult_original,ksresult_model)
	#Not currently used for optimization###bdtreweighter.discrimination_measurement(originalcr,targetcr,originalcr_weights,bkgclassifierparams,"%s"%tag,"original")
	#Not currently used for optimization###bdtreweighter.discrimination_measurement(originalcr,targetcr,foldingcr_weights,bkgclassifierparams,"%s"%tag,"model")
	########################################
	##Update 3b dataframe for modeling
	########################################
	return foldingcr_weights,reweightermodel,transferfactor,renormtransferfactor

def CreatePredictionModel(reweightermodel,transferfactor,renormtransferfactor,data_3b,category):
	############################################################################
	##Let's slice data one more time to have the inputs for the bdt reweighting#
	############################################################################
	if category =='GGF':
		 variables = ['nJet_ec','H1_m','H2_m','H1_pt','H2_pt','h1h2_deltaEta','H1_bb_deltaR','H2_bb_deltaR','HH_m','costh_HH_b1_ggfcm','HH_btag_b3_bscore','HH_btag_b3_bres']
	elif category == 'VBF':
		 variables = ['HH_m','H1_m','H2_m','H1_pt','H2_pt','h1h2_deltaEta','h1h2_deltaPhi','nJet_ec','JJ_m','j1j2_deltaEta']
	else:
		 variables = ['H1_pt','H2_pt','H1_eta','H2_eta']          
	original,original_weights = data.preparedataforprediction(data_3b,transferfactor,variables)
	########################################3############
	##Folding Gradient Boosted Reweighter (and DQM plots)
	#####################################################
	folding_weights= data.getmodelweights(original,original_weights,reweightermodel,transferfactor,renormtransferfactor)
	return folding_weights


def AddModelWeight(dataset,bkgparams,bkgclassifierparams,weightname,categ,valflag,tag):
	#Slice the data sample to take only events with three/four b-tags among the two categories
	data_cr_3b_categ,data_sr_3b_categ,data_rest_3b_categ = SelectRegions(dataset,'3b',categ,valflag)
	del dataset
	data_cr_4b_categ,data_sr_4b_categ,data_rest_4b_categ = SelectRegions(data_rest_3b_categ,'4b',categ,valflag)
	#Get weights ,model, transferfactor for CR data
	print "[INFO] Building BDT-reweighting model for %s in the CR data"%weightname
	weights_cr_categ,reweightermodel_categ,transferfactor_categ,renormtransferfactor_categ=BuildReweightingModel(data_cr_3b_categ,data_cr_4b_categ,categ,'%s_%s_%sCR'%(tag,weightname,categ),bkgparams,bkgclassifierparams)
	del data_cr_4b_categ,data_sr_4b_categ,data_rest_4b_categ
	#Get weights for the dataset
	print "[INFO] Calculating BDT-reweighting model prediction %s in the rest of the data"%weightname
	weights_sr_categ=CreatePredictionModel(reweightermodel_categ,transferfactor_categ,renormtransferfactor_categ,data_sr_3b_categ,categ)		
	print "[INFO] Calculating BDT-reweighting model prediction %s in the rest of the data"%weightname
	weights_rest_categ=CreatePredictionModel(reweightermodel_categ,transferfactor_categ,renormtransferfactor_categ,data_rest_3b_categ,categ)
	#Add the weights to the CR & the rest
	print "[INFO] Adding weight %s to the dataframes"%weightname
	data_cr_3b_categ[weightname]      = weights_cr_categ 
	data_sr_3b_categ[weightname]      = weights_sr_categ
	data_rest_3b_categ[weightname]    = weights_rest_categ
	print "[INFO] Adding training weight for MVA training"
	if weightname=='Weight_AnaGGF':
		l = int(len(data_sr_3b_categ.index) / 2)
		data_sr_3b_categ = data_sr_3b_categ.reset_index(drop=True)
		data_sr_3b_categ.loc[:l,"Weight_MVA"] = 0.
		data_sr_3b_categ.loc[l:,"Weight_MVA"] = 2.0  
	#Merge (concatenate) them
	print "[INFO] Returning dataframe"
	datasetwithweight = pandas.concat( (data_cr_3b_categ,data_sr_3b_categ,data_rest_3b_categ), ignore_index=True   )
	del data_cr_3b_categ,data_sr_3b_categ,data_rest_3b_categ
	value = numpy.ones(dtype='float64',shape=len(datasetwithweight))
	valuef = numpy.multiply(value,transferfactor_categ)
	datasetwithweight[weightname+"_tfactor"] = valuef
	return datasetwithweight

def RunReweightingModel(dataset,ggfbkgparams,vbfbkgparams,ggfanaclassifierparams,vbfanaclassifierparams,ggfvalclassifierparams,vbfvalclassifierparams,case,directory,tag):
	datasetwithweights1=AddModelWeight(dataset,ggfbkgparams,ggfvalclassifierparams,'Weight_ValGGF','GGF',True,'%s%s'%(tag,case))
	del dataset
	datasetwithweights2=AddModelWeight(datasetwithweights1,vbfbkgparams,vbfvalclassifierparams,'Weight_ValVBF','VBF',True,'%s%s'%(tag,case))
	del datasetwithweights1
	datasetwithweights3=AddModelWeight(datasetwithweights2,ggfbkgparams,ggfanaclassifierparams,'Weight_AnaGGF','GGF',False,'%s%s'%(tag,case))
	del datasetwithweights2
	datasetwithweights =AddModelWeight(datasetwithweights3,vbfbkgparams,vbfanaclassifierparams,'Weight_AnaVBF','VBF',False,'%s%s'%(tag,case))
	del datasetwithweights3
	print 'Here comes the dataset with reweighting-model weights . . .'
	print datasetwithweights.head()
	#Save modeling dataframe tree to a root file
	seed = ggfbkgparams[5]
	if seed!=2019:
	   data.pandas2root(datasetwithweights,'bbbbTree',   'outputskims/%s/%s/SKIM_BKG_MODEL_tree_%s.root'%(case,directory,seed)  )
	   data.roothist2root('outputskims/%s/%s/SKIM_Data.root'%(case,directory),'eff_histo','outputskims/%s/%s/SKIM_BKG_MODEL_hist_%s.root'%(case,directory,seed)  )
	   os.system("hadd -f outputskims/%s/%s/SKIM_BKG_MODEL_%s.root outputskims/%s/%s/SKIM_BKG_MODEL_tree_%s.root outputskims/%s/%s/SKIM_BKG_MODEL_hist_%s.root"%(case,directory,seed,case,directory,seed,case,directory,seed))
	   os.system("rm outputskims/%s/%s/SKIM_BKG_MODEL_tree_%s.root outputskims/%s/%s/SKIM_BKG_MODEL_hist_%s.root"%(case,directory,seed,case,directory,seed) )
	else:
	   data.pandas2root(datasetwithweights,'bbbbTree',   'outputskims/%s/%s/SKIM_BKG_MODEL_tree.root'%(case,directory)  )
	   data.roothist2root('outputskims/%s/%s/SKIM_Data.root'%(case,directory),'eff_histo','outputskims/%s/%s/SKIM_BKG_MODEL_hist.root'%(case,directory)  )
	   os.system("hadd -f outputskims/%s/%s/SKIM_BKG_MODEL.root outputskims/%s/%s/SKIM_BKG_MODEL_tree.root outputskims/%s/%s/SKIM_BKG_MODEL_hist.root"%(case,directory,case,directory,case,directory))
	   os.system("rm outputskims/%s/%s/SKIM_BKG_MODEL_tree.root outputskims/%s/%s/SKIM_BKG_MODEL_hist.root"%(case,directory,case,directory) )			

#############COMMAND CODE IS BELOW ######################

###########OPTIONS
parser = argparse.ArgumentParser(description='Command line parser of skim options')
parser.add_argument('--config', dest='cfgfile',  help='Name of config file',   required = True)
parser.add_argument('--casename', dest='casename',  help='Case name',   required = True)
args = parser.parse_args()
configfilename = args.cfgfile
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
ggfbkgparams = ast.literal_eval(cfgparser.get("configuration","ggfbkgparams"))
print "    -The ggf-hh background parameters (trees, learning rate, max depth, min sample leaf, subsample, randomseed):"
print "      *",ggfbkgparams
vbfbkgparams = ast.literal_eval(cfgparser.get("configuration","vbfbkgparams"))
print "    -The vbf-hh background parameters (trees, learning rate, max depth, min sample leaf, subsample, randomseed):"
print "      *",vbfbkgparams
ggfanaclassifierparams = ast.literal_eval(cfgparser.get("configuration","ggfanaclassifierparams"))
print "    -The ggf-hh ana classifier parameters (trees, learning rate, subsample, randomseed):"
print "      *",ggfanaclassifierparams
vbfanaclassifierparams = ast.literal_eval(cfgparser.get("configuration","vbfanaclassifierparams"))
print "    -The vbf-hh ana classifier parameters (trees, learning rate, subsample, randomseed):"
print "      *",vbfanaclassifierparams
ggfvalclassifierparams  = ast.literal_eval(cfgparser.get("configuration","ggfvalclassifierparams"))
print "    -The ggf-hh val classifier parameters (trees, learning rate, subsample, randomseed):"
print "      *",ggfvalclassifierparams
vbfvalclassifierparams = ast.literal_eval(cfgparser.get("configuration","vbfvalclassifierparams"))
print "    -The vbf-hh val classifier parameters (trees, learning rate, subsample, randomseed):"
print "      *",vbfvalclassifierparams
tag         = ast.literal_eval(cfgparser.get("configuration","tag"))
print "    -The tag name:"
print "      *",tag 
print "    -The case of event selection a.k.a. foldername:"
print "      *",case 
##########Make microskims
print "[INFO] Making background model . . . "
#Get data and create panda dataframes with specific variables, a.k.a. slicing the data. Then, create background model based on control region data
dataset      = data.root2pandas('outputskims/%s/%s/SKIM_Data.root'%(case,directory),'bbbbTree')
RunReweightingModel(dataset,ggfbkgparams,vbfbkgparams,ggfanaclassifierparams,vbfanaclassifierparams,ggfvalclassifierparams,vbfvalclassifierparams,case,directory,tag)