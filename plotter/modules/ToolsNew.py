### this are the Tools to handle the new output of the histo filler
### cfr. Tools.py define the tools to handle the output of evalQCD

import fnmatch
import os
import SampleHist

def findInFolder (folder, pattern):
    ll = []
    for ff in os.listdir(folder):
        if fnmatch.fnmatch(ff, pattern): ll.append(ff)
    if len (ll) == 0:
        print "*** WARNING: No valid config " , pattern , "found in " , folder
        return None
    if len (ll) > 1:
        print "*** WARNING: Too many configs found in " , folder , ", using first one : " , ll
    return ll[0]

# prototype: ZZTo2L2Q_defaultBtagLLNoIsoBBTTCut_SSrlx_HHsvfit_deltaPhi
# L.C changed on 05.03.2018: adapted to new naming scheme by Fabio:
# QCD_HT_1000_1500/selectionVBF_VBF/QCD_HT_1000_1500_selectionVBF_VBF_H1_b1_pt
def retrieveHisto (rootFile, name, var, sel):
    # fullName = name + '_' + sel + '_' + var
    vals = {
        'name' : name,
        'var'  : var,
        'sel'  : sel,
    }
    fullName = '{name}/{sel}/{name}_{sel}_{var}'.format(**vals)
    # if not rootFile.GetListOfKeys().Contains(fullName):
    #     print "*** WARNING: histo " , fullName , " not available"
    #     return None
    hFound = rootFile.Get(fullName)
    if not hFound:
        print "*** WARNING: histo " , fullName , " not available"
        return None
    return hFound

# tag = "sig", "bkg", "DATA"
def retrieveHistos (rootFile, namelist, var, sel):
    res = {}
    for name in namelist:
        theH = retrieveHisto(rootFile, name, var, sel)
        if not theH:
            continue
        # res[name] = SampleHist.SampleHist(name=name, inputH=theH)
        res[name] = theH
    return res

# def retrieveQCD (rootFile, var, sel, dataNameList):
#     # name = "CORR_DDQCD_SS_DATA_" + var + "_SS_" + sel + "_" + dataNameList[-1]
#     name = "DDQCD_SS_DATA_" + var + "_SS_" + sel + "_" + dataNameList[-1]
#     if not rootFile.GetListOfKeys().Contains(name):
#         print "*** WARNING: QCD histo " , name , " not available"
#         return None
#     hQCD = SampleHist.SampleHist(name="QCD", inputH=rootFile.Get(name))
#     # print "QCD yield: " , hQCD.Integral()

#     # if 2b0j_Mcut, replace with 1b1j_Mcut for the shape
#     # if False:
#     # if sel == "resolved_2b0j_Mcut":
#     #     print "*** taking 1b1j shape for " , sel
#     #     name = "DDQCD_SS_DATA_" + var + "_SS_" + "resolved_1b1j_Mcut" + "_" + dataNameList[-1]
#     #     if not rootFile.GetListOfKeys().Contains(name):
#     #         print "*** WARNING: QCD histo " , name , " not available"
#     #         return None
#     #     hQCD1b1j = rootFile.Get(name)
#     #     # print "QCD yield: " , hQCD.Integral()
#     #     hQCD1b1j.Scale (1.*hQCD.Integral()/hQCD1b1j.Integral())
#     #     return hQCD1b1j
#     return hQCD

