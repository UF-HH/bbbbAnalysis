import os
import root_numpy
import pandas
import glob
import ast
import argparse
import sys
import copy
import numpy
import math
import pickle
from  ConfigParser import *
from root_numpy import root2array
from numpy.lib.recfunctions import stack_arrays
from sklearn.externals.joblib import dump, load
import time
import multiprocessing as mp
import ROOT
#My modules
import modules.datatools as data
import modules.plotter as plotter
import modules.bdtreweighter as bdtreweighter
import modules.selections as selector

def AddGGFKiller(skim,ggfkillerfile):
	# open picked file and use it back again
	#print '  *Adding the ggfkiller in %s'%ggfkillerfile
	infile     = open(ggfkillerfile,'rb')
	ggfkiller  = pickle.load(infile)
	infile.close()
	variables  = ggfkiller.get('vars')
	X          = skim[list(set(variables))] 
	y_pred     = ggfkiller.get('model').predict_proba(X)[:,1]
	return y_pred

def AddGGFMVA(skim,ggfmvafile):
	# open picked file and use it back again
	#print '  *Adding the ggfmva in %s'%ggfmvafile
	infile     = open(ggfmvafile,'rb')
	ggfmva     = pickle.load(infile)
	infile.close()
	variables  = ggfmva.get('vars')
	X          = skim[list(set(variables))] 
	y_pred     = ggfmva.get('model').predict_proba(X)[:,1]
	return y_pred

def AddGGFMVAV(originalskim,ggfmvafile):
		# open picked file and use it back again
		skim = originalskim.copy()
		#print '  *Adding the ggfmva in %s'%ggfmvafile
		infile       = open(ggfmvafile,'rb')
		ggfmva       = pickle.load(infile)
		infile.close()
		variables    = ggfmva.get('vars')
		skim['H1_m'] = skim['H1_m_v']
		skim['H2_m'] = skim['H2_m_v']
		X            = skim[list(set(variables))]
		y_pred       = ggfmva.get('model').predict_proba(X)[:,1]
		return y_pred

