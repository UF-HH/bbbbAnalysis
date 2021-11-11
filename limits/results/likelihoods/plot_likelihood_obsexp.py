import re, optparse
import os.path,sys
import argparse
from math import *
# from ROOT import *
import ROOT
ROOT.gROOT.SetBatch(True)

#####
def redrawBorder():
   ROOT.gPad.Update()
   ROOT.gPad.RedrawAxis()
   l=ROOT.TLine()
   l.SetLineWidth(5)
#   l.DrawLine(ROOT.gPad.GetUxmin(), ROOT.gPad.GetUymax(), ROOT.gPad.GetUxmax(), ROOT.gPad.GetUymax());
#   l.DrawLine(ROOT.gPad.GetUxmin(), ROOT.gPad.GetUymin(), ROOT.gPad.GetUxmax(), ROOT.gPad.GetUymin());
#   l.DrawLine(ROOT.gPad.GetUxmin(), ROOT.gPad.GetUymin(), ROOT.gPad.GetUxmin(), ROOT.gPad.GetUymax());
#   l.DrawLine(ROOT.gPad.GetUxmax(), ROOT.gPad.GetUymin(), ROOT.gPad.GetUxmax(), ROOT.gPad.GetUymax());
   l.DrawLine(ROOT.gPad.GetUxmin(), ROOT.gPad.GetUymax(), ROOT.gPad.GetUxmax(), ROOT.gPad.GetUymax());
   l.DrawLine(ROOT.gPad.GetUxmax(), ROOT.gPad.GetUymin(), ROOT.gPad.GetUxmax(), ROOT.gPad.GetUymax());
   l.DrawLine(ROOT.gPad.GetUxmin(), ROOT.gPad.GetUymin(), ROOT.gPad.GetUxmax(), ROOT.gPad.GetUymin());
   l.DrawLine(ROOT.gPad.GetUxmin(), ROOT.gPad.GetUymin(), ROOT.gPad.GetUxmin(), ROOT.gPad.GetUymax());

def getVals(fname,coupling):
	fIn = ROOT.TFile.Open(fname)
	tIn = fIn.Get('limit')
	vals = []
	for i in range(1, tIn.GetEntries()):
		tIn.GetEntry(i)
		if coupling=='C2V': 
			x = tIn.C2V
		elif coupling=='CV':
			x = tIn.CV
		else:
			x = tIn.kl
		y  = 2*(tIn.deltaNLL)
		if y<0: y=0
		#print x,y
		vals.append((x,y))
	return vals

################################################################################################
###########OPTIONS
parser = argparse.ArgumentParser(description='Command line parser of skim options')
parser.add_argument('--datasetid', dest='datasetid',  help='Dataset:  (1) 2016, (2) 2017, (3) 2018, (4) 20172018, (5) Run2', required = True)
parser.add_argument('--categid',   dest='categid',    help='Category: (1) GGF/VBFcateg1, (2) GGF/VBFcateg2, (3) GGF/VBF-HH-Combination and (4) Full Combination', required = True)
parser.add_argument('--coupling', dest='coupling',help='Coupling: (1) KL, (2) C2V, (3) CV', required = True)

args = parser.parse_args()
###########
###CREATE TAGS
dataid     = args.datasetid
categid    = args.categid
coupling   = args.coupling

##Couplings
if coupling=='C2V' or coupling=='CV' or coupling=='KL':
  print "Plotting likelihood scan for %s coupling"%coupling	
  if coupling=='C2V': 
  	ranges = [-2.5,4.5]
  	xaxistitle  = '#kappa_{2V}'
  elif coupling=='KL':
  	ranges = [-15.0,20.0]
  	xaxistitle  = '#kappa_{#lambda}'
  else:
    ranges = [-3.0,3.0]
    xaxistitle  = '#kappa_{V}'		
else:
  print "Coupling is not specfied correctly! No plot is done"		
  sys.exit()

