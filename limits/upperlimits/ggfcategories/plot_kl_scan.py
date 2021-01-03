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
   l.DrawLine(ROOT.gPad.GetUxmax(), 0.5, ROOT.gPad.GetUxmax(), ROOT.gPad.GetUymax());
   l.DrawLine(ROOT.gPad.GetUxmin(), 0.5, ROOT.gPad.GetUxmin(), ROOT.gPad.GetUymax());
   l.DrawLine(ROOT.gPad.GetUxmin(), 0.5, ROOT.gPad.GetUxmax(), 0.5);

def getVals(fname):
	fIn = ROOT.TFile.Open(fname)
	tIn = fIn.Get('limit')
	if tIn.GetEntries() != 5:
		print "*** WARNING: cannot parse file", fname, "because nentries != 5"
		raise RuntimeError('cannot parse file')
	vals = []
	for i in range(0, tIn.GetEntries()):
		tIn.GetEntry(i)
		qe = tIn.quantileExpected
		lim = tIn.limit
		vals.append((qe,lim))
	return vals

################################################################################################
###########OPTIONS
parser = argparse.ArgumentParser(description='Command line parser of skim options')
parser.add_argument('--datasetid', dest='datasetid',help='Dataset:  (1) 2016, (2) 2017, (3) 2018, (4) Run2', required = True)
parser.add_argument('--categid',   dest='categid',  help='Category: (1) GGFcateg1, (2) GGFcateg2, (3) GGF-HH-Combination and (4) Full Combination', required = True)
args = parser.parse_args()
###########
###CREATE TAGS
dataid  = args.datasetid
categid = args.categid

##Dataset properties
if dataid == '1':
  folder   = 'kl_limits_2016'
  outname  = 'kl_scan_2016'
  datalumi = "2016, 35.9 fb^{-1} (13 TeV)"
elif dataid == '2':
  folder   = 'kl_limits_2017'
  outname  = 'kl_scan_2017'
  datalumi = "2017, 41.5 fb^{-1} (13 TeV)"
elif dataid == '3':
  folder   = 'kl_limits_2018'
  outname  = 'kl_scan_2018'
  datalumi = "2018, 59.7 fb^{-1} (13 TeV)"
elif dataid == '4':
  folder   = 'kl_limits_20172018'
  outname  = 'kl_scan_20172018'
  datalumi = "2017+2018, 101.2 fb^{-1} (13 TeV)"
elif dataid == '5':
  folder   = 'kl_limits_run2'
  outname  = 'kl_scan_run2'
  datalumi = "Run-2, 137.1 fb^{-1} (13 TeV)"
else:
  print "Dataset is not specified correctly! No plot is done"	
  sys.exit()

##Categories properties
if categid == '1':
  categlat = 'GGF-HH Cat-1'
  outname += '_ggfcat1'
  folder +='_1'
  yaxismax=8000
elif categid == '2':
  categlat = 'GGF-HH Cat-2'
  outname += '_ggfcat2' 
  folder +='_2'
  yaxismax=8000
elif categid == '3':
  categlat = 'GGF-HH Combination'
  outname += '_ggfcomb'
  if dataid=='5':
  	yaxismax=4000
  else:
  	yaxismax=7500  	
elif categid == '4':
  categlat = 'ggF-HH + VBF-HH Combination'
  outname += '_fullcomb'
else:
  print "Category is not specfied correctly! No plot is done"	
  sys.exit()

#Plot it
c1 = ROOT.TCanvas("c1", "c1", 1300, 1000)
c1.SetFrameLineWidth(3)
c1.SetBottomMargin (0.15)
c1.SetRightMargin (0.05)
c1.SetLeftMargin (0.15)
c1.SetGridx()
c1.SetGridy()

mg = ROOT.TMultiGraph()

