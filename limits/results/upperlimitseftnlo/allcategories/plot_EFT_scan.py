import ROOT
import collections 
ROOT.gROOT.SetBatch(True)

def setBarWidth(gr, bw, isX = True):
    for ipt in range(0, gr.GetN()):
        if isX:
            gr.SetPointEXhigh(ipt, bw)
            gr.SetPointEXlow(ipt, bw)
        else:
            gr.SetPointEYhigh(ipt, bw)
            gr.SetPointEYlow(ipt, bw)


def parseFile(filename):
    f = open(filename)
    res = collections.OrderedDict()
    for l in f:
        l = l.strip()
        if not l: continue
        if 'BENCH' in l: continue
        ## BENCH                  EXP                  OBS,              +2sigma,              +1sigma,              -1sigma,           -2sigma
        tokens = l.split()
        bn  = int(tokens[0])
        print '... parsing benchmark', bn
        exp = float(tokens[1])*(0.339)/(0.582*0.582) #in fb
        obs = float(tokens[2])*(0.339)/(0.582*0.582)
        p2s = float(tokens[3])*(0.339)/(0.582*0.582)
        p1s = float(tokens[4])*(0.339)/(0.582*0.582)
        m1s = float(tokens[5])*(0.339)/(0.582*0.582)
        m2s = float(tokens[6])*(0.339)/(0.582*0.582)

        res[bn] = (exp, obs, p2s, p1s, m1s, m2s)
    f.close()
    return res

values = parseFile('benchOutputCombined.txt')
thelabsize = 0.045
blind = False

agrObs    = ROOT.TGraphAsymmErrors()
agrExp    = ROOT.TGraphAsymmErrors()
agr1sigma = ROOT.TGraphAsymmErrors()
agr2sigma = ROOT.TGraphAsymmErrors()

for ipt in range(0, 12):
    bn = ipt + 1
    agrExp.    SetPoint(agrExp.GetN(),    bn, values[bn][0])
    agr1sigma. SetPoint(agr1sigma.GetN(), bn, values[bn][0])
    agr2sigma. SetPoint(agr2sigma.GetN(), bn, values[bn][0])
    agrObs.    SetPoint(agrObs.GetN(),    bn, values[bn][1])

    agr2sigma.SetPointEYhigh(agr1sigma.GetN()-1, abs(values[bn][2] - values[bn][0]))
    agr1sigma.SetPointEYhigh(agr2sigma.GetN()-1, abs(values[bn][3] - values[bn][0]))

    agr1sigma.SetPointEYlow(agr2sigma.GetN()-1, abs(values[bn][4] - values[bn][0]))
    agr2sigma.SetPointEYlow(agr1sigma.GetN()-1, abs(values[bn][5] - values[bn][0]))

setBarWidth(agr1sigma, 0.3)
setBarWidth(agr2sigma, 0.3)
setBarWidth(agrExp, 0.0)
setBarWidth(agrExp, 0.0, isX=False)

## make the plot
c1 = ROOT.TCanvas("cc", "cc", 650, 500)
c1.SetLogy()
c1.cd()
c1.SetFrameLineWidth(3)
c1.SetBottomMargin (0.15)
c1.SetRightMargin (0.05)
c1.SetLeftMargin (0.15)

agrExp.SetTitle("Expected CLs")
agrExp.SetMarkerStyle(24)
agrExp.SetMarkerColor(4)
agrExp.SetMarkerSize(0.8)
agrExp.SetLineColor(4)
agrExp.SetLineStyle(2)
agrExp.SetFillColor(0)

agr1sigma.SetTitle("Expected #pm 1#sigma")
agr1sigma.SetMarkerStyle(0)
agr1sigma.SetMarkerColor(3)
agr1sigma.SetFillColor(ROOT.kGreen+1)
agr1sigma.SetLineColor(ROOT.kGreen+1)
agr1sigma.SetFillStyle(1001)
agr1sigma.SetMarkerStyle(0)

