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

	#######################################
	##Grid search or main fit
	#######################################

	print "----------------------------GRID-SEARCH---------------------------------------"
	k=1
	mymodels=[]
	for modelargs in bkgparams:	
		print "------------------------------------------------------------------------------"
		print "Parameters: ",modelargs
		print "------------------------------------------------------------------------------"
		#######################################
		##Folding Gradient Boosted Reweighter (3-fold BDT reweighter)
		#######################################
		foldingcr_weights,reweightermodel = data.fitreweightermodel(originalcr,targetcr,originalcr_weights,targetcr_weights,transferfactor,modelargs)  

		########################################
		## KS Test (as the developers of the method do), currently used for optimization/check of the parameters
		########################################
		ksresult_original = bdtreweighter.ks_test(originalcr, targetcr, variablescr, originalcr_weights)
		ksresult_model    = bdtreweighter.ks_test(originalcr, targetcr, variablescr, foldingcr_weights)	
		passedksresult    = bdtreweighter.ks_comparison(variablescr,ksresult_original,ksresult_model)
		if passedksresult==False: continue 
		###########################################################################################
		## GB ROC AUC Test Study (Very slow test, needs to train a classifier in cross-validation)
		############################################################################################
		roc_auc_nowts  = bdtreweighter.discrimination_test(originalcr,targetcr,originalcr_weights,bkgclassifierparams,"%s"%tag,"original",False)
		roc_auc_wts    = bdtreweighter.discrimination_test(originalcr,targetcr,foldingcr_weights,bkgclassifierparams,"%s"%tag,"model",False)
		passedresult   = bdtreweighter.discrimination_comparison(roc_auc_nowts,roc_auc_wts)
		if passedresult==False: continue
		mymodels.append([modelargs,roc_auc_wts])
	mymodels.sort(key=lambda s: s[1])

	return mymodels

def OptimizeModelParameters(dataset,bkgparams,bkgclassifierparams,weightname,categ,valflag,tag):
	print "\n"*2
	print "[INFO] Optimizing parameters for %s"%weightname
	print "...... Preparing data and building BDT-reweighting model for %s using the 3b/4b CR data"%weightname
	data_cr_3b_categ,data_sr_3b_categ,data_rest_3b_categ                  = SelectRegions(dataset,'3b',categ,valflag)
	del dataset
	data_cr_4b_categ,data_sr_4b_categ,data_rest_4b_categ                  = SelectRegions(data_rest_3b_categ,'4b',categ,valflag)
	print "...... Building BDT-reweighting or Transfer Factor model for %s using the 3b/4b CR data using grid"%weightname
	del data_sr_4b_categ,data_rest_4b_categ		

	#Prepare grid for multiprocessing
	processes = []
	threadNumber=4
	listNumber = 0
	skimFileListOfList = [ [] for i in range(threadNumber)]
	for fileName in bkgparams:
			skimFileListOfList[listNumber].append(fileName)
			listNumber = listNumber + 1
			if listNumber >= 4: 
				listNumber = 0
	#Create pool of processes, run them and save the output
	results = []
	p = mp.Pool(processes=4)
	inputs = [skimFileListOfList[0],skimFileListOfList[1],skimFileListOfList[2],skimFileListOfList[3]]
	func = partial(BuildReweightingModel, data_cr_3b_categ,data_cr_4b_categ,categ,'%s_%s'%(tag,weightname),bkgclassifierparams)
	results = p.map(func,inputs)
	p.close()
	#Collect the best results 
	bestresults = []
	for k in results: 
		if len(k)!=0: 
			for j in k:
				bestresults.append(j) 
	bestresults.sort(key=lambda s: s[1])
	print "[INFO]The best set of hyperparameters for %s are the following:"%weightname
	for res in bestresults:
		print "     *",res

	#Merge (concatenate) them and return the dataframe
	print "[INFO] Concatenating all events in the dataframe for the next weight"
	datasetwithweight = pandas.concat( (data_cr_3b_categ,data_sr_3b_categ,data_rest_3b_categ), ignore_index=True   )
	del data_cr_3b_categ,data_sr_3b_categ,data_rest_3b_categ,data_cr_4b_categ
	datasetwithweight = datasetwithweight.reset_index(drop=True)


	return bestresults,datasetwithweight

