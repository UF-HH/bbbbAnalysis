import ROOT
import numpy as np
from array import array
import sys
import math

ROOT.gStyle.SetOptStat(0)
ROOT.gROOT.SetBatch(True)

## to make inputs:
# combine -M MultiDimFit  ../datacard${YEAR}_comb_GGFcateg1GGFcateg2VBFcateg1VBFcateg2.root -D data_obs --redefineSignalPOIs r_gghh,r_qqhh --setParameters r=1,C2V=1,CV=1,kl=1,kt=1 --freezeParameters r,CV,C2V,kl,kt --algo contour2d --cl=0.68 --setParameterRanges r_gghh=-20.0,5.0:r_qqhh=-1000.0,300.0 --points=20

pois = ['r_gghh', 'r_qqhh'] ## x axis, y axis
poi_name  = ['#mu_{ggF}', '#mu_{VBF}']
SM_marker = (1,1)
categlat = 'All categories'

x_range = (-20, 20)
y_range = (-1200, 1200)

do_obs = True


if do_obs:
#    f_68 = ROOT.TFile.Open('/uscms/home/lcadamur/nobackup/XYH_4b_datacards/CMSSW_10_2_13/src/4bresults/gghh_qqhh_bestfit_run2/2dcont_20points_higgsCombineTest.MultiDimFit.mH120.root')
#    f_95 = ROOT.TFile.Open('/uscms/home/lcadamur/nobackup/XYH_4b_datacards/CMSSW_10_2_13/src/4bresults/gghh_qqhh_bestfit_run2/higgsCombinecontour95.MultiDimFit.mH120.root')
    f_68 = ROOT.TFile.Open('bestfit_allcategories/higgsCombineOBScontour68.MultiDimFit.mH120.root')
    f_95 = ROOT.TFile.Open('bestfit_allcategories/higgsCombineOBScontour95.MultiDimFit.mH120.root')
else:
    f_68 = ROOT.TFile.Open('bestfit_allcategories/higgsCombineEXPcontour68.MultiDimFit.mH120.root')
    f_95 = ROOT.TFile.Open('bestfit_allcategories/higgsCombineEXPcontour95.MultiDimFit.mH120.root')

t_68 = f_68.Get('limit')
t_95 = f_95.Get('limit')

oname = 'plot_2d_obs.pdf' if do_obs else 'plot_2d_exp.pdf'

def extract_contour (tIn, parlist):
    """ returns all values in parlist + the quantile expected """

    nev = tIn.GetEntries()
    print('.... extracting the following parameters: ', parlist, 'out of', nev, 'entries')
    # data = t.AsMatrix(["var1"])

    out = []
    for i in range(nev):
        tIn.GetEntry(i)
        # qe = tIn.quantileExpected
        # if qe < quantile:
        #     continue 
        values = [getattr(tIn, p) for p in parlist]
        values.append(tIn.quantileExpected)
        out.append(tuple(values))
    
    return out

c68 = extract_contour(t_68, pois)
c95 = extract_contour(t_95, pois)

for p in c68:
    if p[-1] == -1:
        bestfit = p
        print(".. best fit:", bestfit)
        break

## trim points by CL
c68 = [x for x in c68 if x[-1] >= 0.310]
c95 = [x for x in c95 if x[-1] >= 0.049]

# def angle_point(center, p):
#     dx = p[0] - center[0]
#     dy = p[1] - center[1]
#     v = [dx, dy]
#     horiz = [1, 0]
#     dot_product = np.dot(v, horiz)
#     # print(dot_product)
#     # angle = np.arccos(dot_product)    
#     return dot_product if dy >= 0 else -dot_product

# # c68 = sorted(c68, key=lambda x: angle_point(bestfit[:2], x[:2]))
# # c95 = sorted(c95, key=lambda x: angle_point(bestfit[:2], x[:2]))

# c68 = sorted(c68, key=lambda x: bestfit[0] > x[0])
# c95 = sorted(c95, key=lambda x: bestfit[0] > x[0])

g68 = ROOT.TGraph()
g95 = ROOT.TGraph()

def fill_graph(g, in_data):
    for i in range(len(in_data)):
        g.SetPoint(i, in_data[i][0], in_data[i][1])

fill_graph(g68, c68)
fill_graph(g95, c95)

