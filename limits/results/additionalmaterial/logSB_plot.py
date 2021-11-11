# python logSB_plot.py --xmin -7 --xmax -1 --nbins 30 --oname plot_SB_fullrange.pdf --leg left
# python logSB_plot.py --xmin -3.5 --xmax -1.5 --nbins 20 --oname plot_SB_highrange.pdf --leg right


import ROOT
import math
import numpy as np

from argparse import ArgumentParser
ROOT.gROOT.SetBatch(True)
ROOT.gStyle.SetOptStat(0)

##This is done using the fitDiagnostics file, below the one used for the preliminary results
fname = '/uscms/home/guerrero/nobackup/Run2/HH2020/Spring/CMSSW_10_2_13/src/bbbbAnalysis/limits/prepareModelsApproval/physicsmodelrun2/fitDiagnostics.root'

## NOTE: if we use shapes_fit_b, there will be no signal in the output to compute log10(S/B)

categs   = [
    'GGFcateg1_{}',
    'GGFcateg2_{}',
    'VBFcateg1_{}',
    'VBFcateg2_{}',
]
years = ['2016', '20172018']

# categs   = [
#     'GGFcateg2_{}',
# ]
# years = ['20172018']

# categs   = [
#     'VBFcateg2_{}',
# ]
# years = ['2016']


## NOTE: total_signal is what is best-fitted by fitDiagnostic
## since we make the fit assuming kl=1, it automatically only includes SM and not the other kl=2.45 and kl=5 samples
## so we can use it to make the log10(S/B) plot

signame  = 'total_signal'
bkgname  = 'total_background'
dataname = 'data' ## is a TGraph!

parser = ArgumentParser('plot options')
parser.add_argument ('--sigscale', dest='sigscale', default=100, type=int)
parser.add_argument ('--ratiomin', dest='ratiomin', default=0.71, type=float)
parser.add_argument ('--ratiomax', dest='ratiomax', default=1.29, type=float)
parser.add_argument ('--xmin', dest='xmin', default=None, type=float)
parser.add_argument ('--xmax', dest='xmax', default=None, type=float)
parser.add_argument ('--nbins', dest='nbins', default=30, type=int)
parser.add_argument ('--oname', dest='oname', default='plot_log10sb.pdf')
parser.add_argument ('--leg', dest='leg', default='left')
args = parser.parse_args()

adddatahbars = False
# sigscale = 100
# ratiomin = 0.71
# ratiomax = 1.29
sigscale = args.sigscale
ratiomin = args.ratiomin
ratiomax = args.ratiomax

######
textfont = 43
titlesize = 20
labelsize = 18
xoffset = 4.0
yoffset = 1.5
######

all_histos_names = []
for y in years:
    for c in categs:
        hbkg  = '{}/{}/{}'.format('shapes_fit_b', c.format(y), bkgname)
        hsig  = '{}/{}/{}'.format('shapes_prefit', c.format(y), signame) ## signal at prefit has SM couplings
        gdata = '{}/{}/{}'.format('shapes_fit_b', c.format(y), dataname) ## data is always the same: 'shapes_fit_s', 'shapes_fit_b', 'shapes_prefit' are all the same
        all_histos_names.append((hbkg, hsig, gdata))

for h in all_histos_names:
    print(h)


def extract_from_histo(hIn_bkg, hIn_signal, gIn_data):
    """ return a list of values with the S/B and the associated error as (data, sig, bkg, bkg sumw2)"""
    
    nbins_l = [hIn_bkg.GetNbinsX(), hIn_signal.GetNbinsX(), gIn_data.GetN()]
    if len(set(nbins_l)) != 1:
        print('incompatible histograms: ', nbins_l)
        raise RuntimeError("incompatible histos")
    nbins = nbins_l[0]

    result    = []
    sumw2_bkg = hIn_bkg.GetSumw2()
    
    for ibin in range(1, nbins+1):
        
        n_bkg    = hIn_bkg.GetBinContent(ibin)
        n_signal = hIn_signal.GetBinContent(ibin)
        # n_data   = hIn_data.GetBinContent(ibin)
        n_data   = gIn_data.GetY()[ibin-1] ## npoints starts from 0 in a tgraph
        
        # e_bkg    = hIn_bkg.GetBinError(ibin)
        # e_signal = hIn_signal.GetBinError(ibin)
        # e_data   = hIn_data.GetBinError(ibin)
        
        ## todo: handle errors of 0 division

        # print(ibin, n_signal, n_bkg) ## fitted signal is negative
        # print("******* FIXME ONLY FOR TEST ABS")
        # n_signal = abs(hIn_signal.GetBinContent(ibin))
        if n_bkg == 0:
            continue # some bins are empty because of RooFit constraints

        log10sb = math.log10(n_signal/n_bkg)
        w2 = sumw2_bkg[ibin]

        point = (log10sb, n_data, n_signal, n_bkg, w2)
        result.append(point)
    return result


