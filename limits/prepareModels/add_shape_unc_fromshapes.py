import ROOT
import sys
import os

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
shapefile = '/uscms/home/guerrero/nobackup/Run2/HH2020/Spring/CMSSW_10_2_13/src/bbbbAnalysis/MyLimitsHistos/Histos{}/outPlotter.root'.format(year)
print '... shape file  : ',shapefile
fInshape  = ROOT.TFile.Open(shapefile)
fOut      = ROOT.TFile.Open(foname, 'recreate')

if categ   == 'GGFcateg1': variable = 'GGFMVA1'
elif categ == 'GGFcateg2': variable = 'GGFMVA2'
elif categ == 'VBFcateg1': variable = 'HH_m_1'
else: print "No shape is found in category: ",categ

nproc = 0
for key in fIn.GetListOfKeys():
    # print key
    nproc += 1
    h = fIn.Get(key.GetName())
    fOut.cd()
    h.Write()
    if h.GetName() == 'MODEL':
      if categ == 'GGFcateg1' or categ == 'GGFcateg2' or categ == 'VBFcateg1':    
          h1  = fInshape.Get("MODEL_Shapeup/Btag4_%s_SR_110_Histogram/MODEL_Shapeup_Btag4_%s_SR_110_Histogram_%s"%(categ,categ,variable) )
          h2  = fInshape.Get("MODEL_Shapedown/Btag4_%s_SR_110_Histogram/MODEL_Shapedown_Btag4_%s_SR_110_Histogram_%s"%(categ,categ,variable) )
          hup = h1.Clone('MODEL_CMS_bkgshape_{}_{}Up'.format(categ,year))
          hdo = h2.Clone('MODEL_CMS_bkgshape_{}_{}Down'.format(categ,year))
          fOut.cd()
          hup.Write()
          hdo.Write()