def TreeDevelopment(sample,filelist,columns_input,case,directory,lepveto,trigmat,ggfkillerfile,ggfmva1file1,ggfmva2file1,ggfmva1file2,ggfmva2file2,ggfmva1file1btag,ggfmva2file1btag,ggfmva1file2btag,ggfmva2file2btag):

	#Make initial skim using defined branches
	skim   = data.root2pandas(filelist,'bbbbTree', branches=columns_input)

	##############---------------Apply cuts like the lepton veto and trigger matching
	#Veto on leptons?
	if lepveto==True: skim = skim[(skim.IsolatedElectron_Multiplicity==0) & (skim.IsolatedMuon_Multiplicity==0)]
	#TriggerMatching?
	if trigmat==True:
	   skim['TrigMatched'] = 1
	   if directory == '2016': 
		  skim = skim[( (skim.HLT_DoubleJet90_Double30_TripleBTagCSV_p087_ObjectMatched==1) & (skim.HLT_DoubleJet90_Double30_TripleBTagCSV_p087_Fired==1) ) | ( (skim.HLT_DoubleJet90_Double30_TripleBTagCSV_p087_ObjectMatched==1) & (skim.HLT_DoubleJet90_Double30_TripleBTagCSV_p087_Fired==1)) ]
		  skim = skim.drop(['HLT_DoubleJet90_Double30_TripleBTagCSV_p087_ObjectMatched', 'HLT_DoubleJet90_Double30_TripleBTagCSV_p087_Fired','HLT_DoubleJet90_Double30_TripleBTagCSV_p087_ObjectMatched','HLT_DoubleJet90_Double30_TripleBTagCSV_p087_Fired'], axis=1)	 
	   elif directory == '2017':
		  skim = skim[( (skim.HLT_PFHT300PT30_QuadPFJet_75_60_45_40_TriplePFBTagCSV_3p0_ObjectMatched==1) & (skim.HLT_PFHT300PT30_QuadPFJet_75_60_45_40_TriplePFBTagCSV_3p0_Fired==1) ) | ( (skim.HLT_HT300PT30_QuadJet_75_60_45_40_TripeCSV_p07_ObjectMatched==1) & (skim.HLT_HT300PT30_QuadJet_75_60_45_40_TripeCSV_p07_Fired==1)) ]
		  skim = skim.drop(['HLT_PFHT300PT30_QuadPFJet_75_60_45_40_TriplePFBTagCSV_3p0_ObjectMatched','HLT_PFHT300PT30_QuadPFJet_75_60_45_40_TriplePFBTagCSV_3p0_Fired','HLT_HT300PT30_QuadJet_75_60_45_40_TripeCSV_p07_ObjectMatched','HLT_HT300PT30_QuadJet_75_60_45_40_TripeCSV_p07_Fired'] ,axis=1) 
	   elif directory == '2018':
		  skim = skim[( (skim.HLT_PFHT330PT30_QuadPFJet_75_60_45_40_TriplePFBTagDeepCSV_4p5_ObjectMatched==1) & (skim.HLT_PFHT330PT30_QuadPFJet_75_60_45_40_TriplePFBTagDeepCSV_4p5_Fired==1) )]
		  skim = skim.drop(['HLT_PFHT330PT30_QuadPFJet_75_60_45_40_TriplePFBTagDeepCSV_4p5_ObjectMatched','HLT_PFHT330PT30_QuadPFJet_75_60_45_40_TriplePFBTagDeepCSV_4p5_Fired'],axis=1)
	   else:
		   print "No trigger matching as directory is not 2016/2017/2018"	
	else:
		skim['TrigMatched'] = 0

	#####--------------------------Create new variables
	#Btagging variable Tight studies
	btcut = 0
	if directory=='2016': btcut   = 0.7221
	elif directory=='2017': btcut = 0.7489
	elif directory=='2018': btcut = 0.7264
	else: btcut=0
	b1t   = skim.HH_btag_b1_bscore >=  btcut
	b1not = skim.HH_btag_b1_bscore < btcut
	b2t   = skim.HH_btag_b2_bscore >=  btcut
	b2not = skim.HH_btag_b2_bscore < btcut
	b3t   = skim.HH_btag_b3_bscore >=  btcut
	b3not = skim.HH_btag_b3_bscore < btcut
	skim['HH_btag_b1_bscore_t'] = numpy.select([b1t, b1not], [1, 0], default=0)
	skim['HH_btag_b2_bscore_t'] = numpy.select([b2t, b2not], [1, 0], default=0)
	skim['HH_btag_b3_bscore_t'] = numpy.select([b3t, b3not], [1, 0], default=0)
	skim['nBtagTight'] = skim['HH_btag_b1_bscore_t'] + skim['HH_btag_b2_bscore_t'] + skim['HH_btag_b3_bscore_t']
	
	#Create two new columns with mHH (one for SM(1), other for BSM(2) )
	skim['HH_m_1'] = skim['HH_m']
	skim['HH_m_2'] = skim['HH_m']
	skim['H1_m_v'] = skim['H1_m'] - 54
	skim['H2_m_v'] = skim['H2_m'] - 52

	#Rotation variables:
	theta = math.atan(-float(1/1.04) )
	skim['H1_m_rot'] = skim['H1_m']*math.cos(theta) - (skim['H2_m']*math.sin(theta))
	skim['H2_m_rot'] = skim['H1_m']*math.sin(theta) + (skim['H2_m']*math.cos(theta))

	#Additional variables for the MVAs (abs values, combinations tcs)
	skim['abs_JJ_eta']         = abs(skim['JJ_eta'])
	skim['H1_centrality']      = numpy.exp(-((skim['H1_eta']-((skim['JJ_j1_eta']+skim['JJ_j2_eta'])/2))/skim['j1j2_deltaEta']).pow(2)) 
	skim['H2_centrality']      = numpy.exp(-((skim['H2_eta']-((skim['JJ_j1_eta']+skim['JJ_j2_eta'])/2))/skim['j1j2_deltaEta']).pow(2)) 
	skim['H1H2_centrality']    = skim['H1_centrality']*skim['H2_centrality']
	#Branches to be use to split the data for MVAtraining and background modeling 
	value = numpy.ones(dtype='float64',shape=len(skim))
	skim['Weight_AnaGGF1']      = value
	skim['Weight_AnaGGF2']      = value
	skim['Weight_ValGGF1']      = value
	skim['Weight_ValGGF2']      = value
	skim['Weight_AnaVBF1']      = value
	skim['Weight_AnaVBF2']      = value
	skim['Weight_ValVBF1']      = value
	skim['Weight_ValVBF2']      = value
	skim['Weight_AnaGGF1_Frac'] = value
	skim['Weight_AnaGGF2_Frac'] = value
	#New branches for the new method
	skim['Weight_AnaGGF1_New']  = value
	skim['Weight_AnaGGF2_New']  = value
	skim['Weight_ValGGF1_New']  = value
	skim['Weight_ValGGF2_New']  = value

	#Create the MVAs evaluating half with one weight and half with other weight in signal and data
	skim['Frac1']        = value
	skim['Frac2']        = value
	l = int(len(skim.index) / 2) 
	skim = skim.reset_index(drop=True)  
	skim.loc[:l,"Frac1"] = 1.0
	skim.loc[l:,"Frac1"] = 0.0  
	skim.loc[:l,"Frac2"] = 0.0
	skim.loc[l:,"Frac2"] = 1.0 

	#Create the ggFKiller
	skim['GGFKiller']    = AddGGFKiller(skim,ggfkillerfile)
	skim['GGFMVA1']      = (AddGGFMVA(skim,ggfmva1file1)* skim['Frac1'])  + (AddGGFMVA(skim,ggfmva1file2)* skim['Frac2'])
	skim['GGFMVA2']      = (AddGGFMVA(skim,ggfmva2file1)* skim['Frac1'])  + (AddGGFMVA(skim,ggfmva2file2)* skim['Frac2'])	
	skim['GGFMVA1V']     = (AddGGFMVAV(skim,ggfmva1file1)* skim['Frac1']) + (AddGGFMVAV(skim,ggfmva1file2)* skim['Frac2'])
	skim['GGFMVA2V']     = (AddGGFMVAV(skim,ggfmva2file1)* skim['Frac1']) + (AddGGFMVAV(skim,ggfmva2file2)* skim['Frac2'])
	skim['GGFMVA1btag']  = (AddGGFMVA(skim,ggfmva1file1btag)* skim['Frac1'])  + (AddGGFMVA(skim,ggfmva1file2btag)* skim['Frac2'])
	skim['GGFMVA2btag']  = (AddGGFMVA(skim,ggfmva2file1btag)* skim['Frac1'])  + (AddGGFMVA(skim,ggfmva2file2btag)* skim['Frac2'])

	#Create a default branch for merging signals 2017 and 2018
	if directory=='2018' and 'Data' not in sample:
	   value = numpy.ones(dtype='float32',shape=len(skim))
	   skim['L1PreFiringWeight_Nom']= value
	   skim['L1PreFiringWeight_Up'] = value
	   skim['L1PreFiringWeight_Dn'] = value

	#For systematics (renamings)
	if 'Data' not in sample:
	  skim.rename(columns = {"triggerScaleFactorUp":"triggerScaleFactor_up"},     inplace=True)
	  skim.rename(columns = {"triggerScaleFactorDown":"triggerScaleFactor_down"}, inplace=True)
	return skim

