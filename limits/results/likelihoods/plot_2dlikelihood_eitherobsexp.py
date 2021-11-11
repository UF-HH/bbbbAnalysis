import re, optparse
import os.path,sys
import argparse
from math import *
from array import array
# from ROOT import *
import ROOT
ROOT.gROOT.SetBatch(True)

#####
def redrawBorder():
   ROOT.gPad.Update()
   ROOT.gPad.RedrawAxis()
   l=ROOT.TLine()
   l.SetLineWidth(4)
#   l.DrawLine(ROOT.gPad.GetUxmin(), ROOT.gPad.GetUymax(), ROOT.gPad.GetUxmax(), ROOT.gPad.GetUymax());
#   l.DrawLine(ROOT.gPad.GetUxmin(), ROOT.gPad.GetUymin(), ROOT.gPad.GetUxmax(), ROOT.gPad.GetUymin());
#   l.DrawLine(ROOT.gPad.GetUxmin(), ROOT.gPad.GetUymin(), ROOT.gPad.GetUxmin(), ROOT.gPad.GetUymax());
#   l.DrawLine(ROOT.gPad.GetUxmax(), ROOT.gPad.GetUymin(), ROOT.gPad.GetUxmax(), ROOT.gPad.GetUymax());
   l.DrawLine(ROOT.gPad.GetUxmin(), ROOT.gPad.GetUymax(), ROOT.gPad.GetUxmax(), ROOT.gPad.GetUymax());
   l.DrawLine(ROOT.gPad.GetUxmax(), ROOT.gPad.GetUymin(), ROOT.gPad.GetUxmax(), ROOT.gPad.GetUymax());
   l.DrawLine(ROOT.gPad.GetUxmin(), ROOT.gPad.GetUymin(), ROOT.gPad.GetUxmax(), ROOT.gPad.GetUymin());
   l.DrawLine(ROOT.gPad.GetUxmin(), ROOT.gPad.GetUymin(), ROOT.gPad.GetUxmin(), ROOT.gPad.GetUymax());

def getVals(fname,couplingid,cut):
	fIn = ROOT.TFile.Open(fname)
	tIn = fIn.Get('limit')
	vals = []
	for i in range(1, tIn.GetEntries()):
		tIn.GetEntry(i)
		if couplingid=='1': 
			x = tIn.C2V
			y = tIn.CV
		elif couplingid=='2':
			x = tIn.kl
			y = tIn.C2V
		elif couplingid=='3':
			x = tIn.CV
			y = tIn.kl
		else:
			x = tIn.kl
			y = tIn.kt
		z  = 2*(tIn.deltaNLL)
		if z<0: z=0
		if z<=cut: vals.append((x,y,z))
	return vals

################################################################################################
###########OPTIONS
parser = argparse.ArgumentParser(description='Command line parser of skim options')
parser.add_argument('--datasetid',  dest='datasetid',help='Dataset:  (1) 2016, (2) 2017, (3) 2018, (4) 20172018, (5) Run2', required = True)
parser.add_argument('--categid',    dest='categid',  help='Category: (1) GGF/VBFcateg1, (2) GGF/VBFcateg2, (3) GGF/VBF-HH-Combination and (4) Full Combination', required = True)
parser.add_argument('--couplingid', dest='couplingid', help='Coupling: (1) C2V-CV, (2) KL-C2V, (3) CV-KL, (4) KL-KT', required = True)
parser.add_argument('--typeplot', dest='typeplot',help='Type of plot: (1) Observed, (0) Expected', required = True)

args = parser.parse_args()
###########
###CREATE TAGS
dataid     = args.datasetid
categid    = args.categid
couplingid = args.couplingid
typeplot   = args.typeplot

