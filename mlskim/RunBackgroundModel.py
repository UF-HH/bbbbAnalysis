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

def SelectControlRegion(sample,btagregion,category,validation):
	datacr,datarest = selector.eventselection(sample,btagregion,category,'CR',validation)
	return datacr,datarest

def BuildReweightingModel(data_3b, data_4b,category,tag):
	print "[INFO] Processing predicted model for %s"%(tag)
	############################################################################
	##Let's slice data one more time to have the inputs for the bdt reweighting#
	############################################################################
	if category =='GGF':
		 variablescr = ['H1_pt', 'H2_pt','H1_eta', 'H2_eta','nJet','H1_bb_deltaR','H2_bb_deltaR','H1_bb_deltaPhi','H2_bb_deltaPhi']
		 modelargs= [100,0.1,5,300,0.5]
	elif category == 'VBF':
		 variablescr = ['H1_pt', 'H2_pt','H1_eta', 'H2_eta','nJet','JJ_j1_qgl','JJ_j2_qgl','JJ_m','j1j2_deltaEta']
		 modelargs= [80,0.1,5,200,0.5]
	else:
		 variablescr = ['H1_pt', 'H2_pt','H1_eta', 'H2_eta','nJet']  	
	originalcr,targetcr,originalcr_weights,targetcr_weights = data.preparedataformodel(data_3b,data_4b,variablescr)
	#plotter.Draw1DHistosComparison(originalcr, targetcr, variablescr, originalcr_weights,True,"original_%s"%tag)
	#######################################
	##Folding Gradient Boosted Reweighter
	#######################################
	foldingcr_weights,reweightermodel,transferfactor = data.fitreweightermodel(originalcr,targetcr,originalcr_weights,targetcr_weights,modelargs)  
	#plotter.Draw1DHistosComparison(originalcr, targetcr, variablescr, foldingcr_weights,True,"model_%s"%tag)
	########################################
	##GB ROC AUC
	########################################
	bdtreweighter.roc_auc_measurement(originalcr,targetcr,originalcr_weights,foldingcr_weights)
	########################################
	##Update 3b dataframe for modeling
	########################################
	return foldingcr_weights,reweightermodel,transferfactor

def CreatePredictionModel(reweightermodel,transferfactor,data_3b,category):
	############################################################################
	##Let's slice data one more time to have the inputs for the bdt reweighting#
	############################################################################
	if category =='GGF':
		 variables = ['H1_pt', 'H2_pt','H1_eta', 'H2_eta','nJet','H1_bb_deltaR','H2_bb_deltaR','H1_bb_deltaPhi','H2_bb_deltaPhi']
	elif category == 'VBF':
		 variables = ['H1_pt', 'H2_pt','H1_eta', 'H2_eta','nJet','JJ_j1_qgl','JJ_j2_qgl','JJ_m','j1j2_deltaEta']
	else:
		 variables = ['H1_pt', 'H2_pt','H1_eta', 'H2_eta','nJet']          
	original,original_weights = data.preparedataforprediction(data_3b,transferfactor,variables)
	########################################3############
	##Folding Gradient Boosted Reweighter (and DQM plots)
	#####################################################
	folding_weights= data.getmodelweights(original,original_weights,reweightermodel,transferfactor)
	return folding_weights


def AddModelWeight(dataset,weightname,categ,valflag):
	#Slice the data sample to take only events with three/four b-tags among the two categories
	data_cr_3b_categ,data_rest_3b_categ = SelectControlRegion(dataset,'3b',categ,valflag)
	data_cr_4b_categ,data_rest_4b_categ = SelectControlRegion(dataset,'4b',categ,valflag)
	#Get weights ,model, transferfactor for CR data
	print "[INFO] Building BDT-reweighting model for %s in the CR data"%weightname
	weights_cr_categ,reweightermodel_categ,transferfactor_categ=BuildReweightingModel(data_cr_3b_categ,data_cr_4b_categ,categ,'CR_%s_%s'%(categ,weightname) )
	#Get weights for the dataset
	print "[INFO] Calculating BDT-reweighting model prediction %s in the other data"%weightname
	weights_rest_categ=CreatePredictionModel(reweightermodel_categ,transferfactor_categ,data_rest_3b_categ,categ)
	#Add the weights to the CR & the rest
	print "[INFO] Adding weight %s to the dataframes"%weightname
	data_cr_3b_categ[weightname]    = weights_cr_categ 
	data_rest_3b_categ[weightname]  = weights_rest_categ 
	#Merge (concatenate) them
	print "[INFO] Returning dataframe"
	datasetwithweight = pandas.concat( (data_cr_3b_categ,data_rest_3b_categ), ignore_index=True   )
	del dataset
	return datasetwithweight