def FindAndAddVariations(file,branches,branchname):
	f = ROOT.TFile.Open(file)
	t = f.bbbbTree
	tlist = t.GetListOfBranches()
	for ibranch in range(tlist.GetEntries()):
		bname  = tlist[ibranch].GetName()
		if branchname in bname: branches.append(bname)
	branches.remove(branchname)
	return branches

def MakeBranchList(sample,filelist,originalbranches,trigmat):
	branches = copy.copy(originalbranches)
	#Correct variables acccording to the type of ntuple (data, signal, etc)
	if "GGHH4B" not in sample and 'HH_reweight' in branches: branches.remove('HH_reweight')
	#Search for LHEScaleWeight in one file
	if "LHEScaleWeight" in branches: branches = FindAndAddVariations(filelist[0],branches,"LHEScaleWeight")
	if "PSWeight"       in branches: branches = FindAndAddVariations(filelist[0],branches,"PSWeight")
	if "LHEPdfWeight"   in branches: branches = FindAndAddVariations(filelist[0],branches,"LHEPdfWeight")
	trigoutput = []
	##Trigger matching objets:
	for branch in branches: 
		if "ObjectMatched" in branch and trigmat==False: trigoutput.append(branch)
	for out in trigoutput: branches.remove(out)
	return branches

