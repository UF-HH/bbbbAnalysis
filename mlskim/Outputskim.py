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

def TreeDevelopment(sample,filelist,columns_input,case,directory,lepveto,ggfkillerfile,ggfmva1file,ggfmva2file):
	#Make initial skim using defined branches
	skim   = data.root2pandas(filelist,'bbbbTree', branches=columns_input)
	#Veto on leptons
	if lepveto==True: skim = skim[(skim.IsolatedElectron_Multiplicity==0) & (skim.IsolatedMuon_Multiplicity==0)]	 
	#Create two new columns with mHH (one for SM(1), other for BSM(2) )
	skim['HH_m_1'] = skim['HH_m']
	skim['HH_m_2'] = skim['HH_m']
	#Rotation variables:
	theta = math.atan(-float(1/1.05) )
	skim['H1_m_rot'] = skim['H1_m']*math.cos(theta) - (skim['H2_m']*math.sin(theta))
	skim['H2_m_rot'] = skim['H1_m']*math.sin(theta) + (skim['H2_m']*math.cos(theta))
	#Additional variables for the MVAs (abs values, combinations tcs)
	skim['abs_JJ_eta']         = abs(skim['JJ_eta'])
	skim['H1_centrality']      = numpy.exp(-((skim['H1_eta']-((skim['JJ_j1_eta']+skim['JJ_j2_eta'])/2))/skim['j1j2_deltaEta']).pow(2)) 
	skim['H2_centrality']      = numpy.exp(-((skim['H2_eta']-((skim['JJ_j1_eta']+skim['JJ_j2_eta'])/2))/skim['j1j2_deltaEta']).pow(2)) 
	skim['H1H2_centrality']    = skim['H1_centrality']*skim['H2_centrality']
	#Create the ggFKiller and ggfMVA scores
	skim['GGFKiller']          = AddGGFKiller(skim,ggfkillerfile)
	skim['GGFMVA1']            = AddGGFMVA(skim,ggfmva1file)
	skim['GGFMVA2']            = AddGGFMVA(skim,ggfmva2file)
	#Branches to be use to split the data for MVAtraining and background modeling 
	value = numpy.ones(dtype='float64',shape=len(skim))
	skim['Weight_AnaGGF1']     = value
	skim['Weight_AnaGGF2']     = value
	skim['Weight_ValGGF1']     = value
	skim['Weight_ValGGF2']     = value
	skim['Weight_AnaVBF1']     = value
	skim['Weight_AnaVBF2']     = value
	skim['Weight_ValVBF1']     = value
	skim['Weight_ValVBF2']     = value
	skim['Weight_AnaGGF1_MVA'] = value
	skim['Weight_AnaGGF2_MVA'] = value

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

def MakeBranchList(sample,filelist,originalbranches):
	branches = copy.copy(originalbranches)
	#Correct variables acccording to the type of ntuple (data, signal, etc)
	if "GGHH4B" not in sample and 'HH_reweight' in branches: branches.remove('HH_reweight')
	#Search for LHEScaleWeight in one file
	if "LHEScaleWeight" in branches: branches = FindAndAddVariations(filelist[0],branches,"LHEScaleWeight")
	if "PSWeight"       in branches: branches = FindAndAddVariations(filelist[0],branches,"PSWeight")
	if "LHEPdfWeight"   in branches: branches = FindAndAddVariations(filelist[0],branches,"LHEPdfWeight")
	return branches