def RunReweightingModel(dataset,case):
	datasetwithweights1=AddModelWeight(dataset,'Weight_210_GGF','GGF',True)
	datasetwithweights2=AddModelWeight(datasetwithweights1,'Weight_210_VBF','VBF',True)
	datasetwithweights3=AddModelWeight(datasetwithweights2,'Weight_110_GGF','GGF',False)
	datasetwithweights =AddModelWeight(datasetwithweights3,'Weight_110_VBF','VBF',False)
	print 'Here comes the dataset with reweighting-model weights . . .'
	print datasetwithweights.head()
	#Save modeling dataframe tree to a root file
	data.pandas2root(datasetwithweights,'bbbbTree', 'outputskims/%s/SKIM_BKG_MODEL_tree.root'%case)
	data.roothist2root('Data','%s'%(case),'eff_histo','outputskims/%s/SKIM_BKG_MODEL_hist.root'%(case)  )
	os.system("hadd -f outputskims/%s/SKIM_BKG_MODEL.root outputskims/%s/SKIM_BKG_MODEL_tree.root outputskims/%s/SKIM_BKG_MODEL_hist.root"%(case,case,case))
	os.system("rm outputskims/%s/SKIM_BKG_MODEL_tree.root outputskims/%s/SKIM_BKG_MODEL_hist.root"%(case,case) )	

def RunReweightingModelwithSignal(dataset,signal,case,tag):
	dataset_signal     = pandas.concat( (dataset,signal),  ignore_index=True)
	datasetwithweights1=AddModelWeight(dataset_signal,'Weight_210_GGF','GGF',True)
	datasetwithweights2=AddModelWeight(datasetwithweights1,'Weight_210_VBF','VBF',True)
	datasetwithweights3=AddModelWeight(datasetwithweights2,'Weight_110_GGF','GGF',False)
	datasetwithweights =AddModelWeight(datasetwithweights3,'Weight_110_VBF','VBF',False)
	print 'Here comes the dataset with reweighting-model weights . . .'
	print datasetwithweights.head()
	#Save modeling dataframe tree to a root file
	data.pandas2root(datasetwithweights,'bbbbTree', 'outputskims/%s/SKIM_BKG_MODEL_%s_tree.root'%(case,tag) )
	data.roothist2root('Data','%s'%(case),'eff_histo','outputskims/%s/SKIM_BKG_MODEL_%s_hist.root'%(case,tag)  )
	os.system("hadd -f outputskims/%s/SKIM_BKG_MODEL_%s.root outputskims/%s/SKIM_BKG_MODEL_%s_tree.root outputskims/%s/SKIM_BKG_MODEL_%s_hist.root"%(case,tag,case,tag,case,tag))
	os.system("rm outputskims/%s/SKIM_BKG_MODEL_%s_tree.root outputskims/%s/SKIM_BKG_MODEL_%s_hist.root"%(case,tag,case,tag) )

#############ML CODE IS BELOW ######################
#Get data and create panda dataframes with specific variables, a.k.a. slicing the data AND create model
dataset2016    = data.root2pandas('outputskims/2016case2/SKIM_Data.root','bbbbTree')
RunReweightingModel(dataset2016,'2016case2')
dataset2017  = data.root2pandas('outputskims/2017case2/SKIM_Data.root','bbbbTree')
RunReweightingModel(dataset2017,'2017case2')
dataset2018  = data.root2pandas('outputskims/2018case2/SKIM_Data.root','bbbbTree')
RunReweightingModel(dataset2018,'2018case2')

#Get data and create panda dataframes with specific variables, a.k.a. slicing the data AND create model (2016 analysis selection)
dataset2016analysis  = data.root2pandas('outputskims/2016case3/SKIM_Data.root','bbbbTree')
RunReweightingModel(dataset2016analysis,'2016case3')

#Get data and create panda dataframes with specific variables, a.k.a. slicing the data AND create model but infected with signal
vbfsignal2016_8000  = data.root2pandas('outputskims/BKGSTUDIES2016/SKIM_VBFHHTo4B_CV_1_C2V_1_C3_1_13TeV-madgraph_8000.root','bbbbTree')
vbfsignal2016_10000 = data.root2pandas('outputskims/BKGSTUDIES2016/SKIM_VBFHHTo4B_CV_1_C2V_1_C3_1_13TeV-madgraph_10000.root','bbbbTree')
vbfsignal2016_12000 = data.root2pandas('outputskims/BKGSTUDIES2016/SKIM_VBFHHTo4B_CV_1_C2V_1_C3_1_13TeV-madgraph_12000.root','bbbbTree')
ggfsignal2016_3000  = data.root2pandas('outputskims/BKGSTUDIES2016/SKIM_GluGluToHHTo4B_node_SM_13TeV-madgraph_3000.root','bbbbTree')
ggfsignal2016_6000  = data.root2pandas('outputskims/BKGSTUDIES2016/SKIM_GluGluToHHTo4B_node_SM_13TeV-madgraph_6000.root','bbbbTree')
ggfsignal2016_9000  = data.root2pandas('outputskims/BKGSTUDIES2016/SKIM_GluGluToHHTo4B_node_SM_13TeV-madgraph_9000.root','bbbbTree')
RunReweightingModelwithSignal(dataset2016,vbfsignal2016_12000,'2016case2','VBF_12000')
RunReweightingModelwithSignal(dataset2016,ggfsignal2016_9000, '2016case2','GGF_9000')
RunReweightingModelwithSignal(dataset2016,vbfsignal2016_8000,'2016case2','VBF_8000')
RunReweightingModelwithSignal(dataset2016,ggfsignal2016_3000,'2016case2','GGF_3000')
RunReweightingModelwithSignal(dataset2016,vbfsignal2016_10000,'2016case2','VBF_10000')
RunReweightingModelwithSignal(dataset2016,ggfsignal2016_6000, '2016case2','GGF_6000')