def MakeOutputSkims(eos,eosdirectory,samples,branches,case,directory,lepveto,trigmat,ggfkillerfile,ggfmva1file1,ggfmva2file1,ggfmva1file2,ggfmva2file2,ggfmva1file1btag,ggfmva2file1btag,ggfmva1file2btag,ggfmva2file2btag):
	
	#Create directory
	if eos==True:
		os.system('mkdir %s/outputskims'%eosdirectory)
		os.system('mkdir %s/outputskims/%s/'%(eosdirectory,case)  )
		os.system('mkdir %s/outputskims/%s/%s'%(eosdirectory,case,directory))
	else:
		os.system('mkdir outputskims')
		os.system('mkdir outputskims/%s/'%case)
		os.system('mkdir outputskims/%s/%s'%(case,directory))
	for sample in samples:
		#Create a list of files and dataframe (but edit eos address accordingly)
		print "[INFO] Sample in process: %s"%sample
		#Here point the skim towards the right eos directory
		filelist   = data.GetFileList('%s/%s/%s/SKIM_%s/output/*.root'%(eosdirectory,case,directory,sample),'bbbbTree')
		branchlist = MakeBranchList(sample,filelist,branches,trigmat)
		skim       = TreeDevelopment(sample,filelist,branchlist,case,directory,lepveto,trigmat,ggfkillerfile,ggfmva1file1,ggfmva2file1,ggfmva1file2,ggfmva2file2,ggfmva1file1btag,ggfmva2file1btag,ggfmva1file2btag,ggfmva2file2btag) 
		if eos==True:
		  #Save it in a root file (bbbbtree)
		  data.pandas2root(skim,'bbbbTree','%s/outputskims/%s/%s/SKIM_%s_tree.root'%(eosdirectory,case,directory,sample)  )
		  #Save it in a root file (eff_histo) 
		  data.roothist2root(filelist,'eff_histo','%s/outputskims/%s/%s/SKIM_%s_hist.root'%(eosdirectory,case,directory,sample))
		  #Merge tree and efficiency histos in root file and save it to eos
		  data.mergedata2root('%s/outputskims/%s/%s/SKIM_%s_tree.root'%(eosdirectory,case,directory,sample), '%s/outputskims/%s/%s/SKIM_%s_hist.root'%(eosdirectory,case,directory,sample), '%s/outputskims/%s/%s/SKIM_%s.root'%(eosdirectory,case,directory,sample))
		else:
		  #Save it in a root file (bbbbtree)
		  data.pandas2root(skim,'bbbbTree','outputskims/%s/%s/SKIM_%s_tree.root'%(case,directory,sample)  )
		  #Save it in a root file (eff_histo) 
		  data.roothist2root(filelist,'eff_histo','outputskims/%s/%s/SKIM_%s_hist.root'%(case,directory,sample))
		  #Merge tree and efficiency histos in root file and save it to eos
		  data.mergedata2root('outputskims/%s/%s/SKIM_%s_tree.root'%(case,directory,sample), 'outputskims/%s/%s/SKIM_%s_hist.root'%(case,directory,sample), 'outputskims/%s/%s/SKIM_%s.root'%(case,directory,sample))			
		print '[INFO] Saving output skim for',sample,' in ',case
		del skim 
#############COMMAND CODE IS BELOW ######################

