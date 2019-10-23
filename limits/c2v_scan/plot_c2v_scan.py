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
parser.add_argument('--dataset',      dest='dataset',  help='Year of the dataset: (1) 2016, (2) 2017, (3) 2018, (4) 2016+2017+2018',  required = True)
args = parser.parse_args()
###########
###CREATE TAGS
dataid  = args.dataset

if dataid == '1':
  folder   = 'c2vcards_2016'
  outname  = 'c2v_scan_2016.pdf'
  datalumi = "35.9 fb^{-1} (13 TeV)"
elif dataid == '2':
  folder   = 'c2vcards_2017'
  outname  = 'c2v_scan_2017.pdf'
  datalumi = "36.7 fb^{-1} (13 TeV)"
elif dataid == '3':
  folder   = 'c2vcards_2018'
  outname  = 'c2v_scan_2018.pdf'
  datalumi = "59.7 fb^{-1} (13 TeV)"
elif dataid == '4':
  folder   = 'c2vcards_COMB'
  outname  = 'c2v_scan_COMB.pdf'
  datalumi = "132.3 fb^{-1} (13 TeV)"
else:
  print "Dataset is not specified correctly! No plot is done"	
  sys.exit()


c1 = ROOT.TCanvas("c1", "c1", 650, 500)
c1.SetFrameLineWidth(3)
c1.SetBottomMargin (0.15)
c1.SetRightMargin (0.05)
c1.SetLeftMargin (0.15)
c1.SetGridx()
c1.SetGridy()

mg = ROOT.TMultiGraph()

#lambdas = [x for x in range(-10, 11)]
lambdas = [ -5.5 + (x*0.1) for x in range(0,131)]  
print "C2V points list: ", lambdas

### signals are normalsed to SM xs * B (bbbb) = 0.01043 pb
scaleToXS = 1000/(0.58*0.58) # in fb and without BR (HH -> bbbb)
gr2sigma = ROOT.TGraphAsymmErrors()
gr1sigma = ROOT.TGraphAsymmErrors()
grexp = ROOT.TGraph()
grobs = ROOT.TGraph()
coupling_xs = []
#Get XS predicted 
with open("config/vbfline.txt") as fobj:
	for line in fobj:
		row = line.split()
		coupling_xs.append(row)

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
	fname = folder + '/' + 'higgsCombinec2v_{kl}.AsymptoticLimits.mH120.root'.format(kl=kl)
	vals  = getVals(fname)
	xs = coupling_xs[k]
	obs   = scaleToXS*0.0 ## FIXME
	m2s_t = vals[0][1]*float(xs[1])*scaleToXS
	m1s_t = vals[1][1]*float(xs[1])*scaleToXS
	exp   = vals[2][1]*float(xs[1])*scaleToXS
	p1s_t = vals[3][1]*float(xs[1])*scaleToXS
	p2s_t = vals[4][1]*float(xs[1])*scaleToXS

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
fakePlot3.SetFillColor(ROOT.kRed)
fakePlot3.SetFillStyle(3001)
fakePlot3.SetLineColor(ROOT.kRed)
fakePlot3.SetLineWidth(3)
legend.AddEntry(fakePlot3, "Theoretical prediction", "lf")


##### text
# pt = ROOT.TPaveText(0.1663218,0.886316,0.3045977,0.978947,"brNDC")
pt = ROOT.TPaveText(0.1663218-0.02,0.886316,0.3045977-0.02,0.978947,"brNDC")
pt.SetBorderSize(0)
pt.SetTextAlign(12)
pt.SetTextFont(62)
pt.SetTextSize(0.05)
pt.SetFillColor(0)
pt.SetFillStyle(0)
pt.AddText("CMS" )
pt2 = ROOT.TPaveText(0.79,0.9066667,0.8997773,0.957037,"brNDC")
pt2.SetBorderSize(0)
pt2.SetFillColor(0)
pt2.SetTextSize(0.040)
pt2.SetTextFont(42)
pt2.SetFillStyle(0)
pt2.AddText(datalumi)

pt4 = ROOT.TPaveText(0.4819196+0.036,0.7780357+0.015+0.02,0.9008929+0.036,0.8675595+0.015,"brNDC")
pt4.SetTextAlign(12)
pt4.SetFillColor(ROOT.kWhite)
pt4.SetFillStyle(1001)
pt4.SetTextFont(42)
pt4.SetTextSize(0.05)
pt4.SetBorderSize(0)
pt4.SetTextAlign(32)
pt4.AddText("HH #rightarrow bbbb") 


## ###### theory lines
#xmin=-10
#xmax=10
#yt=1
#BR = 1
#myFunc = ROOT.TF1 ("myFunc", functionGF_kl_wrap, xmin, xmax, 1)
#graph = ROOT.TGraph(myFunc);
graph = ROOT.TGraph("config/vbfline.txt")
ci = ROOT.TColor.GetColor("#ff0000")
graph.SetLineColor(ci)
graph.SetLineWidth(2)
for k in range (0,graph.GetN()): graph.GetY()[k] *= scaleToXS

hframe = ROOT.TH1F('hframe', '', 100, -5, 7.5)
hframe.SetMinimum(0.5)
hframe.SetMaximum(900000)
hframe.GetYaxis().SetTitleSize(0.047)
hframe.GetXaxis().SetTitleSize(0.055)
hframe.GetYaxis().SetLabelSize(0.045)
hframe.GetXaxis().SetLabelSize(0.045)
hframe.GetXaxis().SetLabelOffset(0.012)
hframe.GetYaxis().SetTitleOffset(1.2)
hframe.GetXaxis().SetTitleOffset(1.1)
hframe.GetYaxis().SetTitle("95% CL on #sigma_{VBF}(pp#rightarrowHHjj) [fb]")
hframe.GetXaxis().SetTitle("c_{2V}")

hframe.SetStats(0)
ROOT.gPad.SetTicky()
ROOT.gPad.SetLogy()
hframe.Draw()
gr2sigma.Draw("3same")
gr1sigma.Draw("3same")
grexp.Draw("Lsame")
grobs.Draw("Lsame")
graph.Draw("l same")
pt.Draw()
pt2.Draw()
redrawBorder()
c1.Update()
c1.RedrawAxis("g")
legend.Draw()
pt4.Draw()
c1.Update()
c1.Print(outname, 'pdf')
