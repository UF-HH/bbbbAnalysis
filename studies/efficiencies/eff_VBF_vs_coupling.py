import effutils.EffUtilsNew as eut
import effutils.HHscalings  as hhs
import ROOT
import numpy as np
import sys
import sympy

ROOT.gROOT.SetBatch(True)

couplingname = 'kl' # the coupling to make eff vs : CV, C2V, kl - note : others are fixed to 1
year         = 2018

if len(sys.argv) > 1:
    couplingname = sys.argv[1]
if len(sys.argv) > 2:
    year = int(sys.argv[2])

print "... coupling name:", couplingname
print '... doing year :' , year

oname        = 'eff_%i_VBF_vs_%s.pdf' % (year, couplingname)
title        = 'VBF signal, %i simulation' % year
folder_in    = '/uscms/home/guerrero/nobackup/Run2/HH2020/Spring/CMSSW_10_2_13/src/bbbbAnalysis/MyDataMCHistos/DataMCHistos%i' % year
fin          = '%s/outPlotter.root' % folder_in
file_in      = ROOT.TFile.Open(fin)
cfg_in       = folder_in + '/' + eut.findInFolder(folder_in, 'plotter_*.cfg')
print "[INFO] : input file is  ", fin
print "[INFO] : config file is ", cfg_in

# logy = False
# ymax = 1.1
# ymin = 0

logy = True
ymax = 5
ymin = 1.e-3


## these samples are added to the plots if required
## empty list for none
single_pts_list = [
    {
        'kl'       : 1,
        'CV'       : 1,
        'C2V'       : 1,
        'rootfile' : file_in,
        'name'     : 'qqHH_CV_1_C2V_1_kl_1_%i' % year,
        'cfg'      : cfg_in,
    },
    {
        'kl'       : 1,
        'CV'       : 1,
        'C2V'      : 2,
        'rootfile' : file_in,
        'name'     : 'qqHH_CV_1_C2V_2_kl_1_%i' % year,
        'cfg'      : cfg_in,
    },
    {
        'kl'       : 0,
        'CV'       : 1,
        'C2V'      : 1,
        'rootfile' : file_in,
        'name'     : 'qqHH_CV_1_C2V_1_kl_0_%i' % year,
        'cfg'      : cfg_in,
    },
    {
        'kl'       : 2,
        'CV'       : 1,
        'C2V'      : 1,
        'rootfile' : file_in,
        'name'     : 'qqHH_CV_1_C2V_1_kl_2_%i' % year,
        'cfg'      : cfg_in,
    },
    {
        'kl'       : 1,
        'CV'       : 0.5,
        'C2V'      : 1,
        'rootfile' : file_in,
        'name'     : 'qqHH_CV_0p5_C2V_1_kl_1_%i' % year,
        'cfg'      : cfg_in,
    },
    {
        'kl'       : 1,
        'CV'       : 1.5,
        'C2V'      : 1,
        'rootfile' : file_in,
        'name'     : 'qqHH_CV_1p5_C2V_1_kl_1_%i' % year,
        'cfg'      : cfg_in,
    },
]

input_samples = [
    eut.effReader(file_in, 'qqHH_CV_1_C2V_1_kl_1_%i' % year),
    eut.effReader(file_in, 'qqHH_CV_1_C2V_2_kl_1_%i' % year),
    eut.effReader(file_in, 'qqHH_CV_1_C2V_1_kl_0_%i' % year),
    eut.effReader(file_in, 'qqHH_CV_1_C2V_1_kl_2_%i' % year),
    eut.effReader(file_in, 'qqHH_CV_0p5_C2V_1_kl_1_%i' % year),
    eut.effReader(file_in, 'qqHH_CV_1p5_C2V_1_kl_1_%i' % year),
]

input_samples_pts = [eut.effReader(x['rootfile'], x['name']) for x in single_pts_list]

## scale by lumi and xs:
for e in input_samples:
    e.correctForXS(cfg_in)

for i in range(len(input_samples_pts)):
    input_samples_pts[i].correctForXS(single_pts_list[i]['cfg'])