###############################

fIn  = ROOT.TFile.Open(fname)
values = []
for hbkg_name, hsig_name, gdata_name in all_histos_names:    
    print('... doing:', hbkg_name, hsig_name, gdata_name)
    hbkg   = fIn.Get(hbkg_name)
    hsig   = fIn.Get(hsig_name)
    gdata  = fIn.Get(gdata_name)
    v = extract_from_histo(hbkg, hsig, gdata)
    values += v

print(values)

###############################
# now make the plot

xmin = min(list(zip(*values))[0])
xmax = max(list(zip(*values))[0])
print('xmin:', xmin)
print('xmax:', xmax)

if args.xmin:
    xmin = args.xmin
if args.xmax:
    xmax = args.xmax

# some margin
delta = abs(xmax-xmin)
if delta == 0:
    delta += 0.1
xmin = xmin-0.01*delta
xmax = xmax+0.01*delta


nbins = args.nbins
make_flat = False

if not make_flat:
    h_bkg  = ROOT.TH1D('h_bkg',  ';log_{10}(S/B);Events', nbins, xmin, xmax)
    h_data = ROOT.TH1D('h_data', ';log_{10}(S/B);Events', nbins, xmin, xmax)
    h_sig  = ROOT.TH1D('h_sig',  ';log_{10}(S/B);Events', nbins, xmin, xmax)
else:
    ## define a binning of nbins that has approx. the same N events in every bin
    tot_bkg = sum(list(zip(*values))[-2]) # sum of bkg events
    tot_per_bin = tot_bkg/nbins
    print('... will make a const', nbins, 'binning from', tot_bkg, 'events -> ', tot_per_bin, 'events/bin')
    bkgevts = list(zip(*values))[-2]
    sob     = list(zip(*values))[0]
    input_binning_data = list(zip(sob, bkgevts))
    input_binning_data.sort()
    # print(input_binning_data)
    # make the cumulative
    cumul = []
    for i in range(len(input_binning_data)):
        if i == 0:
            cumul.append(input_binning_data[i][1])
        else:
            cumul.append(input_binning_data[i][1] + cumul[-1])
    # now find the binning
    bins = [xmin]
    runsum = 0
    for i in range(len(cumul)-1):
        runsum += input_binning_data[i][1]
        if runsum >= tot_per_bin:
            bins.append(0.5*(input_binning_data[i+1][0] + input_binning_data[i][0]))
            runsum = 0
    bins.append(xmax)

    print('... equispaced binning is:', bins)

    h_bkg  = ROOT.TH1D('h_bkg',  ';log_{10}(S/B);Events', nbins-1, np.asarray(bins, 'd'))
    h_data = ROOT.TH1D('h_data', ';log_{10}(S/B);Events', nbins-1, np.asarray(bins, 'd'))
    h_sig  = ROOT.TH1D('h_sig',  ';log_{10}(S/B);Events', nbins-1, np.asarray(bins, 'd'))


w2sums = [0] * (nbins+2) ## with u/oflow
for ioriginal, (log10sb, n_data, n_signal, n_bkg, w2) in enumerate(values):
    ibin = h_bkg.FindBin(log10sb)
    print(log10sb, '-->', ibin, ' .. idx in value list:', ioriginal, '... nbkg:', n_bkg)
    def incr(h, ibin, cont):
        c = h.GetBinContent(ibin)
        # print(h.GetName(), '..... OLD: ', c, " + ", cont, "in", ibin)
        h.SetBinContent(ibin, c + cont)
        # print(h.GetName(), '..... NEW:', h.GetBinContent(ibin))

    incr(h_bkg, ibin, n_bkg)
    incr(h_sig, ibin, n_signal)
    incr(h_data, ibin, n_data)
    # w2sums[ibin-1] += w2
    w2sums[ibin] += w2

