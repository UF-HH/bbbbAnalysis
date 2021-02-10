import ROOT
import sys
import os

def apply_shift(h, f, uncname):
    hup = h.Clone(h.GetName() + '_{}Up'.format(uncname))
    hdo = h.Clone(h.GetName() + '_{}Down'.format(uncname))

    A = f.GetParameter(0)
    B = f.GetParameter(1)
    eA = f.GetParError(0)
    eB = f.GetParError(1)
    print 'f = A + Bx'
    print 'A = {} +/- {}'.format(A, eA)
    print 'B = {} +/- {}'.format(B, eB)
    
    f.SetParameters(A, B+eB)
    for ibin in range(1, h.GetNbinsX()+1):
        c = h.GetBinCenter(ibin)
        r = f.Eval(c)
        # print '... {} --> {}'.format(ibin, r)
        hup.SetBinContent(ibin, r*hup.GetBinContent(ibin))

    f.SetParameters(A, B-eB)
    for ibin in range(1, h.GetNbinsX()+1):
        c = h.GetBinCenter(ibin)
        r = f.Eval(c)
        # print '... {} --> {}'.format(ibin, r)
        hdo.SetBinContent(ibin, r*hdo.GetBinContent(ibin))

    ## ensure the normalization
    norm = h.Integral()
    print '[INFO] : setting normalization to the nominal one:', norm
    hup.Scale(norm/hup.Integral())
    hdo.Scale(norm/hdo.Integral())

    return hup, hdo

infile = sys.argv[1]
year   = int(sys.argv[2])
categ  = sys.argv[3]

fbase  = os.path.basename(infile)
fpath  = os.path.dirname(infile)
foname = fpath + '/' + fbase.replace('_nobkgshapeunc.root','.root')

print '... running on  : ', infile
print '... year        : ', year
print '... category    : ', categ
print '... output file : ', foname

fIn  = ROOT.TFile.Open(infile)
fOut = ROOT.TFile.Open(foname, 'recreate')

fIn.cd()

## A + Bx
# funcfile = {
#     'GGFcateg1' : '/uscms/home/lcadamur/nobackup/bbbbAnalysis_btagSF/CMSSW_10_2_5/src/bbbbAnalysis/studies/bkgShapeSyst/fitfuncs_{}_GGFcat1.root'.format(year),
#     'GGFcateg2' : '/uscms/home/lcadamur/nobackup/bbbbAnalysis_btagSF/CMSSW_10_2_5/src/bbbbAnalysis/studies/bkgShapeSyst/fitfuncs_{}_GGFcat2.root'.format(year),
#     'VBFcateg1' : '/uscms/home/lcadamur/nobackup/bbbbAnalysis_btagSF/CMSSW_10_2_5/src/bbbbAnalysis/studies/bkgShapeSyst/fitfuncs_{}_VBFcat1.root'.format(year),
# }

## 1 + Bx
funcfile = {
    'GGFcateg1' : '/uscms/home/lcadamur/nobackup/bbbbAnalysis_btagSF/CMSSW_10_2_5/src/bbbbAnalysis/studies/bkgShapeSyst/fixedA_fitfuncs_{}_GGFcat1.root'.format(year),
    'GGFcateg2' : '/uscms/home/lcadamur/nobackup/bbbbAnalysis_btagSF/CMSSW_10_2_5/src/bbbbAnalysis/studies/bkgShapeSyst/fixedA_fitfuncs_{}_GGFcat2.root'.format(year),
    'VBFcateg1' : '/uscms/home/lcadamur/nobackup/bbbbAnalysis_btagSF/CMSSW_10_2_5/src/bbbbAnalysis/studies/bkgShapeSyst/fixedA_fitfuncs_{}_VBFcat1.root'.format(year),
}

# # ## 1 + Bx - reversed fit
# funcfile = {
#     'GGFcateg1' : '/uscms/home/lcadamur/nobackup/bbbbAnalysis_btagSF/CMSSW_10_2_5/src/bbbbAnalysis/studies/bkgShapeSyst/reversed_fixedA_fitfuncs_{}_GGFcat1.root'.format(year),
#     'GGFcateg2' : '/uscms/home/lcadamur/nobackup/bbbbAnalysis_btagSF/CMSSW_10_2_5/src/bbbbAnalysis/studies/bkgShapeSyst/reversed_fixedA_fitfuncs_{}_GGFcat2.root'.format(year),
#     'VBFcateg1' : '/uscms/home/lcadamur/nobackup/bbbbAnalysis_btagSF/CMSSW_10_2_5/src/bbbbAnalysis/studies/bkgShapeSyst/reversed_fixedA_fitfuncs_{}_VBFcat1.root'.format(year),
# }

## A + Bx - reversed fit
# funcfile = {
#     'GGFcateg1' : '/uscms/home/lcadamur/nobackup/bbbbAnalysis_btagSF/CMSSW_10_2_5/src/bbbbAnalysis/studies/bkgShapeSyst/reversed_fitfuncs_{}_GGFcat1.root'.format(year),
#     'GGFcateg2' : '/uscms/home/lcadamur/nobackup/bbbbAnalysis_btagSF/CMSSW_10_2_5/src/bbbbAnalysis/studies/bkgShapeSyst/reversed_fitfuncs_{}_GGFcat2.root'.format(year),
#     'VBFcateg1' : '/uscms/home/lcadamur/nobackup/bbbbAnalysis_btagSF/CMSSW_10_2_5/src/bbbbAnalysis/studies/bkgShapeSyst/reversed_fitfuncs_{}_VBFcat1.root'.format(year),
# }


print '... func file   : ', funcfile[categ]
fInFunc = ROOT.TFile.Open(funcfile[categ])
ffit = fInFunc.Get('ffit')
nproc = 0
for key in fIn.GetListOfKeys():
    # print key
    nproc += 1
    h = fIn.Get(key.GetName())
    fOut.cd()
    h.Write()
    if h.GetName() == 'MODEL':
        hup, hdo = apply_shift(h, ffit, 'CMS_bkgshape_{}_{}'.format(categ,year))
        hup.Write()
        hdo.Write()