import os
import glob
import ast
import argparse
import sys
import copy
import ROOT
import numpy as np
from  ConfigParser import *

def RunOutputVariations(couplingsfile,dataset,directory,samples,histograms,tag):
	#Get root file
	file = ROOT.TFile.Open("../../%s/outPlotter.root"%directory)
	file_extra = ROOT.TFile.Open("../../Histos2018/outPlotter.root")

	os.system('mkdir %s'%tag)
	#Open output file
	hfile = ROOT.TFile('%s/outPlotter.root'%tag, 'RECREATE')
	for histo in histograms:
		print histo
		#Get histograms
		hsig1=file.Get("%s/Btag4_VBFcateg_SR_110_Histogram/%s_Btag4_VBFcateg_SR_110_Histogram_%s"%(samples[0],samples[0],histo) ) 
		hsig2=file.Get("%s/Btag4_VBFcateg_SR_110_Histogram/%s_Btag4_VBFcateg_SR_110_Histogram_%s"%(samples[1],samples[1],histo) ) 
		hsig3=file.Get("%s/Btag4_VBFcateg_SR_110_Histogram/%s_Btag4_VBFcateg_SR_110_Histogram_%s"%(samples[2],samples[2],histo) ) 
		hsig4=file.Get("%s/Btag4_VBFcateg_SR_110_Histogram/%s_Btag4_VBFcateg_SR_110_Histogram_%s"%(samples[3],samples[3],histo) ) 
		hsig6=file.Get("%s/Btag4_VBFcateg_SR_110_Histogram/%s_Btag4_VBFcateg_SR_110_Histogram_%s"%(samples[5],samples[5],histo) ) 
        htst =file.Get("%s/Btag4_VBFcateg_SR_110_Histogram/%s_Btag4_VBFcateg_SR_110_Histogram_%s"%(samples[0],samples[0],histo) )		
		hbkg =file.Get("MODEL/Btag4_VBFcateg_SR_110_Histogram/MODEL_Btag4_VBFcateg_SR_110_Histogram_%s"%histo)
		hdata=file.Get("DATA/Btag4_VBFcateg_SR_110_Histogram/DATA_Btag4_VBFcateg_SR_110_Histogram_%s"%histo)	
		if dataset == '2017': 
			hsig5     =file_extra.Get("%s/Btag4_VBFcateg_SR_110_Histogram/%s_Btag4_VBFcateg_SR_110_Histogram_%s"%(samples[4],samples[4],histo) ) 
			hsig1_2018=file_extra.Get("%s/Btag4_VBFcateg_SR_110_Histogram/%s_Btag4_VBFcateg_SR_110_Histogram_%s"%(samples[0],samples[0],histo) )
			hsig6_2018=file_extra.Get("%s/Btag4_VBFcateg_SR_110_Histogram/%s_Btag4_VBFcateg_SR_110_Histogram_%s"%(samples[5],samples[5],histo) )
			pointsX=[ 1, 1.5 ]
			pointsY=[ float(hsig1.Integral()/hsig1_2018.Integral()) , float(hsig6.Integral()/hsig6_2018.Integral()) ]
			lincoeff = np.polyfit(pointsX, pointsY, 1)
			print "      -[Info] 2018->2017 Scale factor %f to model missing sample in 2017"%(lincoeff[0]*0.5+lincoeff[1])
			hsig5.Scale(lincoeff[0]*0.5+lincoeff[1])
		else:
			hsig5=file.Get("%s/Btag4_VBFcateg_SR_110_Histogram/%s_Btag4_VBFcateg_SR_110_Histogram_%s"%(samples[4],samples[4],histo) )
		histos = [hsig1,hsig2,hsig3,hsig4,hsig5,hsig6]
		#Get couplings file
		couplingstoscan = []
		with open("config/%s"%couplingsfile) as fobj:
			for line in fobj:
				row = line.split()
				couplingstoscan.append(row)
		#Write data and bkg:
		htst.Write()
		hbkg.Write()
		hdata.Write()	
		#Make variations defined in couplings files
		MakeOutputVariations(couplingstoscan,histos,histo,tag)
	hfile.Close()

