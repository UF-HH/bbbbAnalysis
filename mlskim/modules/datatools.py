import numpy 
import os
import math
import matplotlib
import root_numpy
import pandas
import glob
import ROOT
import root_pandas
#from keras.models import load_model
from hep_ml import reweight
from root_numpy import root2array
from matplotlib import pyplot as plt
from numpy.lib.recfunctions import stack_arrays
from sklearn.model_selection import train_test_split
from hep_ml.metrics_utils import ks_2samp_weighted
from sklearn.ensemble import GradientBoostingClassifier
from sklearn.model_selection import train_test_split
from sklearn.metrics import roc_auc_score
from matplotlib.colors import LogNorm
from sklearn.externals.joblib import dump, load
#My modules
import modules.plotter as plotter
import modules.bdtreweighter as bdtreweighter

def PrepareFeaturesFromSkim(skim,features,tag):
	#Get features
	data = skim[features]
	#Convert it as input in the model
	X = data.as_matrix()
	scaler = load('./mymodels/std_scaler_%s.bin'%tag)
	X = scaler.transform(X)
	return X

def CalculateSumw2Errors(original,original_weights,variable,fmt):
	sumw2 = [] 
	nbins = fmt[0]
	binwidth = (fmt[2]-fmt[1])/fmt[0]
	bines = [ [((x)*binwidth)+ fmt[1], ((x+1)*binwidth)+fmt[1]] for x in range(0,int(nbins)) ]
	for x in range(0,int(nbins)):
		ws=0 
		for k in range(0,len(original[variable])): 
			if original[variable][k]<=bines[x][1] and original[variable][k]>bines[x][0]: ws+= (original_weights[k]*original_weights[k])
		sumw2.append(ws)
	entries_original_error = numpy.sqrt(sumw2)
	return entries_original_error

def PrepareModel(tag):
	model = load_model(tag)
	print("Loaded model and weights from mymodels")	
	return model

def GetFileList(files_path,tree_name):
	# -- create list of .root files to process
	filelist = glob.glob(files_path)
	files    = []
	# -- check that the file has the bbbbTree, otherwise don't add it
	for file in filelist:
		ftmp       = ROOT.TFile.Open(file)
		if ftmp.GetListOfKeys().Contains(tree_name)==False: 
			print "This file doesn't have a tree, have a look at it: ",file	
			continue		
		if ftmp.GetListOfKeys().Contains('eff_histo')==False: 
			print "This file doesn't have a eff_histo, have a look at it: ",file
			continue
		tree       = ftmp.Get(tree_name)
		events     = tree.GetEntries()
		if events!=0: 
			files.append(file)
		else:
			print "This file doesn't have entries in the tree, have a look at it: ",file		
		ftmp.Close()
	return files	

def root2pandas(files, tree_name, **kwargs):
	# -- process ntuples into rec arrays
	ss = stack_arrays([root2array(fpath, tree_name, **kwargs).view(numpy.recarray) for fpath in files])
	try:
		return pandas.DataFrame(ss)
	except Exception:
		return pandas.DataFrame(ss.data)

def flatten(column):
	try:
		return np.array([v for e in column for v in e])
	except (TypeError, ValueError):
		return column

def preparedataformodel(ioriginal, itarget, variables):
	original  = ioriginal[variables].reset_index(drop=True)
	target    = itarget[variables].reset_index(drop=True)
	tfactor   = float( float(len(target.index)) / float(len(original.index)) )
	original_weights = numpy.ones(dtype='float64',shape=len(original))
	original_weights = numpy.multiply(original_weights,tfactor)
	target_weights   = numpy.ones(dtype='float64',shape=len(target))
	return original,target,original_weights,target_weights,tfactor

def preparedataforprediction(ioriginal,tfactor, variables):
	original         = ioriginal[variables].reset_index(drop=True)
	original_weights = numpy.ones(dtype='float64',shape=len(original))
	original_weights = numpy.multiply(original_weights,tfactor)
	return original,original_weights

def preparemcforprediction(ioriginal,variables):
	original         = ioriginal[variables].reset_index(drop=True)
	original_weights = ioriginal['MC_Weight']
	return original,original_weights 