def RunReweightingModel(dataset,bkgparams,classifierparams,case,directory,tag,valflag,vbfflag,subcatflag):
	#Add weights for analysis
	if valflag==True  and vbfflag==False: #Validation GGF
		if subcatflag==1:
			modelargs,dataset=OptimizeModelParameters(dataset,bkgparams,classifierparams,'Weight_ValGGF1','GGF1',valflag,'%s'%(tag))
			if len(modelargs)==0: print "[ERROR] No good args were found for Weight_ValGGF1, returning dataframe with w=1"
			else: print "[INFO] Success for Weight_ValGGF1!!!"
		elif subcatflag==2:
			modelargs,dataset=OptimizeModelParameters(dataset,bkgparams,classifierparams,'Weight_ValGGF2','GGF2',valflag,'%s'%(tag))
			if len(modelargs)==0: print "[ERROR] No good args were found for Weight_ValGGF2, returning dataframe with w=1"
			else: print "[INFO] Success for Weight_ValGGF2!!!"
		else:
			modelargs,dataset=OptimizeModelParameters(dataset,bkgparams,classifierparams,'Weight_ValGGF','GGF',valflag,'%s'%(tag))
			if len(modelargs)==0: print "[ERROR] No good args were found for Weight_ValGGF" 
			else: print "[INFO] Success for Weight_ValGGF!!!"
	elif valflag==True and vbfflag==True: #Validation VBF
		if subcatflag==1:
			modelargs,dataset=OptimizeModelParameters(dataset,bkgparams,classifierparams,'Weight_ValVBF1','VBF1',valflag,'%s'%(tag)) 
			if len(modelargs)==0: print "[ERROR] No good args were found for Weight_ValVBF1, returning dataframe with w=1"
			else: print "[INFO] Success for Weight_ValVBF1!!!"
		elif subcatflag==2:
			print "[INFO] No grid-training needed for Weight_AnaVBF2!!!"
		else:
			modelargs,dataset=OptimizeModelParameters(dataset,bkgparams,classifierparams,'Weight_ValVBF','VBF',valflag,'%s'%(tag))
			if len(modelargs)==0: print "[ERROR] No good args were found for Weight_ValVBF, returning dataframe with w=1"
			else: print "[INFO] Success for Weight_ValVBF!!!"
	elif valflag==False and vbfflag==False: #Analysis GGF
		if subcatflag==1: 
			modelargs,dataset=OptimizeModelParameters(dataset,bkgparams,classifierparams,'Weight_AnaGGF1','GGF1',valflag,'%s'%(tag))
			if len(modelargs)==0: print "[ERROR] No good args were found for Weight_AnaGGF1, returning dataframe with w=1"
			else: print "[INFO] Success for Weight_AnaGGF1!!!"
		elif subcatflag==2:
			modelargs,dataset=OptimizeModelParameters(dataset,bkgparams,classifierparams,'Weight_AnaGGF2','GGF2',valflag,'%s'%(tag))
			if len(modelargs)==0: print "[ERROR] No good args were found for Weight_AnaGGF2, returning dataframe with w=1"
			else: print "[INFO] Success for Weight_AnaGGF2!!!"
		else:
			modelargs,dataset=OptimizeModelParameters(dataset,bkgparams,classifierparams,'Weight_AnaGGF','GGF',valflag,'%s'%(tag))
			if len(modelargs)==0: print "[ERROR] No good args were found for Weight_AnaGGF, returning dataframe with w=1"
			else: print "[INFO] Success for Weight_AnaGGF!!!"		
	elif valflag==False and vbfflag==True: #Analysis VBF
		if subcatflag==1: 
			modelargs,dataset=OptimizeModelParameters(dataset,bkgparams,classifierparams,'Weight_AnaVBF1','VBF1',valflag,'%s'%(tag))
			if len(modelargs)==0: print "[ERROR] No good args were found for Weight_AnaVBF1, returning dataframe with w=1"
			else: print "[INFO] Success for Weight_AnaVBF1!!!"
		elif subcatflag==2:
			print "[INFO] No grid-training needed for Weight_AnaVBF2!!!"
		else:
			modelargs,dataset=OptimizeModelParameters(dataset,bkgparams,classifierparams,'Weight_AnaVBF','VBF',valflag,'%s'%(tag))
			if len(modelargs)==0: print "[ERROR] No good args were found for Weight_AnaVBF, returning dataframe with w=1"
			else: print "[INFO] Success for Weight_AnaVBF!!!"
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
##########Make background miodel
print "[INFO] Making grid for background model . . . "
#Create folder to save training information
os.system("mkdir bkgtraining")
os.system("mkdir bkgtraining/mymodels")

