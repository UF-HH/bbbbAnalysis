import numpy 
import os
import root_numpy
import pandas
import glob
from hep_ml import reweight
from root_numpy import root2array
from numpy.lib.recfunctions import stack_arrays
#My modules
import modules.datatools as data
import modules.plotter as plotter
import modules.bdtreweighter as bdtreweighter
import modules.selections as selector

def SelectRegions(sample,btagregion,category,validation):
	datacr = selector.eventselection(sample,btagregion,category,'CR',validation)
	return datacr
def BuildReweightingModel(data_3b, data_4b,category,tag):
	print "[INFO] Processing predicted model for %s"%(tag)
	############################################################################
	##Let's slice data one more time to have the inputs for the bdt reweighting#
	############################################################################
	if category =='GGF':
		 variablescr = ['H1_pt', 'H2_pt','H1_eta', 'H2_eta','nJet','H1_bb_deltaR','H2_bb_deltaR','H1_bb_deltaPhi','H2_bb_deltaPhi']
	elif category == 'VBF':
		 variablescr = ['H1_pt', 'H2_pt','H1_eta', 'H2_eta','nJet','JJ_j1_qgl','JJ_j2_qgl','JJ_m','j1j2_deltaEta']
	else:
		 variablescr = ['H1_pt', 'H2_pt','H1_eta', 'H2_eta','nJet']  	
	originalcr,targetcr,originalcr_weights,targetcr_weights = data.preparedataformodel(data_3b,data_4b,variablescr)
	plotter.Draw1DHistosComparison(originalcr, targetcr, variablescr, originalcr_weights,True,"original_%s"%tag)
	#######################################
	##Folding Gradient Boosted Reweighter
	#######################################
	foldingcr_weights,reweightermodel,transferfactor = data.fitreweightermodel(originalcr,targetcr,originalcr_weights,targetcr_weights)  
	plotter.Draw1DHistosComparison(originalcr, targetcr, variablescr, foldingcr_weights,True,"model_%s"%tag)
	########################################
	##GB ROC AUC
	########################################
	bdtreweighter.roc_auc_measurement(originalcr,targetcr,originalcr_weights,foldingcr_weights)
	########################################
	##Update 3b dataframe for modeling
	########################################
	return data_3b,reweightermodel,transferfactor

def CreatePredictionModel(reweightermodel,transferfactor,data_3b,data_4b,category):
	############################################################################
	##Let's slice data one more time to have the inputs for the bdt reweighting#
	############################################################################
	if category =='GGF':
		 variables = ['H1_pt', 'H2_pt','H1_eta', 'H2_eta','nJet','H1_bb_deltaR','H2_bb_deltaR','H1_bb_deltaPhi','H2_bb_deltaPhi']
	elif category == 'VBF':
		 variables = ['H1_pt', 'H2_pt','H1_eta', 'H2_eta','nJet','JJ_j1_qgl','JJ_j2_qgl','JJ_m','j1j2_deltaEta']
	else:
		 variables = ['H1_pt', 'H2_pt','H1_eta', 'H2_eta','nJet']          
	original,target,original_weights,target_weights = data.preparedataformodel(data_3b,data_4b,variables)
	########################################3############
	##Folding Gradient Boosted Reweighter (and DQM plots)
	#####################################################
	folding_weights= data.getmodelweights(original,original_weights,reweightermodel,transferfactor)
	return folding_weights

