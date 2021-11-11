# for y in 2016 20172018; do for c in GGFcateg1 GGFcateg2 VBFcateg1 VBFcateg2; do python plot_GoF.py $y $c ; done ; done
# python plot_GoF.py run2 comb_GGFcateg1GGFcateg2VBFcateg1VBFcateg2

import ROOT
import sys

ROOT.gROOT.SetBatch(True)
ROOT.gStyle.SetOptStat(0)

year  = sys.argv[1]
categ = sys.argv[2]
nametag = sys.argv[3] if len(sys.argv) > 3 else ''

print'... year', year
print'... categ', categ
print'... nametag', nametag

fol = "../physicsmodel{}".format(year)
obs_file = 'higgsCombine{}_{}{}.GoodnessOfFit.mH120.root'.format(year, categ, nametag)
toy_file = 'higgsCombine{}_{}{}.GoodnessOfFit.mH120.AllToyRuns.root'.format(year, categ, nametag)

oname = 'GoF_{}_{}.pdf'.format(year, categ)

print'... obs file', obs_file
print'... toy file', toy_file
print'... output name', oname

fIn_obs = ROOT.TFile.Open('{}/{}'.format(fol, obs_file))
fIn_toy = ROOT.TFile.Open('{}/{}'.format(fol, toy_file))

tIn_obs = fIn_obs.Get('limit')
tIn_toy = fIn_toy.Get('limit')

def extract_values (tree) :
    res = []
    print'[INFO] tree contains', tree.GetEntries(), 'entries'
    for i in range (tree.GetEntries()):
        tree.GetEntry(i)
        res.append(tree.limit)
    return res

print' >> reading obs'
obs = extract_values(tIn_obs)
if len(obs) != 1:
    raise RuntimeError('observed has %i values instead of 1' % len(obs))

print' >> reading toys'
toys = extract_values(tIn_toy)
print' .... found', len(toys), 'toys'

c1 = ROOT.TCanvas('c1', 'c1', 600, 600)
c1.SetFrameLineWidth(3)

mmin = min (toys+obs)
mmax = max (toys+obs)

h = ROOT.TH1D('h', 'Saturated GoF - {} - {};Best fit test statistic;Events'.format(year, categ), 50, 0.85*mmin, 1.15*mmax)
for t in toys:
    h.Fill(t)
h.SetLineWidth(2)
h.SetLineColor(ROOT.kBlack)

arr = ROOT.TArrow(obs[0], 0.75*h.GetMaximum(), obs[0], 0, 0.03, "|>")
arr.SetLineWidth(2)
arr.SetAngle(45)
arr.SetLineColor(ROOT.kBlue)
arr.SetFillColor(ROOT.kBlue)

def compute_prob(obs, toys):
    greater = [x for x in toys if x > obs]
    return 1.*len(greater)/len(toys)

ptext = ROOT.TLatex(0.6, 0.8, 'Probability : {:.1f}%'.format(100.*compute_prob(obs[0], toys)))
ptext.SetNDC(True)
ptext.SetTextFont(42)
ptext.SetTextSize(0.035)

h.Draw('hist')
arr.Draw()
ptext.Draw()

print'OBSERVED GOF', obs[0]

c1.Print(oname, 'pdf')
