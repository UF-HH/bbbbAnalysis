import ROOT
import sys

ROOT.gROOT.SetBatch(True)
ROOT.gStyle.SetOptStat(0)

def make_histogram(tIn, expr, cut, hname, nbins, xmin, xmax):
    hformat = 'h (%i, %f, %f)' % (nbins, xmin, xmax)
    tIn.Draw(expr + ' >> ' + hformat, cut)
    myh = ROOT.gPad.GetPrimitive("h");
    out_h = myh.Clone(hname)
    out_h.SetDirectory(0)
    return out_h


var = "H1_b2_pt"
cut = ""

xmin = 0
xmax = 400

# var = "H1_m"
# cut = ""

# xmin = 0
# xmax = 500


# logy = True

if len(sys.argv) > 1:
    var = sys.argv[1]

if len(sys.argv) > 3:
    xmin = float(sys.argv[2])
    xmax = float(sys.argv[3])


print "... var : ", var , "[", xmin, "," , xmax, "]"
print "... cut : ", cut

syst_list = [
    'AbsoluteMPFBias',
    'AbsoluteScale',
    'AbsoluteStat',
    'FlavorQCD',
    'Fragmentation',
    'PileUpDataMC',
    'PileUpPtBB',
    'PileUpPtEC1',
    'PileUpPtEC2',
    'PileUpPtHF',
    'PileUpPtRef',
    'RelativeFSR',
    'RelativeJEREC1',
    'RelativeJEREC2',
    'RelativeJERHF',
    'RelativePtBB',
    'RelativePtEC1',
    'RelativePtEC2',
    'RelativePtHF',
    'RelativeBal',
    'RelativeSample',
    'RelativeStatEC',
    'RelativeStatFSR',
    'RelativeStatHF',
    'SinglePionECAL',
    'SinglePionHCAL',
    'TimePtEta',
]

dirlist = ['up', 'down']

# plot_cmd_proto = '%s >> h_{idx} (100, {xmin}, {xmax})'



fname_proto = 'root://cmseos.fnal.gov//store/user/lcadamur/bbbb_ntuples/testJECsysts5/SKIM_GluGluToHHTo4B_node_SM_13TeV-madgraph_{syst}_{dir}/output/bbbbNtuple_0.root'

histos_up = []
histos_down = []

for syst in syst_list:
    for dirr in dirlist:
        print '... doing', syst, dirr
        file = ROOT.TFile.Open(fname_proto.format(syst=syst, dir=dirr))
        tree = file.Get('bbbbTree')
        h = make_histogram(tree, var, cut, 'h_{syst}_{dir}'.format(syst=syst, dir=dirr), 100, xmin, xmax)

        if dirr == 'up':
            h.SetLineColor(ROOT.kGreen+1)
            histos_up.append(h)
        else:
            h.SetLineColor(ROOT.kBlue)
            histos_down.append(h)

## now make the nominal one
file = ROOT.TFile.Open('root://cmseos.fnal.gov//store/user/lcadamur/bbbb_ntuples/testJECsysts5/SKIM_GluGluToHHTo4B_node_SM_13TeV-madgraph/output/bbbbNtuple_0.root')
tree = file.Get('bbbbTree')
ref  = make_histogram(tree, var, cut, 'h_{syst}_{dir}'.format(syst=syst, dir=dirr), 100, xmin, xmax)

c1 = ROOT.TCanvas('c1', 'c1', 600, 600)

ref.SetLineColor(ROOT.kRed)
ref.SetLineWidth(3)

ref.Draw()
for idx in range(len(syst_list)):
    histos_up[idx].Draw('same')
    histos_down[idx].Draw('same')

c1.Print('plots/JECcompare_%s.pdf' % var, 'pdf')

cloneref = ref.Clone('cloneref')

## now print the impact of each JEC
for i, syst in enumerate(syst_list):
    print '... plotting', syst
    h_v1 = histos_up[i] ## up
    h_v2 = histos_down[i] ## down
    cloneref.SetTitle(syst)
    cloneref.Draw()
    h_v1.Draw('same')
    h_v2.Draw('same')
    c1.Print('plots/comp_%s_%s.pdf' % (var, syst), 'pdf')