def RunPredictionModel(dataset,case):
	#Slice the data sample to take only events with three/four b-tags among the two categories
	val_cr_3b_ggf = SelectRegions(dataset,'3b','GGF',validation=True)
	val_cr_4b_ggf = SelectRegions(dataset,'4b','GGF',validation=True)
	val_cr_3b_vbf = SelectRegions(dataset,'3b','VBF',validation=True)
	val_cr_4b_vbf = SelectRegions(dataset,'4b','VBF',validation=True)    

	hh_cr_3b_ggf  = SelectRegions(dataset,'3b','GGF',validation=False)
	hh_cr_4b_ggf  = SelectRegions(dataset,'4b','GGF',validation=False)
	hh_cr_3b_vbf  = SelectRegions(dataset,'3b','VBF',validation=False)
	hh_cr_4b_vbf  = SelectRegions(dataset,'4b','VBF',validation=False)    

	##VALIDATION MODEL FIRST
	#Get data,model,transferfactor from CR
	print "[INFO] Building BDT-reweighting model"
	data_val_cr_ggf,reweightermodel_val_ggf,transferfactor_val_ggf=BuildReweightingModel(val_cr_3b_ggf,val_cr_4b_ggf,'GGF','VAL_CR_GGF')
	data_val_cr_vbf,reweightermodel_val_vbf,transferfactor_val_vbf=BuildReweightingModel(val_cr_3b_vbf,val_cr_4b_vbf,'VBF','VAL_CR_VBF')
	data_hh_cr_ggf, reweightermodel_hh_ggf, transferfactor_hh_ggf =BuildReweightingModel(hh_cr_3b_ggf,hh_cr_4b_ggf,'GGF','HH_CR_GGF')
	data_hh_cr_vbf, reweightermodel_hh_vbf, transferfactor_hh_vbf =BuildReweightingModel(hh_cr_3b_vbf,hh_cr_4b_vbf,'VBF','HH_CR_VBF')    

	#Estimate weights for the dataset
	print "[INFO] Calculating BDT-reweighting model prediction"
	data_hh_ggf =CreatePredictionModel(reweightermodel_hh_ggf, transferfactor_hh_ggf, dataset,dataset,'GGF')
	data_hh_vbf =CreatePredictionModel(reweightermodel_hh_vbf, transferfactor_hh_vbf, dataset,dataset,'VBF')
	data_val_ggf=CreatePredictionModel(reweightermodel_val_ggf,transferfactor_val_ggf,dataset,dataset,'GGF')
	data_val_vbf=CreatePredictionModel(reweightermodel_val_vbf,transferfactor_val_vbf,dataset,dataset,'VBF')    

	#Add weights to the dataset
	print "[INFO] Adding BDT-reweighting model prediction"
	dataset['Weight_110_GGF'] = data_hh_ggf
	dataset['Weight_110_VBF'] = data_hh_vbf
	dataset['Weight_210_GGF'] = data_val_ggf
	dataset['Weight_210_VBF'] = data_val_vbf    

	print 'Here comes the dataset with reweighting model . . .'
	print dataset.head()
	#Save modeling dataframe tree to a root file
	data.pandas2root(dataset,'bbbbTree', 'outputskims/%s/SKIM_BKG_MODEL_tree.root'%case)
	data.roothist2root('Data','%s'%(case),'eff_histo','outputskims/%s/SKIM_BKG_MODEL_hist.root'%(case)  )
	os.system("hadd -f outputskims/%s/SKIM_BKG_MODEL.root outputskims/%s/SKIM_BKG_MODEL_tree.root outputskims/%s/SKIM_BKG_MODEL_hist.root"%(case,case,case))
	os.system("rm outputskims/%s/SKIM_BKG_MODEL_tree.root outputskims/%s/SKIM_BKG_MODEL_hist.root"%(case,case) )	

#############ML CODE IS BELOW ######################
#Get data and create panda dataframes with specific variables, a.k.a. slicing the data AND create model
dataset2016  = data.root2pandas('outputskims/2016case2/SKIM_Data.root','bbbbTree')
RunPredictionModel(dataset2016,'2016case2')
dataset2017  = data.root2pandas('outputskims/2017case2/SKIM_Data.root','bbbbTree')
RunPredictionModel(dataset2017,'2017case2')
dataset2018  = data.root2pandas('outputskims/2018case2/SKIM_Data.root','bbbbTree')
RunPredictionModel(dataset2018,'2018case2')