###########OPTIONS
parser = argparse.ArgumentParser(description='Command line parser of skim options')
parser.add_argument('--config' ,  dest='cfgfile',  help='Name of config file',  required = True)
parser.add_argument('--casename',    dest='case',  help='Name of ntuples case', required = True)
parser.add_argument('--mcbkgs',    dest='mcbkgs', action='store_true', help='Skim MC background samples')
parser.add_argument('--no-mcbkgs', dest='mcbkgs', action='store_false',help='Do not skim MC background samples')
parser.add_argument('--lepveto',    dest='lepveto', action='store_true', help='Apply Lepton veto')
parser.add_argument('--no-lepveto', dest='lepveto', action='store_false',help='Do not apply Lepton veto')
parser.add_argument('--trigmat',    dest='trigmat', action='store_true', help='Apply Trigger Matching')
parser.add_argument('--no-trigmat', dest='trigmat', action='store_false',help='Do not apply Trigger Matching')
parser.add_argument('--systs',    dest='systs', action='store_true', help='Do signal systematic variations')
parser.add_argument('--no-systs', dest='systs', action='store_false',help='Dont do signal systematic variations')
parser.add_argument('--data',    dest='data', action='store_true', help='Do data')
parser.add_argument('--no-data', dest='data', action='store_false',help='Dont do data')
parser.add_argument('--signal',    dest='signal', action='store_true', help='Do signal')
parser.add_argument('--no-signal', dest='signal', action='store_false',help='Dont do signal')
parser.add_argument('--othersignal',    dest='othersignal', action='store_true', help='Do other signal (LO ggf)')
parser.add_argument('--no-othersignal', dest='othersignal', action='store_false',help='Dont do other signal (LO ggf)')
parser.add_argument('--eos',    dest='eos', action='store_true', help='Save skims on eos')
parser.add_argument('--no-eos', dest='eos', action='store_false',help='Do not save skims on eos')
#Set defaults
parser.set_defaults(lepveto=False)
parser.set_defaults(trigmat=False)
parser.set_defaults(systs=False)
parser.set_defaults(mcbkgs=False)
parser.set_defaults(data=False)
parser.set_defaults(signal=False)
parser.set_defaults(othersignal=False)
parser.set_defaults(eos=False)
args = parser.parse_args()
configfilename = args.cfgfile
casename       = args.case
lepveto        = args.lepveto
trigmat        = args.trigmat
systs          = args.systs
mcbkgs         = args.mcbkgs
datas          = args.data
othersignal    = args.othersignal
signal         = args.signal
eos            = args.eos
systsamples    = []
###########Read Config file
print "[INFO] Reading skim configuration file . . ."
cfgparser = ConfigParser()
cfgparser.read('%s'%configfilename)
##########Get skim variables
print "[INFO] Getting configuration parameters . . ."
eosdirectory   = ast.literal_eval(cfgparser.get("configuration","eosdirectory"))
print "    -The eos directory:"
print "      *",eosdirectory
directory   = ast.literal_eval(cfgparser.get("configuration","directory"))
print "    -The directory:"
print "      *",directory
sigsamples     = ast.literal_eval(cfgparser.get("configuration","sigsamples"))
print "    -The list of MC signal samples:"
for x in range(len(sigsamples)):
  print "      *",sigsamples[x]
othersigsamples= ast.literal_eval(cfgparser.get("configuration","othersigsamples"))
print "    -The list of other MC signal samples:"
for x in range(len(othersigsamples)):
  print "      *",othersigsamples[x]
bkgsamples     = ast.literal_eval(cfgparser.get("configuration","bkgsamples"))
print "    -The list of MC background samples:"
for x in range(len(bkgsamples)):
  print "      *",bkgsamples[x] 
datsamples     = ast.literal_eval(cfgparser.get("configuration","datsamples"))
print "    -The list of data samples:"
for x in range(len(datsamples)):
  print "      *",datsamples[x]    
variables   = ast.literal_eval(cfgparser.get("configuration","variables"))
print "    -The list of variables:"
for x in range(len(variables)):
  print "      *",variables[x]
weights     = ast.literal_eval(cfgparser.get("configuration","weights"))
print "    -The list of common MC weights:"
for x in range(len(weights)):
  print "      *",weights[x]
dedicatedweights  = ast.literal_eval(cfgparser.get("configuration","dedicatedweights"))
print "    -The list of dedicated MC weights:"
for x in range(len(dedicatedweights)):
  print "      *",dedicatedweights[x]
sigsystematics   = ast.literal_eval(cfgparser.get("configuration","sigsystematics"))
print "     -The list of systematic variations:"  
for x in range(len(sigsystematics)):
  print "      *",sigsystematics[x]
print "    -The case: ",casename
ggfkillerfile      = ast.literal_eval(cfgparser.get("configuration","GGFKillerfile"))
print "    -The ggfkiller file: ",ggfkillerfile
ggfmva1file1        = ast.literal_eval(cfgparser.get("configuration","GGFMVA1file1"))
print "    -The ggfmva1 file1:   ",ggfmva1file1
ggfmva2file1        = ast.literal_eval(cfgparser.get("configuration","GGFMVA2file1"))
print "    -The ggfmva2 file1:   ",ggfmva2file1
ggfmva1file2        = ast.literal_eval(cfgparser.get("configuration","GGFMVA1file2"))
print "    -The ggfmva1 file1:   ",ggfmva1file2
ggfmva2file2        = ast.literal_eval(cfgparser.get("configuration","GGFMVA2file2"))
print "    -The ggfmva2 file1:   ",ggfmva2file2