##Dataset properties
if dataid == '1':
  folder   = '%s_likelihood_2016'%coupling
  outname  = '%s_likelihood_1d_2016'%coupling
  datalumi = "2016, 35.9 fb^{-1} (13 TeV)"
elif dataid == '2':
  folder   = '%s_likelihood_2017'%coupling
  outname  = '%s_likelihood_1d_2017'%coupling
  datalumi = "2017, 41.5 fb^{-1} (13 TeV)"
elif dataid == '3':
  folder   = '%s_likelihood_2018'%coupling
  outname  = '%s_likelihood_1d_2018'%coupling
  datalumi = "2018, 59.7 fb^{-1} (13 TeV)"
elif dataid == '4':
  folder   = '%s_likelihood_20172018'%coupling
  outname  = '%s_likelihood_1d_20172018'%coupling
  datalumi = "20172018, 101.3 fb^{-1} (13 TeV)"
elif dataid == '5':
  folder   = '%s_likelihood_run2'%coupling
  outname  = '%s_likelihood_1d_run2'%coupling
  datalumi = "       138 fb^{-1} (13 TeV)"
else:
  print "Dataset is not specified correctly! No plot is done"	
  sys.exit()

##Categories properties
if categid == '1':
  categlat = 'GGF-HH Cat-1'
  outname += '_ggfcat1'
  mainfolder = '_ggfcat1'
elif categid == '2':
  categlat = 'GGF-HH Cat-2'
  outname += '_ggfcat2'
  mainfolder = '_ggfcat2'
elif categid == '3':
  categlat = 'VBF-HH Cat-1'
  outname += '_vbfcat1'
  mainfolder = '_vbfcat1'
elif categid == '4':
  categlat = 'VBF-HH Cat-2'
  outname += '_vbfcat2'
  mainfolder = 'vbfcat2'
elif categid == '5':
  categlat = 'ggF-HH Categories'
  outname += '_ggfcats'
  mainfolder = 'ggfcats'
elif categid == '6':
  categlat = 'VBF-HH Categories'
  outname += '_vbfcats'
  mainfolder = 'vbfcats'
elif categid == '7':
  categlat = 'All categories'
  outname += '_allcategories'
  mainfolder = 'allcategories'
else:
  print "Category is not specfied correctly! No plot is done"	
  sys.exit()

#Plot it
c1 = ROOT.TCanvas("c1", "c1", 1300, 1100)
c1.SetFrameLineWidth(3)
c1.SetBottomMargin (0.15)
c1.SetRightMargin (0.08)
c1.SetLeftMargin (0.15)

#Read file and values for observed
grobs = ROOT.TGraph()
fname = 'likelihood_'+ mainfolder + '/' + folder +'/higgsCombineTest.MultiDimFit.mH120.root'
vals  = getVals(fname,coupling)
for ipt, pt in enumerate(vals):
  xval = pt[0]
  exp  = pt[1]
  grobs.SetPoint(ipt, xval, exp)
######## set styles
grobs.SetMarkerStyle(8)
grobs.SetMarkerColor(4)
grobs.SetMarkerSize(2)
grobs.SetLineColor(ROOT.kBlack)
grobs.SetLineWidth(5)
grobs.SetLineStyle(1)
grobs.SetFillColor(0)

#Read file and values for expected
grexp = ROOT.TGraph()
fname = 'likelihood_'+mainfolder+'_expected/'+ folder + '/higgsCombineTest.MultiDimFit.mH120.root'
vals  = getVals(fname,coupling)
for ipt, pt in enumerate(vals):
	xval = pt[0]
	exp  = pt[1]
	grexp.SetPoint(ipt, xval, exp)
######## set styles
grexp.SetMarkerStyle(8)
grexp.SetMarkerColor(4)
grexp.SetMarkerSize(2)
grexp.SetLineColor(ROOT.kAzure+1)
grexp.SetLineWidth(5)
grexp.SetLineStyle(1)
grexp.SetFillColor(0)