for i in range(nbins):
    h_bkg.SetBinError(i+1, math.sqrt(w2sums[i+1]))

c1 = ROOT.TCanvas('c1', 'c1', 600, 600)

#### make pads

c1.cd()
pad1 = ROOT.TPad ("pad1", "pad1", 0, 0.25, 1, 1.0)
pad1.SetFrameLineWidth(3)
pad1.SetLeftMargin(0.15)
pad1.SetBottomMargin(0.02)
pad1.SetTopMargin(0.055)

c1.cd()
pad2 = ROOT.TPad ("pad2", "pad2", 0, 0.0, 1, 0.2496)
pad2.SetLeftMargin(0.15)
pad2.SetTopMargin(0.05)
pad2.SetBottomMargin(0.35)
pad2.SetGridy(True)
pad2.SetFrameLineWidth(3)

###############################
#convert h_data to graph with poissonian errors
h_data.SetBinErrorOption(ROOT.TH1.kPoisson)
g_data = ROOT.TGraphAsymmErrors()

for i in range(h_data.GetNbinsX()):
    x   = h_data.GetBinCenter(i+1)
    bw  = h_data.GetBinLowEdge(i+2) - h_data.GetBinLowEdge(i+1)
    xerr = 0.5*bw    
    if not adddatahbars:
        xerr = 0
    y   = h_data.GetBinContent(i+1)
    yerrup = h_data.GetBinErrorUp(i+1)
    yerrdo = h_data.GetBinErrorLow(i+1)
    n = g_data.GetN()
    if y > 0:
        g_data.SetPoint(n, x, y)
        g_data.SetPointError(n, xerr, xerr, yerrdo, yerrup)

###############################

c1.cd()
pad1.Draw()
pad1.cd()

h_bkg_err = h_bkg.Clone('h_bkg_err')
h_bkg_err.SetFillStyle(3008)
h_bkg_err.SetFillColor(ROOT.kAzure+2)
h_bkg_err.SetMarkerColor(ROOT.kAzure+2)
h_bkg_err.SetLineColor(ROOT.kAzure+2)
h_bkg_err.SetLineWidth(0)
h_bkg_err.SetMarkerSize(0)
h_bkg_err.SetMarkerStyle(0)

h_bkg_err.GetXaxis().SetTitleFont(textfont)
h_bkg_err.GetXaxis().SetTitleSize(0)
h_bkg_err.GetXaxis().SetLabelFont(textfont)
h_bkg_err.GetXaxis().SetLabelSize(0)
h_bkg_err.GetXaxis().SetTitleOffset(xoffset)
h_bkg_err.GetYaxis().SetTitleFont(textfont)
h_bkg_err.GetYaxis().SetTitleSize(titlesize)
h_bkg_err.GetYaxis().SetLabelFont(textfont)
h_bkg_err.GetYaxis().SetLabelSize(labelsize)

h_bkg_err.GetYaxis().SetTitleOffset(yoffset)

h_bkg_err.SetMinimum(0)
h_bkg_err.Draw('pe2')

h_bkg.SetLineColor(ROOT.kAzure+3)
h_bkg.SetFillColor(ROOT.kAzure+1)

h_bkg.Draw('hist same')
h_bkg_err.Draw('pe2 same')

# h_data.SetLineColor(ROOT.kBlack)
# h_data.SetMarkerColor(ROOT.kBlack)
# h_data.SetMarkerStyle(8)
# h_data.SetMarkerSize(0.8)
# h_data.Draw('pe same')

g_data.SetLineColor(ROOT.kBlack)
g_data.SetMarkerColor(ROOT.kBlack)
g_data.SetMarkerStyle(8)
g_data.SetMarkerSize(0.8)
g_data.Draw('pz same')

# h_bkg.Draw('hist pe2')

h_sig.SetLineColor(ROOT.kRed)
h_sig.SetLineWidth(2)
h_sig.Scale(sigscale)
h_sig.Draw('hist same')


if args.leg == 'left':
    leg = ROOT.TLegend(0.2, 0.6, 0.5, 0.88)