#Get data and create panda dataframes with specific variables, a.k.a. slicing the data. Then, create background model based on control region data
datalist = []
datalist.append('outputskims/%s/%s/SKIM_Data.root'%(case,directory))
dataset  = data.root2pandas(datalist,'bbbbTree')

#classifierparameters
classifierparams=[40,0.3,2020]

grid =[
  [100,0.1,3,300,0.6,2020],
  [90, 0.1,3,300,0.6,2020],
  [80, 0.1,3,300,0.6,2020],
  [70, 0.1,3,300,0.6,2020],
  [60, 0.1,3,300,0.6,2020],
  [50, 0.1,3,300,0.6,2020],
  [40, 0.1,3,300,0.6,2020],
  [100,0.1,4,300,0.6,2020],
  [90, 0.1,4,300,0.6,2020],
  [80, 0.1,4,300,0.6,2020],
  [70, 0.1,4,300,0.6,2020],
  [60, 0.1,4,300,0.6,2020],
  [50, 0.1,4,300,0.6,2020],
  [40, 0.1,4,300,0.6,2020],
  [100,0.01,3,300,0.6,2020],
  [90, 0.01,3,300,0.6,2020],
  [80, 0.01,3,300,0.6,2020],
  [70, 0.01,3,300,0.6,2020],
  [60, 0.01,3,300,0.6,2020],
  [50, 0.01,3,300,0.6,2020],
  [40, 0.01,3,300,0.6,2020],
  [100,0.01,4,300,0.6,2020],
  [90, 0.01,4,300,0.6,2020],
  [80, 0.01,4,300,0.6,2020],
  [70, 0.01,4,300,0.6,2020],
  [60, 0.01,4,300,0.6,2020],
  [50, 0.01,4,300,0.6,2020],
  [40, 0.01,4,300,0.6,2020],
  [120,0.1,3,300,0.6,2020],
  [110,0.1,3,300,0.6,2020],
  [30, 0.1,3,300,0.6,2020],
  [20, 0.1,3,300,0.6,2020],
  [120,0.1,4,300,0.6,2020],
  [110,0.1,4,300,0.6,2020],
  [30, 0.1,4,300,0.6,2020],
  [20, 0.1,4,300,0.6,2020],
  [120,0.01,3,300,0.6,2020],
  [110,0.01,3,300,0.6,2020],
  [30, 0.01,3,300,0.6,2020],
  [20, 0.01,3,300,0.6,2020],
  [120,0.01,4,300,0.6,2020],
  [110,0.01,4,300,0.6,2020],
  [30, 0.01,4,300,0.6,2020],
  [20, 0.01,4,300,0.6,2020],
  [120, 0.1,2,300,0.6,2020],
  [110, 0.1,2,300,0.6,2020],
  [100, 0.1,2,300,0.6,2020],
  [90,  0.1,2,300,0.6,2020],
  [80,  0.1,2,300,0.6,2020],
  [70,  0.1,2,300,0.6,2020],
  [60,  0.1,2,300,0.6,2020],
  [50,  0.1,2,300,0.6,2020],
  [40,  0.1,2,300,0.6,2020],
  [120,0.01,2,300,0.6,2020],
  [110,0.01,2,300,0.6,2020],
  [100,0.01,2,300,0.6,2020],
  [90, 0.01,2,300,0.6,2020],
  [80, 0.01,2,300,0.6,2020],
  [70, 0.01,2,300,0.6,2020],
  [60, 0.01,2,300,0.6,2020],
  [50, 0.01,2,300,0.6,2020],
  [40, 0.01,2,300,0.6,2020],
  [120, 0.1,5,300,0.6,2020],
  [110, 0.1,5,300,0.6,2020],
  [100, 0.1,5,300,0.6,2020],
  [90,  0.1,5,300,0.6,2020],
  [80,  0.1,5,300,0.6,2020],
  [70,  0.1,5,300,0.6,2020],
  [60,  0.1,5,300,0.6,2020],
  [50,  0.1,5,300,0.6,2020],
  [40,  0.1,5,300,0.6,2020],
  [30,  0.1,5,300,0.6,2020],
  [20,  0.1,5,300,0.6,2020],
  [170, 0.1,5,300,0.6,2020],
  [150, 0.1,5,300,0.6,2020],
  [150, 0.1,5,300,0.6,2020],
  [140, 0.1,5,300,0.6,2020],
  [130, 0.1,5,300,0.6,2020],
  [170, 0.1,4,300,0.6,2020],
  [150, 0.1,4,300,0.6,2020],
  [150, 0.1,4,300,0.6,2020],
  [140, 0.1,4,300,0.6,2020],
  [130, 0.1,4,300,0.6,2020],
  [170, 0.1,3,300,0.6,2020],
  [150, 0.1,3,300,0.6,2020],
  [150, 0.1,3,300,0.6,2020],
  [140, 0.1,3,300,0.6,2020],
  [130, 0.1,3,300,0.6,2020],
  [170, 0.1,2,300,0.6,2020],
  [150, 0.1,2,300,0.6,2020],
  [150, 0.1,2,300,0.6,2020],
  [140, 0.1,2,300,0.6,2020],
  [130, 0.1,2,300,0.6,2020],
  [180, 0.1,5,300,0.6,2020],
  [190, 0.1,5,300,0.6,2020],
  [200, 0.1,5,300,0.6,2020],
  [210, 0.1,5,300,0.6,2020],
  [220, 0.1,5,300,0.6,2020],
  [180, 0.1,4,300,0.6,2020],
  [190, 0.1,4,300,0.6,2020],
  [200, 0.1,4,300,0.6,2020],  
  [210, 0.1,4,300,0.6,2020],
  [220, 0.1,4,300,0.6,2020],
  [180, 0.1,3,300,0.6,2020],
  [190, 0.1,3,300,0.6,2020],
  [200, 0.1,3,300,0.6,2020],
  [210, 0.1,3,300,0.6,2020],
  [220, 0.1,3,300,0.6,2020],
  [180, 0.1,2,300,0.6,2020],
  [190, 0.1,2,300,0.6,2020],
  [200, 0.1,2,300,0.6,2020],
  [210, 0.1,2,300,0.6,2020],
  [220, 0.1,2,300,0.6,2020],
]

starttime = time.time()
#Add for analysis region
dataset = RunReweightingModel(dataset,grid,classifierparams,case,directory,tag,False,False,1)
dataset = RunReweightingModel(dataset,grid,classifierparams,case,directory,tag,False,False,2)
dataset = RunReweightingModel(dataset,grid,classifierparams,case,directory,tag,False,True,1)

###Add for validation region
dataset = RunReweightingModel(dataset,grid,classifierparams,case,directory,tag,True,False,1)
dataset = RunReweightingModel(dataset,grid,classifierparams,case,directory,tag,True,False,2)
dataset = RunReweightingModel(dataset,grid,classifierparams,case,directory,tag,True,True,1)

print('Running the background model grid took {} seconds'.format(time.time() - starttime))