##lINES
line1 = ROOT.TLine(ranges[0],1,ranges[1],1)
line1.SetLineColor(15)
line1.SetLineWidth(2)
line1.SetLineStyle(2)
line2 = ROOT.TLine(ranges[0],3.84,ranges[1],3.84)
line2.SetLineColor(15)
line2.SetLineWidth(2)
line2.SetLineStyle(2)
##### Text
pt1 = ROOT.TPaveText(0.1663218-0.02,0.886316,0.3045977-0.02,0.978947,"brNDC")
pt1.SetBorderSize(0)
pt1.SetTextAlign(12)
pt1.SetTextFont(62)
pt1.SetTextSize(0.05)
pt1.SetFillColor(0)
pt1.SetFillStyle(0)
pt1.AddText("CMS #font[52]{Preliminary}" )
pt2 = ROOT.TPaveText(0.60,0.9066667,0.8997773,0.957037,"brNDC")
pt2.SetBorderSize(0)
pt2.SetFillColor(0)
pt2.SetTextSize(0.040)
pt2.SetTextFont(42)
pt2.SetFillStyle(0)
pt2.AddText(datalumi)

pt4 = ROOT.TPaveText(0.18,0.78,0.45,0.897,"brNDC")
pt4.SetTextAlign(12)
pt4.SetFillColor(ROOT.kWhite)
pt4.SetFillStyle(1001)
pt4.SetTextFont(42)
pt4.SetTextSize(0.035)
pt4.SetBorderSize(0)
pt4.SetTextAlign(32)
pt4.AddText(categlat)
pt4.AddText("HH #rightarrow bbbb") 
legend = ROOT.TLegend(0.58,0.78,0.85,0.897)
legend.SetLineColor(ROOT.kWhite)
legend.SetBorderSize(1)
legend.SetTextSize(0.035)
legend.SetNColumns(1)
legend.AddEntry(grobs, "Observed", "l")
legend.AddEntry(grexp, "SM expected", "l")
pt6 = ROOT.TPaveText(0.94,0.28,0.99,0.33,"brNDC")
pt6.SetBorderSize(0)
pt6.SetFillColor(0)
pt6.SetTextColor(15)
pt6.SetTextSize(0.035)
pt6.SetTextFont(42)
pt6.SetFillStyle(0)
pt6.AddText("68%")
pt7 = ROOT.TPaveText(0.94,0.70,0.99,0.75,"brNDC")
pt7.SetBorderSize(0)
pt7.SetFillColor(0)
pt7.SetTextColor(15)
pt7.SetTextSize(0.035)
pt7.SetTextFont(42)
pt7.SetFillStyle(0)
pt7.AddText("95%")



#Frame
hframe = ROOT.TH1F('hframe', '', 100, ranges[0], ranges[1])
hframe.SetMinimum(0)
hframe.SetMaximum(5)
hframe.GetYaxis().SetTitleSize(0.047)
hframe.GetXaxis().SetTitleSize(0.055)
hframe.GetYaxis().SetLabelSize(0.045)
hframe.GetXaxis().SetLabelSize(0.045)
hframe.GetXaxis().SetLabelOffset(0.012)
hframe.GetYaxis().SetTitleOffset(1.2)
hframe.GetXaxis().SetTitleOffset(1.1)
hframe.GetYaxis().SetTitle("-2#Deltaln(L)")
hframe.GetXaxis().SetTitle(xaxistitle)
hframe.SetStats(0)

#Draw plot
ROOT.gPad.SetTicky()
hframe.Draw()
grobs.Draw("Lsame")
grexp.Draw("Lsame")
pt1.Draw()
pt2.Draw()
c1.Update()
c1.RedrawAxis("g")
pt4.Draw()
pt6.Draw()
pt7.Draw()
line1.Draw()
line2.Draw()
legend.Draw()
redrawBorder()
c1.Update()
c1.SaveAs("%s_obsexp.pdf"%outname)