## NOTE : the xs is actually taken from the files, not from val_xs
# VBF : val_CV, val_C2V, val_kl
VBF_sample_list = [
    hhs.VBFHHSample(1,1,1,   val_xs = 0.00054/(0.3364), label = 'qqHH_CV_1_C2V_1_kl_1_%i' % year  ),
    hhs.VBFHHSample(1,2,1,   val_xs = 0.00472/(0.3364), label = 'qqHH_CV_1_C2V_2_kl_1_%i' % year  ),
    hhs.VBFHHSample(1,1,2,   val_xs = 0.00044/(0.3364), label = 'qqHH_CV_1_C2V_1_kl_2_%i' % year  ),
    hhs.VBFHHSample(1,1,0,   val_xs = 0.00145/(0.3364), label = 'qqHH_CV_1_C2V_1_kl_0_%i' % year  ),
    hhs.VBFHHSample(0.5,1,1, val_xs = 0.00353/(0.3364), label = 'qqHH_CV_0p5_C2V_1_kl_1_%i' % year),
    hhs.VBFHHSample(1.5,1,1, val_xs = 0.02149/(0.3364), label = 'qqHH_CV_1p5_C2V_1_kl_1_%i' % year),
]

scales = hhs.VBFHHFormula(VBF_sample_list)

## now match the input samples to the sample list - use the name
### function has a 1-to-1 correspondence to *input_samples*
functions = []
for i in range(len(input_samples)):
    n1 = input_samples[i].process
    for j in range(len(VBF_sample_list)):
        n2 = VBF_sample_list[j].label
        if (n1 == n2):
            functions.append(scales.coeffs[j])
            break

if len(functions) != len(input_samples):
    raise RuntimeError("cannot match inputs to scalings")

functions_lam = [sympy.utilities.lambdify((sympy.Symbol('CV'), sympy.Symbol('C2V'), sympy.Symbol('kl')), fdesc) for fdesc in functions]

#########################################################################

# define the plot content

to_plot = [
    'trigger',
    'presel',
    '4b',
    'vbf',
    'sr',
]

### maps the things to plot to the names they have
entries = {
    'trigger' : 'Ntrg_w',
    'presel'  : 'Nsel_w',
    '4b'      : 'Btag4_Histogram',
    'vbf'     : 'Btag4_VBFcateg_Histogram',
    'sr'      : 'Btag4_VBFcateg_SR_110_Histogram',
}

colors = {
    'trigger' : ROOT.kBlack,
    'presel'  : ROOT.kGreen+1,
    '4b'      : ROOT.kRed,
    'vbf'     : ROOT.kBlue+1,
    'sr'      : ROOT.kOrange+1
}

legends = {
    'trigger' : 'Trigger',
    'presel'  : 'Preselection (4 jet, 3 b)',
    '4b'      : '4 b-tagged jets',
    'vbf'     : 'VBF category',
    'sr'      : 'Signal region',
}

### change here to vary the coupling plotted

if couplingname == 'CV':
    xtitle       = 'C_{V}'
    npts = 101
    kls  = np.ones(npts)
    CVs  = np.linspace(-5, 5, npts)
    C2Vs = np.ones(npts)
    icoupling_of_interest = 0 ## index in the ZIP function below

if couplingname == 'C2V':
    xtitle       = 'C_{2V}'
    npts = 101
    kls  = np.ones(npts)
    CVs  = np.ones(npts)
    C2Vs = np.linspace(-5, 5, npts)
    icoupling_of_interest = 1 ## index in the ZIP function below

if couplingname == 'kl':
    xtitle       = '#kappa_{#lambda}'
    couplingname = 'kl'
    npts = 101
    kls  = np.linspace(-20, 20, npts)
    CVs  = np.ones(npts)
    C2Vs = np.ones(npts)
    icoupling_of_interest = 2 ## index in the ZIP function below

couplings = zip(CVs, C2Vs, kls)
print "CV, C2V, kl"
print '   ->', couplings[0]
print '   ->', couplings[1]
print '   -> [...]'
print '   ->', couplings[-2]
print '   ->', couplings[-1]

#########################################################################
# make the plot

graphs = {}
for tp in to_plot:
    graphs[tp] = ROOT.TGraph()

