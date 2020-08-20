import re, optparse
import os.path,sys
import argparse
from math import *
# from ROOT import *
import ROOT
ROOT.gROOT.SetBatch(True)

#####
def redrawBorder():
   # this little macro redraws the axis tick marks and the pad border lines.
   ROOT.gPad.Update();
   ROOT.gPad.RedrawAxis();
   l = ROOT.TLine()
   l.SetLineWidth(3)
   l.DrawLine(ROOT.gPad.GetUxmin(), ROOT.gPad.GetUymin(), ROOT.gPad.GetUxmax(), ROOT.gPad.GetUymin());
   l.DrawLine(ROOT.gPad.GetUxmax(), 0., ROOT.gPad.GetUxmax(), ROOT.gPad.GetUymax());
   l.DrawLine(ROOT.gPad.GetUxmin(), 0., ROOT.gPad.GetUxmin(), ROOT.gPad.GetUymax());
   l.DrawLine(ROOT.gPad.GetUxmin(), 0., ROOT.gPad.GetUxmax(), 0.);

def getVals(fname,coupling):
	fIn = ROOT.TFile.Open(fname)
	tIn = fIn.Get('limit')
	vals = []
	for i in range(1, tIn.GetEntries()):
		tIn.GetEntry(i)
		if coupling=='KT': 
			x = tIn.kt
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
parser.add_argument('--datasetid', dest='datasetid',  help='Dataset:  (1) 2016, (2) 2017, (3) 2018, (4) Run2', required = True)
parser.add_argument('--categid',   dest='categid',    help='Category: (1) GGFcateg1, (2) GGFcateg2, (3) GGF-HH-Combination and (4) Full Combination', required = True)
parser.add_argument('--coupling', dest='coupling',    help='Coupling: (1) KT, (2) KL', required = True)

args = parser.parse_args()
###########
###CREATE TAGS
dataid     = args.datasetid
categid    = args.categid
coupling   = args.coupling

##Couplings
if coupling=='KT' or coupling=='KL':
  print "Plotting likelihood scan for %s coupling"%coupling	
  if coupling=='KT': 
  	ranges = [-3.0,3.0]
  	xaxistitle  = '#kappa_{t}'
  else:
  	ranges = [-15.0,20.0]
  	xaxistitle  = '#kappa_{#lambda}'
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
  folder   = '%s_likelihood_run2'%coupling
  outname  = '%s_likelihood_1d_run2'%coupling
  datalumi = "Run-2, 137.1 fb^{-1} (13 TeV)"
else:
  print "Dataset is not specified correctly! No plot is done"	
  sys.exit()

##Categories properties
if categid == '1':
  categlat = 'GGF-HH Cat-1'
  outname += '_ggfcat1'
elif categid == '2':
  categlat = 'GGF-HH Cat-2'
  outname += '_ggfcat2' 
elif categid == '3':
  categlat = 'GGF-HH Combination'
  outname += '_ggfcomb'
elif categid == '4':
  categlat = 'GGF-HH + VBF-HH Combination'
  outname += '_fullcomb'
else:
  print "Category is not specfied correctly! No plot is done"	
  sys.exit()

#Plot it
c1 = ROOT.TCanvas("c1", "c1", 1300, 1000)
c1.SetFrameLineWidth(3)
c1.SetBottomMargin (0.15)
c1.SetRightMargin (0.07)
c1.SetLeftMargin (0.15)
#c1.SetGridx()
#c1.SetGridy()

grexp = ROOT.TGraph()
coupling_xs = []
lambdas     = []
#Read file and values
fname = folder + '/' + 'higgsCombineTest.MultiDimFit.mH120.root'
vals  = getVals(fname,coupling)
for ipt, pt in enumerate(vals):
	xval = pt[0]
	exp  = pt[1]
	grexp.SetPoint(ipt, xval, exp)