if args.leg == 'right':
    leg = ROOT.TLegend(0.2+0.38, 0.6, 0.5+0.38, 0.88)
leg.AddEntry(g_data, "Data", 'pe')
leg.AddEntry(h_bkg, "Bkg. model", 'f')
leg.AddEntry(h_bkg_err, "Bkg. unc.", 'f')
leg.AddEntry(h_sig, "SM ggF-HH #times {}".format(sigscale), 'l')
leg.SetFillStyle(0)
leg.SetBorderSize(0)
leg.Draw()

############################################################

c1.cd()
pad2.Draw()
pad2.cd()

hratio_err = h_bkg_err.Clone('hratio')
hratio_err.Divide(h_bkg)

hratio_err.GetYaxis().SetTitle('Obs./Exp.')

# make ratio with data
gratio_data = ROOT.TGraphAsymmErrors()
for i in range(h_bkg.GetNbinsX()):
    x   = h_data.GetBinCenter(i+1)
    bw  = h_data.GetBinLowEdge(i+2) - h_data.GetBinLowEdge(i+1)
    xerr = 0.5*bw
    if not adddatahbars:
        xerr = 0
    y   = h_data.GetBinContent(i+1)
    den = h_bkg.GetBinContent(i+1)
    
    yerrup = h_data.GetBinErrorUp(i+1)
    yerrdo = h_data.GetBinErrorLow(i+1)

    if den > 0:
        ipt = gratio_data.GetN()
        gratio_data.SetPoint(ipt, x, y/den)
        gratio_data.SetPointError(ipt, xerr, xerr, yerrdo/den, yerrup/den)

hratio_err.SetMinimum(ratiomin)
hratio_err.SetMaximum(ratiomax)

hratio_err.GetXaxis().SetTitleFont(textfont)
hratio_err.GetXaxis().SetTitleSize(titlesize)
hratio_err.GetXaxis().SetTitleOffset(xoffset)
hratio_err.GetXaxis().SetLabelFont(textfont)
hratio_err.GetXaxis().SetLabelSize(labelsize)
hratio_err.GetYaxis().SetTitleFont(textfont)
hratio_err.GetYaxis().SetTitleSize(titlesize)
hratio_err.GetYaxis().SetTitleOffset(yoffset)
hratio_err.GetYaxis().SetLabelFont(textfont)
hratio_err.GetYaxis().SetLabelSize(labelsize)

hratio_err.GetXaxis().SetTickSize(0.10)
hratio_err.GetYaxis().SetTickSize(0.05)



gratio_data.SetLineColor(ROOT.kBlack)
gratio_data.SetMarkerColor(ROOT.kBlack)
gratio_data.SetMarkerStyle(8)
gratio_data.SetMarkerSize(0.8)

hratio_err.Draw("pe2")
gratio_data.Draw('pz same')

########################################################
c1.cd()
pad1.cd()

# xpos = 0.177
xpos = 0.15
ypos = 0.995
CMSbox = ROOT.TLatex  (xpos, ypos , "CMS")
CMSbox.SetNDC()
CMSbox.SetTextSize(0.05)
CMSbox.SetTextFont(61)
CMSbox.SetTextColor(ROOT.kBlack)
CMSbox.SetTextAlign(13) ## inside the frame

## preliminary
# prelimBox = ROOT.TLatex  (xpos, ypos - 0.06 , "Simulation Preliminary")
# prelimBox = ROOT.TLatex  (xpos, ypos - 0.06 , "Preliminary")
prelimBox = ROOT.TLatex  (xpos + 0.09, ypos-0.01, "Preliminary")
prelimBox.SetNDC()
prelimBox.SetTextSize(0.76 *0.05)
prelimBox.SetTextFont(52)
prelimBox.SetTextColor(ROOT.kBlack)
prelimBox.SetTextAlign(13)

lumibox = ROOT.TLatex  (0.9, 0.955, "138 fb^{-1} (13 TeV)")
lumibox.SetNDC()
lumibox.SetTextAlign(31)
lumibox.SetTextSize( 0.76 *0.05)
lumibox.SetTextFont(42)
lumibox.SetTextColor(ROOT.kBlack)

CMSbox.Draw()
prelimBox.Draw()
lumibox.Draw()
########################################################

c1.Update()

c1.Print(args.oname, 'pdf')