for ipt, pt in enumerate(couplings):
    if ipt % 10 == 0 : print '... doing ', pt
    # scales = [functions[isample].evalf(subs = {'CV' : pt[0], 'C2V' : pt[1], 'kl' : pt[2]}) for isample in range(len(input_samples))]
    scales = [functions_lam[isample](pt[0], pt[1], pt[2]) for isample in range(len(input_samples))]
    for tp in to_plot:
        ntot  = 0
        npass = 0
        for isample, sample in enumerate(input_samples):
            nthis_tot     = sample.counts[sample.tot_w_name] * scales[isample]
            nthis_pass    = sample.counts[entries[tp]]       * scales[isample]
            ntot  = ntot + nthis_tot
            npass = npass + nthis_pass

            # print sample.process
            # print 1.*nthis_pass/nthis_tot
            # print 1.*sample.counts[entries[tp]]/sample.counts[sample.tot_w_name]
            # print nthis_tot,  sample.counts[sample.tot_w_name], scales[isample]
            # print nthis_pass, sample.counts[entries[tp]], scales[isample]
            # print sample.get_eff(entries[tp])

        g = graphs[tp]
        g.SetPoint(g.GetN(), pt[icoupling_of_interest], 1.*npass/ntot)

# ####### the check points

single_points_grs = {}
for tp in to_plot:
    single_points_grs[tp] = ROOT.TGraph()

for ipt, pt in enumerate(single_pts_list):
    kl  = pt['kl']
    CV  = pt['CV'] 
    C2V = pt['C2V']

    ## check which couplings were fixed and which were varied
    kl_fixed = False
    CV_fixed = False
    C2V_fixed = False

    if len(set(kls))  == 1: kl_fixed = True
    if len(set(CVs))  == 1: CV_fixed = True
    if len(set(C2Vs)) == 1: C2V_fixed = True

    kl_ref  = -1 if not kl_fixed else kls[0]
    CV_ref  = -1 if not CV_fixed else CVs[0]
    C2V_ref = -1 if not C2V_fixed else C2Vs[0]

    ## reject points that do not belong to this scal
    if kl_fixed and kl != kl_ref:
        continue
    if CV_fixed and CV != CV_ref:
        continue
    if C2V_fixed and C2V != C2V_ref:
        continue

    for tp in to_plot:
        g = single_points_grs[tp]
        eff = input_samples_pts[ipt].get_eff(entries[tp])
        
        xcoupling = None
        if not kl_fixed:
            xcoupling = kl
        if not CV_fixed:
            xcoupling = CV
        if not C2V_fixed:
            xcoupling = C2V

        g.SetPoint(g.GetN(), xcoupling, eff)

        if kl == 1 and CV == 1 and C2V == 1: # printout
            print "... SM : Step : {:<20} - {:<40} : eff : {:.2f}%".format(tp, legends[tp], 100.*eff) 


########################################################################
# set the styles

for tp in to_plot:
    graphs[tp].SetLineColor(colors[tp])
    graphs[tp].SetMarkerColor(colors[tp])
    graphs[tp].SetMarkerStyle(8)
    graphs[tp].SetMarkerSize(0.4)

    if tp in single_points_grs : 
        single_points_grs[tp].SetLineColor(colors[tp])
        single_points_grs[tp].SetMarkerColor(colors[tp])
        # single_points_grs[tp].SetMarkerStyle(4) 
        # single_points_grs[tp].SetMarkerSize(0.8)
        single_points_grs[tp].SetMarkerStyle(5)
        single_points_grs[tp].SetMarkerSize(1.5)

mg = ROOT.TMultiGraph()
for tp in to_plot:
    mg.Add(graphs[tp], 'lp')
    if tp in single_points_grs : mg.Add(single_points_grs[tp], 'p')

c1 = ROOT.TCanvas('c1', 'c1', 600, 600)
c1.SetFrameLineWidth(3)
c1.SetLeftMargin(0.13)
c1.SetBottomMargin(0.13)
c1.SetLogy(logy)
mg.Draw('APL')

# mg.SetMinimum(2e-3)
mg.SetMaximum(ymax)
mg.SetMinimum(ymin)
mg.SetTitle('%s;%s;#varepsilon' % (title, xtitle))
mg.GetXaxis().SetTitleSize(0.055)
mg.GetYaxis().SetTitleSize(0.055)
mg.GetXaxis().SetLabelSize(0.045)
mg.GetYaxis().SetLabelSize(0.045)
mg.GetXaxis().SetTitleOffset(1.08)
mg.GetYaxis().SetTitleOffset(1.08)
mg.Draw('APL')

leg = ROOT.TLegend(0.5, 0.7, 0.88, 0.88)
for tp in to_plot:
    leg.AddEntry(graphs[tp], legends[tp], "pl")
leg.SetFillStyle(0)
leg.SetBorderSize(0)
leg.Draw()
c1.Print(oname, 'pdf')