agr2sigma.SetMarkerColor(5)
agr2sigma.SetFillColor(ROOT.TColor.GetColor(254, 234, 0))
agr2sigma.SetLineColor(ROOT.TColor.GetColor(254, 234, 0))
agr2sigma.SetFillStyle(1001)
agr2sigma.GetYaxis().SetTitleSize(0.047)
agr2sigma.GetXaxis().SetTitleSize(0.055)
agr2sigma.GetYaxis().SetLabelSize(0.045)
agr2sigma.GetXaxis().SetLabelSize(0.0)
agr2sigma.GetXaxis().SetLabelOffset(0.012)
agr2sigma.GetYaxis().SetTitleOffset(1.15)
agr2sigma.GetXaxis().SetTitleOffset(1.1)
agr2sigma.SetFillColor(ROOT.kOrange)
agr2sigma.SetLineColor(ROOT.kOrange)

####### the frame
hframe = ROOT.TH1F("hframe", ";Shape benchmark;#sigma_{ggF} (pp#rightarrowHH) [fb]", 13, 0.01, 12.99)
hframe.SetStats(0)
#hframe.SetMinimum(0.01)
#hframe.SetMaximum(500)
hframe.SetMinimum(5)
hframe.SetMaximum(7000)
hframe.GetYaxis().SetTitleSize(0.047)
hframe.GetXaxis().SetTitleSize(0.055)
hframe.GetYaxis().SetLabelSize(thelabsize)
# hframe.GetXaxis().SetLabelSize(0.045)
hframe.GetXaxis().SetLabelSize(0.00) # do by hand
hframe.GetXaxis().SetLabelOffset(0.012)
hframe.GetYaxis().SetTitleOffset(1.15)
hframe.GetXaxis().SetTitleOffset(1.1)

# hframe.GetXaxis().SetBinLabel(13, "a")
ttext_label = ROOT.TLatex()
ttext_label.SetTextFont(42)
ttext_label.SetTextSize(thelabsize)
ttext_label.SetNDC(False)
# ttext_label.SetFillColor(ROOT.kWhite)


agrExp.SetMarkerSize(1.3)
agrObs.SetMarkerStyle(8)
agrObs.SetMarkerSize(1.3)
agrObs.SetLineWidth(2)

hframe.GetYaxis().SetNdivisions(20)
hframe.GetXaxis().SetNdivisions(20)
hframe.Draw()
agr2sigma.Draw("E2same")
agr1sigma.Draw("E2SAME")
agrExp.Draw("PSAME")
if not blind:
    agrObs.Draw("PESAME")

######################

legend = ROOT.TLegend()
legend.SetLineColor(ROOT.kWhite)
legend.SetBorderSize(0)
legend.SetX1(0.179012)
legend.SetY1(0.167368)
legend.SetX2(0.689815)
legend.SetY2(0.314737)

legend.SetNColumns(2)
legend.SetHeader('95% CL upper limits', 'c')
# if hasObs:
legend.AddEntry(agrObs, "Observed", "p")
legend.AddEntry(agr1sigma, "68% expected", "f")
legend.AddEntry(agrExp, "Median expected", "p")
legend.AddEntry(agr2sigma, "95% expected", "f")
c1.SetLogy()
c1.SetGridy(1)
c1.SetGridx(1)

pt = ROOT.TPaveText(0.1663218-0.02,0.886316,0.3045977-0.02,0.978947,"brNDC")
pt.SetBorderSize(0)
pt.SetTextAlign(12)
pt.SetTextFont(62)
pt.SetTextSize(0.05)
pt.SetFillColor(0)
pt.SetFillStyle(0)
pt.AddText("CMS #font[52]{Preliminary}" )