##Couplings
if couplingid=='1' or couplingid=='2' or couplingid=='3' or couplingid=='4':
	print "Plotting 2D likelihood scan for coupling combination %s"%couplingid	
	if couplingid=='1': 
		if typeplot=='1':
			xranges = [-4.0,11.0]
		else:
			xranges = [-4.0,18.0]
		yranges = [-5.0,5.0]
		xaxistitle  = '#kappa_{2V}'
		yaxistitle  = '#kappa_{V}'
		comb = 'C2V_CV'
	elif couplingid=='2':
		yranges = [-1.5,3.0]
		xranges = [-7.0,14.0]
		yaxistitle  = '#kappa_{2V}'
		xaxistitle  = '#kappa_{#lambda}'
		comb = 'C2V_KL'
	elif couplingid=='3':
		xranges = [-1.8,1.8]
		yranges = [-7.0,14.0]
		xaxistitle  = '#kappa_{V}'		
		yaxistitle  = '#kappa_{#lambda}'
		comb = 'CV_KL'
	else:
		xranges = [-7.0,14.0]
		yranges = [-10.0,10.0]
		xaxistitle  = '#kappa_{#lambda}'		
		yaxistitle  = '#kappa_{t}'
		comb = 'KL_KT'
else:
	print "Coupling is not specfied correctly! No plot is done"		
	sys.exit()

##Dataset properties
if dataid == '1':
	folder   = '%s_likelihood_2016'%comb
	outname  = '%s_likelihood_2d_2016'%comb
	datalumi = "2016, 35.9 fb^{-1} (13 TeV)"
elif dataid == '2':
	folder   = '%s_likelihood_2017'%comb
	outname  = '%s_likelihood_2d_2017'%comb
	datalumi = "2017, 41.5 fb^{-1} (13 TeV)"
elif dataid == '3':
	folder   = '%s_likelihood_2018'%comb
	outname  = '%s_likelihood_2d_2018'%comb
	datalumi = "2018, 59.7 fb^{-1} (13 TeV)"
elif dataid == '4':
	folder   = '%s_likelihood_20172018'%comb
	outname  = '%s_likelihood_2d_20172018'%comb
	datalumi = "20172018, 101.3 fb^{-1} (13 TeV)"
elif dataid == '5':
	folder   = '%s_likelihood_run2'%comb
	outname  = '%s_likelihood_2d_run2'%comb
	datalumi = "138 fb^{-1} (13 TeV)"
else:
	print "Dataset is not specified correctly! No plot is done"	
	sys.exit()

##Categories properties
if categid == '1':
	categlat = 'GGF/VBF-HH Cat-1'
	outname += '_vbfcat1'
	mainfolder = 'VBFcat1'
elif categid == '2':
	categlat = 'GGF/VBF-HH Cat-2'
	outname += '_vbfcat2' 
	mainfolder = 'VBFcat2'
elif categid == '3':
	categlat = 'GGF/VBF-HH Combination'
	outname += '_vbfcomb'
	mainfolder = 'VBFcats'
elif categid == '4':
	categlat = 'All categories'
	outname += '_allcategories'
	mainfolder = 'allcategories'
else:
	print "Category is not specfied correctly! No plot is done"	
	sys.exit()

#Plot it
c1 = ROOT.TCanvas("c1", "c1", 1300, 1000)
c1.SetFrameLineWidth(3)
c1.SetBottomMargin (0.15)
c1.SetRightMargin (0.20)
c1.SetLeftMargin (0.15)
ROOT.gStyle.SetOptTitle(0)

gr12sigma = ROOT.TGraph2D()
#Read file and values
if typeplot=='1': #observed
   fname = 'likelihood_'+ mainfolder+ '/' + folder +'/higgsCombineTest.MultiDimFit.mH120.root'
else:  #expected
   fname = 'likelihood_'+ mainfolder+ '_expected/' + folder +'/higgsCombineTest.MultiDimFit.mH120.root'  

#for 68 CL% contours
vals  = getVals(fname,couplingid,12) #10
for ipt, pt in enumerate(vals):
	xval = pt[0]
	yval = pt[1]
	exp  = pt[2]
	gr12sigma.SetPoint(ipt, xval, yval, exp)