def sort_points(g, bestfit):
    ## scale all the points to have the center of the plot at 0,0 (scale by -bestfit, -bestfit)
    for i in range(g.GetN()):
        g.GetX()[i] = g.GetX()[i] - bestfit[0]
        g.GetY()[i] = g.GetY()[i] - bestfit[1]
    # sort angularly
    if sys.version_info[0] == 3: # python 3 binding
        g.Sort(ROOT.TGraph.CompareArg)
    if sys.version_info[0] == 2: # python 2 binding somehow do not work, do by hand
        xvals = tuple([g.GetX()[i] for i in range(g.GetN())])
        yvals = tuple([g.GetY()[i] for i in range(g.GetN())])
        points = zip(xvals, yvals)
        def angle(p):
            hypo = math.sqrt(p[0]*p[0] + p[1]*p[1])
            cos = p[1]/hypo
            sin = p[0]/hypo
            ang = math.acos(abs(cos))
            if cos >= 0 and sin >= 0:
                return ang
            if cos >= 0 and sin < 0:
                return -ang 
            if cos < 0 and sin >= 0:
                return math.pi - ang
            if cos < 0 and sin < 0:
                return math.pi + ang
        sort_points = sorted(points, key=angle)
        for ip, p in enumerate(sort_points):
            g.SetPoint(ip, p[0], p[1])
    # shift back
    for i in range(g.GetN()):
        g.GetX()[i] = g.GetX()[i] + bestfit[0]
        g.GetY()[i] = g.GetY()[i] + bestfit[1]

# sort graphs
# g68.Sort(ROOT.TGraph.CompareArg)
# g95.Sort(ROOT.TGraph.CompareArg)

sort_points(g68, bestfit)
sort_points(g95, bestfit)

## first point = last point to close the circle
def close_graph(g):
    n = g.GetN()
    g.SetPoint(n, g.GetX()[0], g.GetY()[0])

close_graph(g68)
close_graph(g95)

c1 = ROOT.TCanvas('c1', 'c1', 600, 600)
c1.SetFrameLineWidth(3)
c1.SetBottomMargin (0.15)
c1.SetLeftMargin (0.15)

def set_style(g, color=ROOT.kBlack, mstyle=8, msize=0.5, lstyle=1, lsize=1):
    g.SetLineColor(color)
    g.SetMarkerColor(color)
    g.SetMarkerStyle(mstyle)
    g.SetMarkerSize(msize)
    g.SetLineStyle(lstyle)
    g.SetLineWidth(lsize)

set_style(g68, color=9, lsize=3)
set_style(g95, color=9, lsize=3, lstyle=2)

frame = ROOT.TH1D('frame', ';{};{}'.format(poi_name[0], poi_name[1]), 100, x_range[0], x_range[1])
frame.SetMinimum(y_range[0])
frame.SetMaximum(y_range[1])
frame.GetYaxis().SetTitleSize(0.047)
frame.GetXaxis().SetTitleSize(0.055)
frame.GetYaxis().SetLabelSize(0.045)
frame.GetXaxis().SetLabelSize(0.045)
frame.GetXaxis().SetLabelOffset(0.012)
frame.GetYaxis().SetTitleOffset(1.6)
frame.GetXaxis().SetTitleOffset(1.1)
frame.GetZaxis().SetTitleOffset(2.1)

frame.Draw('axis')
# g95.Draw('APL')
g95.Draw('L same')
g68.Draw('L same')


circle  = ROOT.TGraph(1, array('d',[bestfit[0]]), array('d',[bestfit[1]]))
circle.SetLineColor(9)
circle.SetLineWidth(4)
circle.SetMarkerColor(9)
circle.SetMarkerStyle(20)
circle.SetMarkerSize(2)
circle.Draw('p')

star  = ROOT.TGraph(1, array('d',[SM_marker[0]]), array('d',[SM_marker[1]]) )
star.SetLineColor(1)
star.SetLineWidth(4)
star.SetMarkerColor(1)
star.SetMarkerStyle(29)
star.SetMarkerSize(3)
star.Draw('p')



leg = ROOT.TLegend(0.4, 0.7, 0.88, 0.88)
leg.SetNColumns(1)
leg.SetBorderSize(0)
leg.SetTextSize(0.035)
leg.SetTextFont(42)
leg.SetLineColor(1)
leg.SetLineWidth(10)
leg.SetFillColor(0)
leg.SetFillStyle(0)
leg.AddEntry(star,"SM", "p")
leg.AddEntry(circle,"{} best fit".format(categlat), "p")
leg.AddEntry(g68,"{} 68% CL".format(categlat), "l")
leg.AddEntry(g95,"{} 95% CL".format(categlat), "l")
leg.Draw() 
 

#CMS labels
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
pt2.AddText("138 fb^{-1} (13 TeV)")

pt3 = ROOT.TPaveText(0.19,0.77,0.40,0.87,"brNDC")
#pt3.SetTextAlign(12)
pt3.SetFillColor(ROOT.kWhite)
pt3.SetFillStyle(1001)
pt3.SetTextFont(42)
pt3.SetTextSize(0.04)
pt3.SetBorderSize(0)
pt3.SetTextAlign(32)
if do_obs:
   pt3.AddText("Observed")
else:
   pt3.AddText("SM expected")    
pt3.AddText("HH #rightarrow bbbb") 
pt1.Draw()
pt2.Draw()
pt3.Draw()

c1.Update()
c1.Print(oname, 'pdf')