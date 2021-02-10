import ROOT
ROOT.gROOT.SetBatch(True)
ROOT.gStyle.SetOptStat(0)
import sys

# for c in GGFcat1 GGFcat2 VBFcat1 ; do for y in 2016 20172018; do python make_ratio_plot.py $y $c ; done ; done

def make_histogram(tIn, expr, cut, hname, bounds, is2D=False):
    hformat = 'h (%s)' % bounds
    dropt = '' if not is2D else 'colz'
    tIn.Draw(expr + ' >> ' + hformat, cut, dropt) ## note: if using goff, I can't retrieve the histo
    myh = ROOT.gPad.GetPrimitive("h");
    out_h = myh.Clone(hname)
    out_h.SetDirectory(0)
    out_h.Sumw2()
    return out_h

def set_style(h, col, mstyle=8, msize = 0.5, lw=1, fstyle=None):
    h.SetLineColor(col)
    h.SetMarkerColor(col)
    h.SetMarkerStyle(mstyle)
    h.SetMarkerSize(msize)
    h.SetLineWidth(lw)
    if fstyle:
        h.SetFillStyle(1)
        h.SetFillColor(col)

year     = 20172018
category = 'inclusive'
fixpar     = False
reversefit = False


if len(sys.argv) > 1:
    year = int(sys.argv[1])
if len(sys.argv) > 2:
    category = sys.argv[2]

variables = {
    'GGFcat1' : 'GGFMVA1V', 
    'GGFcat2' : 'GGFMVA2V', 
    'VBFcat1' : 'HH_m_1', 
}

bounds = {
    'GGFcat1' : '30, 0, 1', 
    'GGFcat2' : '30, 0, 1', 
    'VBFcat1' : '30, 200, 1800', 
}

bdtws = {
    'GGFcat1' : 'Weight_ValGGF1_New', 
    'GGFcat2' : 'Weight_ValGGF2_New', 
    'VBFcat1' : 'Weight_ValVBF1',     
}

CR_110 = 'TMath::Sqrt((H1_m-125)*(H1_m-125)+(H2_m-120)*(H2_m-120)) >= 25 && TMath::Sqrt((H1_m-125)*(H1_m-125)+(H2_m-120)*(H2_m-120)) <= 50'
SR_110 = 'TMath::Sqrt((H1_m-125)*(H1_m-125)+(H2_m-120)*(H2_m-120)) < 25'
CR_210 = 'TMath::Sqrt((H1_m-179)*(H1_m-179)+(H2_m-172)*(H2_m-172)) >= 25 && TMath::Sqrt((H1_m-179)*(H1_m-179)+(H2_m-172)*(H2_m-172)) <= 50'
SR_210 = 'TMath::Sqrt((H1_m-179)*(H1_m-179)+(H2_m-172)*(H2_m-172)) < 25'
all_110 = 'TMath::Sqrt((H1_m-125)*(H1_m-125)+(H2_m-120)*(H2_m-120)) <= 50'
all_210 = 'TMath::Sqrt((H1_m-179)*(H1_m-179)+(H2_m-172)*(H2_m-172)) <= 50'

VBFcateg   = '(GGFKiller>=0.5 && VBFEvent==1)'
GGFcateg   = '!(GGFKiller>=0.5 && VBFEvent==1)'
VBFcut1    = 'GGFKiller <  0.970'
VBFcut2    = 'GGFKiller >= 0.970'
GGFcut1    = 'HH_m < 450'
GGFcut2    = '!(HH_m < 450)'

GGFcat1 = '({}) && ({})'.format(GGFcateg, GGFcut1)
GGFcat2 = '({}) && ({})'.format(GGFcateg, GGFcut2)
VBFcat1 = '({}) && ({})'.format(VBFcateg, VBFcut1)
VBFcat2 = '({}) && ({})'.format(VBFcateg, VBFcut2)

### full kin selection + SR210

selection = {
    'GGFcat1' : '({}) && ({})'.format(GGFcat1, SR_210),
    'GGFcat2' : '({}) && ({})'.format(GGFcat2, SR_210),
    'VBFcat1' : '({}) && ({})'.format(VBFcat1, SR_210),
}

title = {
    'GGFcat1' : 'GGF cat. 1',
    'GGFcat2' : 'GGF cat. 2',
    'VBFcat1' : 'VBF cat. 1',
}

xtitle = {
    'GGFcat1' : 'Shifted m_{H} BDT output',
    'GGFcat2' : 'Shifted m_{H} BDT output',
    'VBFcat1' : 'm_{HH} [GeV]',
}