### signals are normalsed to SM xs * B (bbbb) = 0.01043 pb
scaleToXS = 1000/(0.582*0.582) # in fb and without BR (HH -> bbbb)
gr2sigma = ROOT.TGraphAsymmErrors()
gr1sigma = ROOT.TGraphAsymmErrors()
grexp = ROOT.TGraph()
grobs = ROOT.TGraph()
coupling_xs = []
lambdas = []
#Get XS predicted 
with open("../../vbflines/config/ggfhhc3line.txt") as fobj:
	for line in fobj:
		row = line.split()
		coupling_xs.append( float(row[1])  )
		lambdas.append(float (row[0]) )
print coupling_xs
print lambdas
ptsList = [] # (x, obs, exp, p2s, p1s, m1s, m2s)
### read the scan with normal width
k=0
for ipt, lam in enumerate(lambdas):
	kl = '%.1f'%lam
	if not '-' in kl:
		kl = '+' + kl
	kl = kl.replace('-', 'm_')
	kl = kl.replace('+', 'p_')
	#kl = kl.replace('.', 'd')
	# print kl
	fname = folder + '/' + 'higgsCombinekl_{kl}.AsymptoticLimits.mH120.root'.format(kl=kl)
	vals  = getVals(fname)
	xs    = coupling_xs[k]
	obs   = scaleToXS*0.0 ## FIXME
	m2s_t = vals[0][1]*xs*scaleToXS
	m1s_t = vals[1][1]*xs*scaleToXS
	exp   = vals[2][1]*xs*scaleToXS
	p1s_t = vals[3][1]*xs*scaleToXS
	p2s_t = vals[4][1]*xs*scaleToXS

	## because the other code wants +/ sigma vars as deviations, without sign, from the centeal exp value...
	p2s = p2s_t - exp
	p1s = p1s_t - exp
	m2s = exp - m2s_t
	m1s = exp - m1s_t
	xval = lam

	ptsList.append((xval, obs, exp, p2s, p1s, m1s, m2s))
	k=k+1

ptsList.sort()
for ipt, pt in enumerate(ptsList):
	xval = pt[0]
	obs  = pt[1]
	exp  = pt[2]
	p2s  = pt[3]
	p1s  = pt[4]
	m1s  = pt[5]
	m2s  = pt[6]

	grexp.SetPoint(ipt, xval, exp)
	grobs.SetPoint(ipt, xval, obs)
	gr1sigma.SetPoint(ipt, xval, exp)
	gr2sigma.SetPoint(ipt, xval, exp)
	gr1sigma.SetPointError(ipt, 0,0,m1s,p1s)
	gr2sigma.SetPointError(ipt, 0,0,m2s,p2s)

######## set styles
grexp.SetMarkerStyle(24)
grexp.SetMarkerColor(4)
grexp.SetMarkerSize(0.8)
grexp.SetLineColor(ROOT.kBlue+2)
grexp.SetLineWidth(3)
grexp.SetLineStyle(2)
grexp.SetFillColor(0)
grobs.SetLineColor(1)
grobs.SetLineWidth(3)
grobs.SetMarkerColor(1)
grobs.SetMarkerStyle(20)
grobs.SetFillStyle(0)
gr1sigma.SetMarkerStyle(0)
gr1sigma.SetMarkerColor(3)
gr1sigma.SetFillColor(ROOT.kGreen+1)
gr1sigma.SetLineColor(ROOT.kGreen+1)
gr1sigma.SetFillStyle(1001)
gr2sigma.SetMarkerStyle(0)
gr2sigma.SetMarkerColor(5)
gr2sigma.SetFillColor(ROOT.kOrange)
gr2sigma.SetLineColor(ROOT.kOrange)
gr2sigma.SetFillStyle(1001)

mg.Add(gr2sigma, "3")
mg.Add(gr1sigma, "3")
mg.Add(grexp, "L")
mg.Add(grobs, "L")

