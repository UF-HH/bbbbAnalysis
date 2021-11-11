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
   l.SetLineWidth(4)
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
parser.add_argument('--categid',   dest='categid',    help='Category: (1) GGF1-GGF2, (2) VBF1-VBF2, (3) GGF-VBF, (4) GGF1-GGF2-VBF1-VBF2', required = True)
parser.add_argument('--coupling', dest='coupling',help='Coupling: (1) KL, (2) C2V, (3) CV', required = True)
parser.add_argument('--typeplot', dest='typeplot',help='Type of plot: (1) Observed, (0) Expected', required = True)
parser.add_argument('--combline', dest='combline',help='Full combination line: (1) Yes, (0) No', required = True)

args = parser.parse_args()
###########
###CREATE TAGS
dataid     = args.datasetid
categid    = args.categid
coupling   = args.coupling
typeplot   = args.typeplot
combline   = args.combline

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
  categlat = ['Low m_{HH}','High m_{HH}']
  outname += '_ggfcat1ggfcat2'
  mainfolders = ['ggfcat1','ggfcat2']
  colors      = [ROOT.kRed,ROOT.kAzure+1]
elif categid == '2':
  categlat = ['VBF SM','VBF anomalous-#kappa_{2V}']
  outname += '_vbfcat1vbfcat2'
  mainfolders  = ['vbfcat1','vbfcat2']
  colors      = [ROOT.kOrange+1,ROOT.kGreen+1]
elif categid == '3':
  categlat = ['ggF categories','VBF categories']
  outname += '_ggfcatsvbfcats'
  mainfolders  = ['ggfcats','vbfcats']
  colors      = [ROOT.kRed,ROOT.kAzure+1]
elif categid == '4':
  categlat   = ['Low m_{HH}','High m_{HH}','VBF SM','VBF anomalous-#kappa_{2V}']
  outname += '_allcategories'
  mainfolders = ['ggfcat1','ggfcat2','vbfcat1','vbfcat2']
  colors     = [ROOT.kRed,ROOT.kAzure+1,ROOT.kOrange+1,ROOT.kGreen+1]
else:
  print "Category is not specfied correctly! No plot is done"	
  sys.exit()

#Plot it
c1 = ROOT.TCanvas("c1", "c1", 1300, 1100)
c1.SetFrameLineWidth(3)
c1.SetBottomMargin (0.15)
c1.SetRightMargin (0.08)
c1.SetLeftMargin (0.15)

grlines = []
for k in range(0,len(categlat)):
   #Read file and values
   gr = ROOT.TGraph()
   if typeplot=='1': #observed
      fname = 'likelihood_'+ mainfolders[k]+ '/' + folder +'/higgsCombineTest.MultiDimFit.mH120.root'
   else:  #expected
      fname = 'likelihood_'+ mainfolders[k]+ '_expected/' + folder +'/higgsCombineTest.MultiDimFit.mH120.root'    
   vals  = getVals(fname,coupling)
   for ipt, pt in enumerate(vals):
     xval = pt[0]
     exp  = pt[1]
     gr.SetPoint(ipt, xval, exp)
   ######## set styles
   gr.SetMarkerStyle(8)
   gr.SetMarkerColor(4)
   gr.SetMarkerSize(2)
   gr.SetLineColor(colors[k])
   gr.SetLineWidth(5)
   gr.SetLineStyle(1)
   gr.SetFillColor(0)
   tmp = gr.Clone()
   del gr
   #tmp.SetDirectory(0)
   grlines.append(tmp)

if combline=='1':
  #Read file and values for observed
  grobsexp = ROOT.TGraph()
  if typeplot=='1':
     fname = 'likelihood_allcategories/'+folder+'/higgsCombineTest.MultiDimFit.mH120.root'
  if typeplot=='0':
     fname = 'likelihood_allcategories_expected/'+folder+'/higgsCombineTest.MultiDimFit.mH120.root'    
  vals  = getVals(fname,coupling)
  for ipt, pt in enumerate(vals):
    xval = pt[0]
    exp  = pt[1]
    grobsexp.SetPoint(ipt, xval, exp)
  ######## set styles
  grobsexp.SetMarkerStyle(8)
  grobsexp.SetMarkerColor(4)
  grobsexp.SetMarkerSize(2)
  grobsexp.SetLineColor(ROOT.kBlack)
  grobsexp.SetLineWidth(5)
  grobsexp.SetLineStyle(1)
  grobsexp.SetFillColor(0)

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
pt4 = ROOT.TPaveText(0.18,0.76,0.35,0.897,"brNDC")
#pt4.SetTextAlign(12)
pt4.SetFillColor(ROOT.kWhite)
pt4.SetFillStyle(1001)
pt4.SetTextFont(42)
pt4.SetTextSize(0.035)
pt4.SetBorderSize(0)
#pt4.SetTextAlign(32)
if typeplot=='1': pt4.AddText("Observed")
if typeplot=='0': pt4.AddText("SM Expected")
pt4.AddText("HH #rightarrow bbbb") 

if categid == '4': 
  legend = ROOT.TLegend(0.35,0.76,0.88,0.897)
  legend.SetNColumns(2)
else:  
  legend = ROOT.TLegend(0.35,0.76,0.88,0.897)
  legend.SetNColumns(1)
legend.SetLineColor(ROOT.kWhite)
legend.SetBorderSize(1)
legend.SetTextSize(0.035)

for k in range(0,len(categlat)): legend.AddEntry(grlines[k], categlat[k], "l")
if combline=='1': legend.AddEntry(grobsexp,"All categories", "l")


pt6 = ROOT.TPaveText(0.94,0.28,0.99,0.33,"brNDC")
pt6.SetBorderSize(0)
pt6.SetFillColor(0)
pt6.SetTextColor(15)
pt6.SetTextSize(0.040)
pt6.SetTextFont(42)
pt6.SetFillStyle(0)
pt6.AddText("68%")
pt7 = ROOT.TPaveText(0.94,0.70,0.99,0.75,"brNDC")
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
for k in range(0,len(categlat)): grlines[k].Draw("Lsame")
if combline=='1': grobsexp.Draw("Lsame")

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
#c1.SaveAs("%s_obsexp.png"%outname)
if typeplot=='1': c1.SaveAs("%s_lines_obs.pdf"%outname)
if typeplot=='0': c1.SaveAs("%s_lines_exp.pdf"%outname)