# fname = '../../mlskim/outputskims/skims19Gen2020/{}/SKIM_BKG_MODEL.root'.format(year)
fname = '/eos/uscms/store/user/guerrero/bbbb_ntuples/outputskims/NtuplesPA/{}/SKIM_BKG_MODEL.root'.format(year)

print '... year is            : ', year
print '... category is        : ', category
print '... will plot variable : ', variables[category]
print '... opening file       : ', fname
print '... will fix a in fit  : ', fixpar
# print '... will use selection : ', selection[category]

sel_3b = '{} * (({}) && ({}))'.format(bdtws[category], selection[category], 'nBtag==3')
sel_4b = '({}) && ({})'.format(selection[category], 'nBtag==4')

print "\nsel_3b\n", sel_3b
print '\nsel_4b\n', sel_4b
print ''

fIn = ROOT.TFile.Open(fname)
tIn = fIn.Get('bbbbTree')

c1 = ROOT.TCanvas('c1', 'c1', 600, 1500)
c1.Divide(1,4)
c1.SetFrameLineWidth(3)

c1.cd(1)
h_data  = make_histogram(tIn, variables[category], sel_4b, 'h_data',  bounds[category])
h_model = make_histogram(tIn, variables[category], sel_3b, 'h_model', bounds[category])

# scale to same integral
h_data.Scale(1./h_data.Integral())
h_model.Scale(1./h_model.Integral())

set_style(h_data, ROOT.kBlack)
set_style(h_model, ROOT.kBlue, lw=2)

c1.cd(1)
h_model.SetTitle(title[category] + ' - linear scale;{};a.u.'.format(xtitle[category]))
h_model.Draw('hist')
h_data.Draw('pesame')

c1.cd(2)
h_model.SetTitle(title[category] + ' - log scale;{};a.u.'.format(xtitle[category]))
ROOT.gPad.SetLogy(True)
h_model.Draw('hist')
h_data.Draw('pesame')

c1.cd(3)
gratio = ROOT.TGraphAsymmErrors()
gratio.Divide(h_data, h_model, 'pois')
set_style(gratio, ROOT.kBlack)
gratio.SetTitle(title[category] + ' - ratio;{};Data/model'.format(xtitle[category]))
gratio.Draw('AP')

if reversefit:
    ffit = ROOT.TF1('ffit', '[0] + [1]*(1 - x)', int((bounds[category].split(',')[1]).strip()), int((bounds[category].split(',')[2]).strip()) )
else:
    ffit = ROOT.TF1('ffit', '[0] + [1]*x', int((bounds[category].split(',')[1]).strip()), int((bounds[category].split(',')[2]).strip()) )
ffit.SetParameters(1,0)
if fixpar:
    ffit.FixParameter(0, 1)
gratio.Fit('ffit')

A = ffit.GetParameter(0)
B = ffit.GetParameter(1)
eA = ffit.GetParError(0)
eB = ffit.GetParError(1)

ttext1 = ROOT.TLatex(0.15, 0.8, 'f = A + Bx')
ttext2 = ROOT.TLatex(0.15, 0.8-1.*0.05, 'A = {:.2e} #pm {:.2e}'.format(A, eA))
ttext3 = ROOT.TLatex(0.15, 0.8-2.*0.05, 'B = {:.2e} #pm {:.2e}'.format(B, eB))

texts = [ttext1, ttext2, ttext3]
for t in texts:
    t.SetNDC(True)
    t.SetTextFont(42)
    t.Draw()

c1.cd(4)
gratio2 = gratio.Clone('gratio2')
gratio2.GetYaxis().SetRangeUser(0, 2)
gratio2.SetTitle(title[category] + ' - ratio (zoom);{};Data/model'.format(xtitle[category]))
gratio2.Draw('AP')
for t in texts: t.Draw()

onamepdf = 'overlay_{}_{}.pdf'.format(year, category)
oname = 'fitfuncs_{}_{}.root'.format(year, category)
if fixpar:
    oname = 'fixedA_' + oname
    onamepdf = 'fixedA_' + onamepdf
if reversefit:
    oname = 'reversed_' + oname
    onamepdf = 'reversed_' + onamepdf

c1.Print(onamepdf, 'pdf')
fOut = ROOT.TFile.Open(oname, 'recreate')
fOut.cd()
h_model.Write()
h_data.Write()
gratio.SetName('gratio')
gratio.Write()
ffit.Write()