def MakeOutputVariations(couplingstoscan,histos,histo,tag):

	#Make variations and write them in the outputfile
	for j in range(0, len(couplingstoscan)) :
		 ###### target values
		 info = couplingstoscan[j]
		 couplings   = [info[1],info[2],info[3]]
		 factors     = [float(info[4]) ,float(info[5]),float(info[6]),float(info[7]),float(info[8]),float(info[9])]
		 #Getshapes
		 hsig1 = histos[0].Clone()
		 hsig2 = histos[1].Clone()
		 hsig3 = histos[2].Clone()
		 hsig4 = histos[3].Clone()
		 hsig5 = histos[4].Clone()
		 hsig6 = histos[5].Clone()
		 #Scale them according to the factors
		 if factors[0] != 0: hsig1.Scale(factors[0]) 
		 if factors[1] != 0: hsig2.Scale(factors[1])
		 if factors[2] != 0: hsig3.Scale(factors[2])
		 if factors[3] != 0: hsig4.Scale(factors[3])
		 if factors[4] != 0: hsig5.Scale(factors[4])
		 if factors[5] != 0: hsig6.Scale(factors[5])
		 #Create tagname for c2v variation coupling
		 if float(couplings[1]) >= 0:
			couplingname="p_%.1f"%abs(float(couplings[1]) )
		 else:
			couplingname="m_%.1f"%abs(float(couplings[1]))
		 histoname = "VBFHH4B_rew_c2v_%s_Btag4_VBFcateg_SR_110_Histogram_%s"%(couplingname,histo)
		 print "      * (cv,c2v,c3) = (%s,%s,%s)"%(couplings[0],couplings[1],couplings[2])
		 #Create template
		 hsig  = ROOT.TH1F()
		 hsig.SetName(histoname)
		 hsig.SetTitle(histoname)
		 hsig.SetBins(histos[0].GetNbinsX(),histos[0].GetBinLowEdge(1),histos[0].GetBinLowEdge(histos[0].GetNbinsX()+1))
		 if factors[0] !=0: hsig.Add(hsig1)
		 if factors[1] !=0: hsig.Add(hsig2)
		 if factors[2] !=0: hsig.Add(hsig3)
		 if factors[3] !=0: hsig.Add(hsig4)
		 if factors[4] !=0: hsig.Add(hsig5)
		 if factors[5] !=0: hsig.Add(hsig6)
		 hsig.Write()
		 del hsig1,hsig2,hsig3,hsig4,hsig5,hsig6
		
#############COMMAND CODE IS BELOW ######################

###########OPTIONS
parser = argparse.ArgumentParser(description='Command line parser of skim options')
parser.add_argument('--config', dest='cfgfile',  help='Name of config file with MC information',   required = True)
args = parser.parse_args()
configfilename = args.cfgfile
###########Read Config file
print "[INFO] Reading configuration file . . ."
cfgparser = ConfigParser()
cfgparser.read('%s'%configfilename)
##########Get skim variables
print "[INFO] Getting configuration parameters . . ."
couplingsfile  = ast.literal_eval(cfgparser.get("configuration","couplingsfile"))
print "    -The couplings file:"
print "      *",couplingsfile  
dataset     = ast.literal_eval(cfgparser.get("configuration","dataset"))
print "    -The MC dataset:"
print "      *",dataset
directory   = ast.literal_eval(cfgparser.get("configuration","directory"))
print "    -The location of the input histograms:"
print "      *",directory
samples     = ast.literal_eval(cfgparser.get("configuration","samples"))
print "    -The list of samples:"
for x in range(len(samples)):
  print "      *",samples[x]
histograms  = ast.literal_eval(cfgparser.get("configuration","histograms"))
print "    -The list of variables:"
for x in range(len(histograms)):
  print "      *",histograms[x]
tag         = ast.literal_eval(cfgparser.get("configuration","tag"))
print "    -The name of the folder to save the results:"
print "      *",tag
##########Make microskims
print "[INFO] Making the histograms of coupling variations. . ."
RunOutputVariations(couplingsfile,dataset,directory,samples,histograms,tag)