def MakeOutputSkims(samples,branches,case,directory,lepveto,ggfkillerfile,ggfmva1file,ggfmva2file):
	os.system('mkdir outputskims')
	os.system('mkdir outputskims/%s/'%case)
	os.system('mkdir outputskims/%s/%s'%(case,directory))
	for sample in samples:
		#Create a list of files and dataframe (but edit eos address accordingly)
		print "[INFO] Sample in process: %s"%sample
		filelist   = data.GetFileList('/eos/uscms/store/user/guerrero/bbbb_ntuples/%s/%s/SKIM_%s/output/*.root'%(case,directory,sample),'bbbbTree')
		branchlist = MakeBranchList(sample,filelist,branches)
		skim       = TreeDevelopment(sample,filelist,branchlist,case,directory,lepveto,ggfkillerfile,ggfmva1file,ggfmva2file) 
		#Save it in a root file (bbbbtree)
		data.pandas2root(skim,'bbbbTree','outputskims/%s/%s/SKIM_%s_tree.root'%(case,directory,sample)  )
		#Save it in a root file (eff_histo) 
		data.roothist2root(filelist,'eff_histo','outputskims/%s/%s/SKIM_%s_hist.root'%(case,directory,sample))
		#Merge tree and efficiency histos in root file
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
parser.add_argument('--systs',    dest='systs', action='store_true', help='Do signal systematic variations')
parser.add_argument('--no-systs', dest='systs', action='store_false',help='Dont do signal systematic variations')
parser.add_argument('--data',    dest='data', action='store_true', help='Do data')
parser.add_argument('--no-data', dest='data', action='store_false',help='Dont do data')
parser.add_argument('--signal',    dest='signal', action='store_true', help='Do signal')
parser.add_argument('--no-signal', dest='signal', action='store_false',help='Dont do signal')
parser.add_argument('--othersignal',    dest='othersignal', action='store_true', help='Do other signal (LO ggf)')
parser.add_argument('--no-othersignal', dest='othersignal', action='store_false',help='Dont do other signal (LO ggf)')
parser.set_defaults(lepveto=False)
parser.set_defaults(systs=False)
parser.set_defaults(mcbkgs=False)
parser.set_defaults(data=False)
parser.set_defaults(signal=False)
parser.set_defaults(othersignal=False)
args = parser.parse_args()
configfilename = args.cfgfile
casename       = args.case
lepveto        = args.lepveto
systs          = args.systs
mcbkgs         = args.mcbkgs
datas          = args.data
othersignal    = args.othersignal
signal         = args.signal
systsamples    = []
###########Read Config file
print "[INFO] Reading skim configuration file . . ."
cfgparser = ConfigParser()
cfgparser.read('%s'%configfilename)
##########Get skim variables
print "[INFO] Getting configuration parameters . . ."
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
ggfmva1file        = ast.literal_eval(cfgparser.get("configuration","GGFMVA1file"))
print "    -The ggfmva1 file:   ",ggfmva1file
ggfmva2file        = ast.literal_eval(cfgparser.get("configuration","GGFMVA2file"))
print "    -The ggfmva2 file:   ",ggfmva2file
print "[INFO] MC background will be run: ",mcbkgs
print "[INFO] The lepton veto is applied: ",lepveto
print "[INFO] The systematics will be run on signal samples: ",systs

##########Make microskims
starttime = time.time()

#print "[INFO] Making mc nominal and extra signal skims . . . "
if signal==True: MakeOutputSkims(sigsamples,variables+weights+dedicatedweights,casename,directory,lepveto,ggfkillerfile,ggfmva1file,ggfmva2file)
if othersignal==True: MakeOutputSkims(othersigsamples,variables+weights,casename,directory,lepveto,ggfkillerfile,ggfmva1file,ggfmva2file)
if systs==True:
	print "[INFO] Making mc systematic signal variations skims using multiprocessing "
	for sigsample in sigsamples:
		for sigsyst in sigsystematics:
			systsamples.append("%s_%s_up"%(sigsample,sigsyst))
			systsamples.append("%s_%s_down"%(sigsample,sigsyst))	
	#if no cores available: MakeOutputSkims(systsamples,variables+weights,casename,directory,lepveto,ggfkillerfile,ggfmva1file,ggfmva2file)
	#Create pool of processes		
	processes = []
	threadNumber=4
	listNumber = 0
	skimFileListOfList = [ [] for i in range(threadNumber)]
	for fileName in systsamples:
			skimFileListOfList[listNumber].append(fileName)
			listNumber = listNumber + 1
			if listNumber >= 4: 
				listNumber = 0
	for i in range(threadNumber):
		p = mp.Process(target=MakeOutputSkims, args=(skimFileListOfList[i],variables+weights,casename,directory,lepveto,ggfkillerfile,ggfmva1file,ggfmva2file))
		processes.append(p)
		p.start()
	for process in processes:
		process.join()
if mcbkgs==True:
	print "[INFO] Making mc background skims . . . " 
	MakeOutputSkims(bkgsamples,variables+weights,casename,directory,lepveto,ggfkillerfile,ggfmva1file,ggfmva2file)
if datas==True:
    print "[INFO] Making data skims . . . "
    MakeOutputSkims(datsamples,variables,casename,directory,lepveto,ggfkillerfile,ggfmva1file,ggfmva2file)

print('Running skims took {} seconds'.format(time.time() - starttime))