def fitreweightermodel(original,target,original_weights,target_weights,tfactor, model_args):
	print "[INFO] Fitting BDT-reweighter model ..."
	model                = bdtreweighter.reweightermodel(original,target,original_weights,target_weights,model_args) 
	ws_unnormalized      = model.predict_weights(original,original_weights,lambda x: numpy.mean(x, axis=0))
	#Normalized all reweighter weights to 1
	weights = numpy.multiply(ws_unnormalized, (1/ws_unnormalized.sum())  )
	#Give them the normalization using the transfer factor derived in control regions
	totalnorm  = int(len(original))*tfactor
	weights    = numpy.multiply(weights,totalnorm)
	print "[INFO] Event yields report in control region derivation:"
	print "   -The sum of original weights                 = ",int(len(original)),"+/-",math.sqrt(len(original) )
	print "   -The sum of target weights                   = ",int(len(target)),"+/-",math.sqrt(len(target))
	print "   -The transfer factor                         = ",tfactor,"+/-",tfactor*math.sqrt(  (math.sqrt(len(target))/len(target))**2 + (math.sqrt(len(original))/len(original))**2   )
	print "   -The sum of model weights                    = ",weights.sum(),"+/-",math.sqrt(numpy.square(weights).sum() )
	return weights,model

def fittransferfactormodel(original,target,original_weights,target_weights,tfactor):
	print "[INFO] Fitting transfer factor model ..."
	print "[INFO] Event yields report in control region derivation:"
	print "   -The sum of original weights                 = ",int(len(original)),"+/-",math.sqrt(len(original) )
	print "   -The sum of target weights                   = ",int(len(target)),"+/-",math.sqrt(len(target))
	print "   -The transfer factor                         = ",tfactor,"+/-",tfactor*math.sqrt(  (math.sqrt(len(target))/len(target))**2 + (math.sqrt(len(original))/len(original))**2   )
	print "   -The sum of model weights                    = ",original_weights.sum(),"+/-",math.sqrt(numpy.square(original_weights).sum() )

def getmodelweights(original,original_weights,target,target_weights,model,tfactor,valflag,srflag):
	print "[INFO] Running prediction from BDT-reweighter ..."
	ws_unnormalized = model.predict_weights(original,original_weights,lambda x: numpy.mean(x, axis=0))
	#Normalized all reweighter weights to 1
	weights = numpy.multiply(ws_unnormalized, (1/ws_unnormalized.sum())  )	
	#Give them the normalization using the transfer factor derived in control regions
	totalnorm  = int(len(original))*tfactor
	weights    = numpy.multiply(weights,totalnorm)
	print "[INFO] Event yields report in prediction:"
	print "   -The renormalization factor from control regions = ",tfactor
	print "   -The sum of model weights                        = ",weights.sum(),"+/-",math.sqrt(numpy.square(weights).sum() )
	if valflag==False and srflag==True:
		print "   -The sum of target weights                       =  Blinded" 
	else:
		print "   -The sum of target weights                       = ",int(len(target)),"+/-",math.sqrt(len(target))
	return weights

def getmodeltransferfactorweights(original,original_weights,target,target_weights,tfactor,valflag,srflag):
	print "[INFO] Running prediction from transferfactor model ..."
	print "[INFO] Event yields report in prediction:"
	print "   -The renormalization factor from control regions = ",tfactor
	print "   -The sum of model weights                        = ",original_weights.sum(),"+/-",math.sqrt(numpy.square(original_weights).sum() )
	if valflag==False and srflag==True:
		print "   -The sum of target weights                       =  Blinded" 
	else:
		print "   -The sum of target weights                       = ",int(len(target)),"+/-",math.sqrt(len(target))