print "value",grexp.Eval(-2.8) 
######## set styles
grexp.SetMarkerStyle(8)
grexp.SetMarkerColor(4)
grexp.SetMarkerSize(2)
grexp.SetLineColor(ROOT.kBlue+2)
grexp.SetLineWidth(5)
grexp.SetLineStyle(1)
grexp.SetFillColor(0)
############ legend
#####legend = ROOT.TLegend(0,0,0,0)
#####legend.SetX1(0.17284)
#####legend.SetY1(0.630526)
#####legend.SetX2(0.520062)
#####legend.SetY2(0.88)
#####legend.SetFillColor(ROOT.kWhite)
#####legend.SetBorderSize(0)
#####legend.SetHeader('Likelihood Scan')
#####legend.AddEntry(grexp, "Expected", "l")
##lINES
line1 = ROOT.TLine(ranges[0],1,ranges[1],1)
line1.SetLineColor(15)
line1.SetLineWidth(2)
line1.SetLineStyle(2)
line2 = ROOT.TLine(ranges[0],3.82,ranges[1],3.82)
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
pt1.AddText("CMS" )
pt2 = ROOT.TPaveText(0.70,0.9066667,0.8997773,0.957037,"brNDC")
pt2.SetBorderSize(0)
pt2.SetFillColor(0)
pt2.SetTextSize(0.040)
pt2.SetTextFont(42)
pt2.SetFillStyle(0)
pt2.AddText(datalumi)
pt3 = ROOT.TPaveText(0.27,0.90,0.35,0.96,"brNDC")
pt3.SetBorderSize(0)
pt3.SetFillColor(0)
pt3.SetTextSize(0.040)
pt3.SetTextFont(42)
pt3.SetFillStyle(0)
pt3.AddText("Preliminary")
pt4 = ROOT.TPaveText(0.4819196+0.036,0.6780357+0.015+0.02+0.04,0.9008929,0.7675595+0.015+0.04,"brNDC")
pt4.SetTextAlign(12)
pt4.SetFillColor(ROOT.kWhite)
pt4.SetFillStyle(1001)
pt4.SetTextFont(42)
pt4.SetTextSize(0.05)
pt4.SetBorderSize(0)
pt4.SetTextAlign(32)
pt4.AddText("HH #rightarrow bbbb") 
pt5 = ROOT.TPaveText(0.1819196+0.036,0.7780357+0.015+0.02,0.9008929,0.8675595+0.015,"brNDC")
pt5.SetTextAlign(12)
pt5.SetFillColor(ROOT.kWhite)
pt5.SetFillStyle(1001)
pt5.SetTextFont(42)
pt5.SetTextSize(0.05)
pt5.SetBorderSize(0)
pt5.SetTextAlign(32)
pt5.AddText(categlat+", assuming SM")
pt6 = ROOT.TPaveText(0.94,0.2066667,0.9897773,0.257037,"brNDC")
pt6.SetBorderSize(0)
pt6.SetFillColor(0)
pt6.SetTextColor(15)
pt6.SetTextSize(0.040)
pt6.SetTextFont(42)
pt6.SetFillStyle(0)
pt6.AddText("68%")
pt7 = ROOT.TPaveText(0.94,0.4166667,0.9897773,0.467037,"brNDC")
pt7.SetBorderSize(0)
pt7.SetFillColor(0)
pt7.SetTextColor(15)
pt7.SetTextSize(0.040)
pt7.SetTextFont(42)
pt7.SetFillStyle(0)
pt7.AddText("95%")
#Frame
hframe = ROOT.TH1F('hframe', '', 100, ranges[0], ranges[1])
hframe.SetMinimum(0)
hframe.SetMaximum(10)
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
#ROOT.gPad.SetLogy()
hframe.Draw()
grexp.Draw("P Lsame")
pt1.Draw()
pt2.Draw()
redrawBorder()
c1.Update()
c1.RedrawAxis("g")
#legend.Draw()
pt3.Draw()
pt4.Draw()
pt5.Draw()
pt6.Draw()
pt7.Draw()
line1.Draw()
line2.Draw()
c1.Update()
c1.SaveAs("%s.png"%outname)
c1.SaveAs("%s.pdf"%outname)