###########
legend = ROOT.TLegend(0,0,0,0)
legend.SetX1(0.17284)
legend.SetY1(0.630526)
legend.SetX2(0.520062)
legend.SetY2(0.88)


legend.SetFillColor(ROOT.kWhite)
legend.SetBorderSize(0)
# legend
legend.SetHeader('95% CL upper limits')
#legend.AddEntry(grobs,"Observed","l")
legend.AddEntry(grexp, "Median expected", "l")
legend.AddEntry(gr1sigma, "68% expected", "f")
legend.AddEntry(gr2sigma, "95% expected", "f")

fakePlot3 = ROOT.TGraphAsymmErrors()
fakePlot3.SetFillColor(6)
fakePlot3.SetFillStyle(3001)
fakePlot3.SetLineColor(6)
fakePlot3.SetLineWidth(3)
legend.AddEntry(fakePlot3, "Theoretical prediction", "lf")


##### text
pt = ROOT.TPaveText(0.1663218-0.02,0.886316,0.3045977-0.02,0.978947,"brNDC")
pt.SetBorderSize(0)
pt.SetTextAlign(12)
pt.SetTextFont(62)
pt.SetTextSize(0.05)
pt.SetFillColor(0)
pt.SetFillStyle(0)
pt.AddText("CMS" )

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

pt4 = ROOT.TPaveText(0.6819196+0.036,0.7280357+0.015+0.02,0.9008929+0.036,0.8175595+0.015,"brNDC")
pt4.SetTextAlign(12)
pt4.SetFillColor(ROOT.kWhite)
pt4.SetFillStyle(1001)
pt4.SetTextFont(42)
pt4.SetTextSize(0.05)
pt4.SetBorderSize(0)
pt4.SetTextAlign(32)
pt4.AddText("HH #rightarrow bbbb") 

pt5 = ROOT.TPaveText(0.4819196+0.036,0.7780357+0.015+0.02,0.9008929+0.036,0.8675595+0.015,"brNDC")
pt5.SetTextAlign(12)
pt5.SetFillColor(ROOT.kWhite)
pt5.SetFillStyle(1001)
pt5.SetTextFont(42)
pt5.SetTextSize(0.05)
pt5.SetBorderSize(0)
pt5.SetTextAlign(32)
pt5.AddText(categlat)

graph1 = ROOT.TGraph("../../vbflines/config/ggfhhc3line.txt")
graph1.SetLineColor(6)
graph1.SetLineWidth(3)
for k in range (0,graph1.GetN()): graph1.GetY()[k] *= scaleToXS

hframe = ROOT.TH1F('hframe', '', 100, -28.0, 28.0)
hframe.SetMinimum(10)
hframe.SetMaximum(yaxismax)
hframe.GetYaxis().SetTitleSize(0.047)
hframe.GetXaxis().SetTitleSize(0.055)
hframe.GetYaxis().SetLabelSize(0.045)
hframe.GetXaxis().SetLabelSize(0.045)
hframe.GetXaxis().SetLabelOffset(0.012)
hframe.GetYaxis().SetTitleOffset(1.3)
hframe.GetXaxis().SetTitleOffset(1.1)
hframe.GetYaxis().SetTitle("95% CL on #sigma_{ggF}(pp#rightarrowHH) [fb]")
hframe.GetXaxis().SetTitle("#kappa_{#lambda}")

hframe.SetStats(0)
ROOT.gPad.SetTicky()
#ROOT.gPad.SetLogy()
hframe.Draw()
gr2sigma.Draw("3same")
gr1sigma.Draw("3same")
grexp.Draw("Lsame")
grobs.Draw("Lsame")
graph1.Draw("l same")
pt.Draw()
pt2.Draw()
redrawBorder()
c1.Update()
c1.RedrawAxis("g")
legend.Draw()
pt3.Draw()
pt4.Draw()
pt5.Draw()
c1.Update()
c1.SaveAs("%s.png"%outname)
c1.SaveAs("%s.pdf"%outname)