def getmodelweightsformc(original,original_weights,model,tfactor):
	print "[INFO] Running prediction from BDT-reweighter ..."
	ws_unnormalized = model.predict_weights(original,original_weights,lambda x: numpy.mean(x, axis=0))
	#Normalized all reweighter weights to 1
	weights = numpy.multiply(ws_unnormalized, (1/ws_unnormalized.sum())  )	
	#Give them the normalization using the transfer factor derived in control regions
	totalnorm  = original_weights.sum()*tfactor
	weights    = numpy.multiply(weights,totalnorm)
	print "[INFO] Event yields report in prediction:"
	print "   -The sum of model weights (before factor)        = ",original_weights.sum(),"+/-",math.sqrt(numpy.square(original_weights).sum() )
	print "   -The renormalization factor from control regions = ",tfactor
	print "   -The sum of model weights (after factor)         = ",weights.sum(),"+/-",math.sqrt(numpy.square(weights).sum() )
	return weights

def getmodeltransferfactorweightsformc(original,original_weights,tfactor):
	print "[INFO] Running prediction from transferfactor model ..."
	#Normalized all reweighter weights to 1
	weights = numpy.multiply(original_weights, (1/original_weights.sum())  )	
	#Give them the normalization using the transfer factor derived in control regions
	totalnorm  = original_weights.sum()*tfactor
	weights    = numpy.multiply(weights,totalnorm)
	print "[INFO] Event yields report in prediction:"
	print "   -The sum of model weights (before factor)        = ",original_weights.sum(),"+/-",math.sqrt(numpy.square(original_weights).sum() )
	print "   -The renormalization factor from control regions = ",tfactor
	print "   -The sum of model weights (after factor)         = ",weights.sum(),"+/-",math.sqrt(numpy.square(weights).sum() )
	return weights

def pandas2root(tree_dataframe, tree_name, rootfile_name):
	tree_dataframe.to_root('%s'%rootfile_name, key='%s'%tree_name)

def roothist2root(inputrootfile_name, hist_name, outputrootfile_name):
	# -- create list of .root files to process
	files = inputrootfile_name
	# ---read the list of files and get the histogram
	if len(files)==0: print "No files are found in the input directory!,expect this to crash..."
	file0 = ROOT.TFile.Open(files[0])
	histo = file0.Get(hist_name)
	# ---if there are more files, get the histogram and merge it
	if len(files)>1:
		for k in range(1, len(files)):
			ftmp = ROOT.TFile.Open(files[k])
			h = ftmp.Get(hist_name)		
			histo.Add(h)
			ftmp.Close()
	#save histograms in a root file
	hfile = ROOT.TFile('%s'%outputrootfile_name, 'RECREATE')
	histo.Write()
	hfile.Write()
	hfile.Close()  


def pandas2historoot(dataframe,hist_name, rootfile_name):
	ntotal = len(dataframe)
	print "N total is ",ntotal
	ntotalweight = dataframe['MLWeight'].sum()
	print "N total weighted is ",ntotalweight
	hfile =  ROOT.TFile('%s'%rootfile_name, 'RECREATE')
	eff_histo=ROOT.TH1D(hist_name,hist_name,6,0,6)
	eff_histo.Fill("Ntot_uw",ntotal)
	eff_histo.Fill("Ntot_w",ntotalweight)
	eff_histo.Fill("Ntrig_uw",ntotal)
	eff_histo.Fill("Ntrig_w",ntotalweight)
	eff_histo.Fill("Nsel_uw",ntotal)
	eff_histo.Fill("Nsel_w",ntotalweight)
	eff_histo.Write()
	hfile.Write()
	hfile.Close()


def mergedata2root(treefile,histofile,rootfile_name):
	os.system('hadd -f %s %s %s'%(rootfile_name,treefile,histofile) )
	os.system('rm %s %s'%(treefile,histofile) )

def TotalMCEvents(sample):
	file = ROOT.TFile.Open('%s'%sample)
	h=ROOT.TH1D()
	h=file.Get('eff_histo')
	htmp=h.Clone()
	w=htmp.GetBinContent(2)
	file.Close()
	return w

def AddMCWeight(dataframe,lumi,file):
	print TotalMCEvents(file)
	scale                   = float((lumi*dataframe.iloc[0]['XS']))/TotalMCEvents(file)
	dataframe['MC_Weight']  = scale*dataframe['bTagScaleFactor_central']*dataframe['PUWeight']*dataframe['genWeight']
	return dataframe