ggfmva1file1btag        = ast.literal_eval(cfgparser.get("configuration","GGFMVA1file1btag"))
print "    -The ggfmva1 file1:   ",ggfmva1file1btag
ggfmva2file1btag        = ast.literal_eval(cfgparser.get("configuration","GGFMVA2file1btag"))
print "    -The ggfmva2 file1:   ",ggfmva2file1btag
ggfmva1file2btag        = ast.literal_eval(cfgparser.get("configuration","GGFMVA1file2btag"))
print "    -The ggfmva1 file1:   ",ggfmva1file2btag
ggfmva2file2btag        = ast.literal_eval(cfgparser.get("configuration","GGFMVA2file2btag"))
print "    -The ggfmva2 file1:   ",ggfmva2file2btag

print "[INFO] The outputskims will be saved in eos: ",eos
print "[INFO] MC background will be run: ",mcbkgs
print "[INFO] The lepton veto is applied: ",lepveto
print "[INFO] The trigger matching is applied: ",trigmat
print "[INFO] The systematics will be run on signal samples: ",systs

##########Make microskims
starttime = time.time()

#print "[INFO] Making mc nominal and extra signal skims . . . "
if signal==True: MakeOutputSkims(eos,eosdirectory,sigsamples,variables+weights+dedicatedweights,casename,directory,lepveto,trigmat,ggfkillerfile,ggfmva1file1,ggfmva2file1,ggfmva1file2,ggfmva2file2,ggfmva1file1btag,ggfmva2file1btag,ggfmva1file2btag,ggfmva2file2btag)
if othersignal==True: MakeOutputSkims(eos,eosdirectory,othersigsamples,variables+weights,casename,directory,lepveto,trigmat,ggfkillerfile,ggfmva1file1,ggfmva2file1,ggfmva1file2,ggfmva2file2,ggfmva1file1btag,ggfmva2file1btag,ggfmva1file2btag,ggfmva2file2btag)
if systs==True:
	print "[INFO] Making mc systematic signal variations skims using multiprocessing "
	for sigsample in sigsamples:
		for sigsyst in sigsystematics:
			systsamples.append("%s_%s_up"%(sigsample,sigsyst))
			systsamples.append("%s_%s_down"%(sigsample,sigsyst))	
	#if no cores available: MakeOutputSkims(eos,eosdirectory,systsamples,variables+weights,casename,directory,lepveto,ggfkillerfile,ggfmva1file1,ggfmva2file1,ggfmva1file2,ggfmva2file2,ggfmva1file1btag,ggfmva2file1btag,ggfmva1file2btag,ggfmva2file2btag)
	#Create pool of processes		
	processes = []
	threadNumber=6
	listNumber = 0
	skimFileListOfList = [ [] for i in range(threadNumber)]
	for fileName in systsamples:
			skimFileListOfList[listNumber].append(fileName)
			listNumber = listNumber + 1
			if listNumber >= 6: 
				listNumber = 0
	for i in range(threadNumber):
		p = mp.Process(target=MakeOutputSkims, args=(eos,eosdirectory,skimFileListOfList[i],variables+weights,casename,directory,lepveto,trigmat,ggfkillerfile,ggfmva1file1,ggfmva2file1,ggfmva1file2,ggfmva2file2,ggfmva1file1btag,ggfmva2file1btag,ggfmva1file2btag,ggfmva2file2btag))
		processes.append(p)
		p.start()
	for process in processes:
		process.join()
if mcbkgs==True:
	print "[INFO] Making mc background skims . . . " 
	MakeOutputSkims(eos,eosdirectory,bkgsamples,variables+weights,casename,directory,lepveto,trigmat,ggfkillerfile,ggfmva1file1,ggfmva2file1,ggfmva1file2,ggfmva2file2,ggfmva1file1btag,ggfmva2file1btag,ggfmva1file2btag,ggfmva2file2btag)
if datas==True:
	print "[INFO] Making data skims . . . "
	MakeOutputSkims(eos,eosdirectory,datsamples,variables,casename,directory,lepveto,trigmat,ggfkillerfile,ggfmva1file1,ggfmva2file1,ggfmva1file2,ggfmva2file2,ggfmva1file1btag,ggfmva2file1btag,ggfmva1file2btag,ggfmva2file2btag)

print('Running skims took {} seconds'.format(time.time() - starttime))