print "Total number of points < 12:",gr12sigma.GetN() #10

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
pt4 = ROOT.TPaveText(0.18,0.78,0.35,0.89,"brNDC")

pt4 = ROOT.TPaveText(0.7719196+0.036,0.7780357+0.015+0.02+0.01,0.8808929,0.8675595+0.015+0.01,"brNDC")
pt4.SetTextAlign(12)
pt4.SetFillColor(ROOT.kWhite)
pt4.SetFillStyle(1001)
pt4.SetTextFont(42)
pt4.SetTextSize(0.04)
pt4.SetBorderSize(0)
pt4.SetTextAlign(32)
if typeplot=='1': pt4.AddText("Observed")
if typeplot=='0': pt4.AddText("SM Expected")
pt4.AddText("HH #rightarrow bbbb") 
pt5 = ROOT.TPaveText(0.7719196+0.036,0.7780357+0.015+0.02+0.01,0.8808929,0.8675595+0.015+0.01,"brNDC")
pt5.SetTextAlign(12)
pt5.SetFillColor(ROOT.kWhite)
pt5.SetFillStyle(1001)
pt5.SetTextFont(42)
pt5.SetTextSize(0.05)
pt5.SetBorderSize(0)
pt5.SetTextAlign(32)
pt5.AddText(categlat)

#100 for 2
Np=50
if couplingid=='1':
   if typeplot=='1': Np=100
   else: Np=109
	
ROOT.gPad.SetTicky()
gr12sigma.SetNpx(Np) #50
gr12sigma.SetNpy(Np)

hframe = ROOT.TH2F('hframe', '', Np, xranges[0], xranges[1], Np, yranges[0], yranges[1])
hframe.GetYaxis().SetTitleSize(0.047)
hframe.GetXaxis().SetTitleSize(0.055)
hframe.GetYaxis().SetLabelSize(0.045)
hframe.GetXaxis().SetLabelSize(0.045)
hframe.GetXaxis().SetLabelOffset(0.012)
hframe.GetYaxis().SetTitleOffset(1.2)
hframe.GetXaxis().SetTitleOffset(1.1)
hframe.GetZaxis().SetTitleOffset(2.1)
hframe.GetXaxis().SetTitle(xaxistitle)
hframe.GetYaxis().SetTitle(yaxistitle)
hframe.SetStats(0)
hframe.SetMaximum(12.0) #10
hframe.SetMinimum(0.0)

histo = ROOT.TH2F()
histo = gr12sigma.GetHistogram()
histo.GetYaxis().SetTitleSize(0.047)
histo.GetZaxis().SetTitleSize(0.047)
histo.GetXaxis().SetTitleSize(0.055)
histo.GetYaxis().SetLabelSize(0.045)
histo.GetXaxis().SetLabelSize(0.045)
histo.GetXaxis().SetLabelOffset(0.012)
histo.GetYaxis().SetTitleOffset(1.1)
histo.GetXaxis().SetTitleOffset(1.1)
histo.GetZaxis().SetTitleOffset(1.1)
histo.GetXaxis().SetTitle(xaxistitle)
histo.GetYaxis().SetTitle(yaxistitle)
histo.GetZaxis().SetTitle("-2#Deltaln(L)")
histo.GetYaxis().SetRangeUser(yranges[0],yranges[1])
histo.GetXaxis().SetRangeUser(xranges[0],xranges[1])
histo.SetStats(0)
histo.SetMaximum(12.0) #12
histo.SetMinimum(0.0)


levels = array('d', [1.0,3.84] )
histo.SetContour(2,levels)
histo.SetEntries(1)
histo.Draw("COLZ")
if typeplot=='1': c1.SaveAs("%s_obs_check.pdf"%outname)
if typeplot=='0': c1.SaveAs("%s_exp_check.pdf"%outname)
histo.Draw("CONT LIST")
c1.Update()