off1bis = 0.065
off1bisx = 0.015
pt1Bis = ROOT.TPaveText(0.1663218-0.02+off1bisx,0.886316-off1bis,0.3045977-0.02+off1bisx,0.978947-off1bis,"brNDC")
pt1Bis.SetBorderSize(0)
pt1Bis.SetTextAlign(12)
pt1Bis.SetTextFont(42)
pt1Bis.SetTextSize(0.04)
pt1Bis.SetFillColor(ROOT.kWhite)
pt1Bis.SetFillStyle(0)
pt1Bis.AddText("All categories")


boxBand = ROOT.TLegend(0.17284, 0.790, 0.929012, 0.894737)
boxBand.SetFillColor(ROOT.kWhite)
boxBand.SetBorderSize(0)

pt2 = ROOT.TPaveText(0.79,0.9066667,0.8997773,0.957037,"brNDC")
pt2.SetBorderSize(0)
pt2.SetFillColor(0)
pt2.SetTextSize(0.040)
pt2.SetTextFont(42)
pt2.SetFillStyle(0)
pt2.AddText("138 fb^{-1} (13 TeV)")

# pt4 = TPaveText(0.4819196,0.7780357+0.012,0.9008929,0.8675595+0.01+0.012,"brNDC")
# pt4 = TPaveText(0.4819196,0.67,0.9008929,0.77,"brNDC")
pt4 = ROOT.TPaveText(0.534,0.79,0.93,0.88,"brNDC")
pt4.SetTextAlign(12)
pt4.SetFillColor(ROOT.kWhite)
pt4.SetFillStyle(1001)
pt4.SetTextFont(42)
pt4.SetTextSize(0.05)
pt4.SetBorderSize(0)
pt4.SetTextAlign(32)
# pt4.AddText(channelsName[ic]) 
pt4.AddText("HH#rightarrowbbbb") 
# if ic == 3 : pt4.AddText("#scale[0.8]{Combined channels}")
pt.Draw()
pt2.Draw()
# pt4.Draw()

# c1.SetTickx()
c1.SetTicky()
c1.Update()
c1.RedrawAxis();
c1.Update()
c1.RedrawAxis("g");
legend.Draw()
c1.Update()
boxBand.Draw()
pt1Bis.Draw()

ymin_labels = 3
for ival in range(0, 12):
    if ival < 9: ttext_label.DrawLatex(0.85+ival, ymin_labels, str(ival+1))
    else: ttext_label.DrawLatex(0.85+ival-0.15, ymin_labels, str(ival+1))
#ttext_label.DrawLatex(0.8+12-0.2, ymin_labels, 'SM')

pt4.Draw()
c1.Update()
raw_input()

c1.SaveAs('benchmark_nlo_plot_comb_run2.pdf')
c1.SaveAs('benchmark_nlo_plot_comb_run2.jpg')

#### print a text table
print "%10s %20s %20s, %20s, %20s, %20s, %20s" % ("BENCH", "EXP" , "OBS", "+2sigma", "+1sigma", "-1sigma", "-2sigma")
for ipt in range(0, agrExp.GetN()):
    x_exp = ROOT.Double(0.0)
    y_exp = ROOT.Double(0.0)
    x_obs = ROOT.Double(0.0)
    y_obs = ROOT.Double(0.0)

    agrExp.GetPoint(ipt, x_exp, y_exp)
    agrObs.GetPoint(ipt, x_obs, y_obs)

    y_p2s = y_exp + agr2sigma.GetErrorYhigh(ipt)
    y_p1s = y_exp + agr1sigma.GetErrorYhigh(ipt)
    y_m1s = y_exp - agr1sigma.GetErrorYlow(ipt)
    y_m2s = y_exp - agr2sigma.GetErrorYlow(ipt)

    print "{: >10} {: >20} {: >20} {: >20} {: >20} {: >20} {: >20}".format (str(ipt), y_exp, y_obs, y_p2s, y_p1s, y_m1s, y_m2s)