#Draw plot
c2 = ROOT.TCanvas("c2", "c2", 1300, 1300)
c2.SetFrameLineWidth(3)
c2.SetBottomMargin (0.15)
c2.SetRightMargin (0.10)
c2.SetLeftMargin (0.15)
ROOT.gStyle.SetOptTitle(0)
hframe.Draw()

conts = ROOT.gROOT.GetListOfSpecials().FindObject("contours")
contLevel  = ROOT.TList()
curv       = ROOT.TGraph()
TotalConts = conts.GetSize()

for i in range(0,TotalConts):
	contLevel = conts.At(i)
	curv      = contLevel.First()
	for k in range(0,contLevel.GetSize()):
		curv.SetLineColor(9)
		curv.SetLineWidth(2)
		if k==contLevel.GetSize()-1: curv.Draw("L same")
		if i==1: curv.SetLineStyle(2)
		curv = contLevel.After(curv)


if typeplot=='0':
  xo = array('d', [1] )
  yo = array('d', [1] )
  x = array('d', [1] )
  y = array('d', [1] )
else:
  xo = array('d', [1] )
  yo = array('d', [1] )
  if couplingid=='1':  
	x = array('d', [0.415636] )
	y = array('d', [0.640634] )
  elif couplingid=='2': 
	x = array('d', [3.66421] )
	y = array('d', [0.91939] )
  else:  
	x = array('d', [1] )
	y = array('d', [1] )
star  = ROOT.TGraph(1,xo,yo)
star.SetLineColor(1)
star.SetLineWidth(4)
star.SetMarkerColor(1)
star.SetMarkerStyle(29)
star.SetMarkerSize(8)

circle  = ROOT.TGraph(1,x,y)
circle.SetLineColor(9)
circle.SetLineWidth(4)
circle.SetMarkerColor(9)
circle.SetMarkerStyle(20)
circle.SetMarkerSize(3)

line1  = ROOT.TLine(1,1,1,1)
line1.SetLineStyle(1)
line1.SetLineColor(9)
line1.SetLineWidth(3)

line2  = ROOT.TLine(1,1,1,1)
line2.SetLineStyle(2)
line2.SetLineColor(9)
line2.SetLineWidth(3)

#Ranges
if couplingid=='1':
  #hframe.GetXaxis().SetRangeUser(-2.0,3.0)
  #hframe.GetYaxis().SetRangeUser(-4.0,4.0)
  hframe.GetXaxis().SetNdivisions(6,8,0)
  hframe.GetYaxis().SetLimits(-4, 4)
  hframe.GetXaxis().SetLimits(-2, 3)
if couplingid=='2':
  hframe.GetXaxis().SetRangeUser(-7,14)
  hframe.GetYaxis().SetRangeUser(-1.5,3)
c2.Update()

#legend
leg_1 = ROOT.TLegend(0.17,0.17,0.47,0.33)
leg_1.SetNColumns(1)
leg_1.SetBorderSize(0)
leg_1.SetTextSize(0.035)
leg_1.SetTextFont(42)
leg_1.SetLineColor(1)
leg_1.SetLineWidth(10)
leg_1.SetFillColor(0)
leg_1.SetFillStyle(0)
leg_1.AddEntry(star,"SM", "p")
leg_1.AddEntry(circle,"All categories best fit", "p")
leg_1.AddEntry(line1,"All categories 68% CL", "l")
leg_1.AddEntry(line2,"All categories 95% CL", "l")
leg_1.Draw()  

pt1.Draw()
pt2.Draw()
c2.Update()
pt4.Draw()
c2.Update()
star.Draw("P SAME")
circle.Draw("P SAME")
redrawBorder()
c2.Update()

if typeplot=='1': c2.SaveAs("%s_obs.pdf"%outname)
if typeplot=='0': c2.SaveAs("%s_exp.pdf"%outname)
