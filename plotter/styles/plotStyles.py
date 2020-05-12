import ROOT

titles = {
    'MODEL'                 : 'Bkg. model',
    'QCD'                   : 'Multijet',
    'TT'                    : "t#bar{t}",
    'SingleH'               : "Single H",    
    'ZZ_4B'                 : "ZZ#rightarrow4b",
    'ggHH_kl_1_kt_1'      : "ggF-HH",     
    'ggHH_kl_2p45_kt_1'   : "ggF-HH(kl=2.45)",   
    'ggHH_kl_5_kt_1'      : "ggF-HH(kl=5)",   
    'qqHH_CV_1_C2V_1_kl_1': "VBF-HH", 
    'qqHH_CV_1_C2V_2_kl_1': "VBF-HH(c2v=2)", 
}

#########################################################################################

## 1 : solid, 7 : dashed
linestyles = {
}

#########################################################################################

linecolors = {
    'TT'          : ROOT.kGreen+2,
    'SingleH'     : ROOT.kViolet-9,
    'ZZ_4B'       : ROOT.kGray,
    'QCD'         : 94,
    'MODEL'       : ROOT.kAzure+3,
    'ggHH_kl_1_kt_1'      : ROOT.kBlue+1,     
    'ggHH_kl_2p45_kt_1'   : ROOT.kOrange+2,   
    'ggHH_kl_5_kt_1'      : ROOT.kOrange+2,   
    'qqHH_CV_1_C2V_1_kl_1': ROOT.kBlack, 
    'qqHH_CV_1_C2V_2_kl_1': ROOT.kRed, 
}

## here are some colors for reference
# 'DY'      : 94,
# 'DY0b'    : 92,
# 'DY1b'    : 94,
# 'DY2b'    : 96,
# 'ZH'    : ROOT.TColor.GetColor('#0C6AA1'),
# 'VV'    : ROOT.kRed-6,
# 'WJets' : ROOT.kGray,
# 'QCD'   : 94,
# 'other' : ROOT.kRed-6,
# 'singleH' : ROOT.kRed-4,
# 'HH'    : ROOT.kBlue,
# 'Radion650' : 634,
# 'Radion750' : 926,

#########################################################################################

fillcolors = {
    'TT'    : 8,
    'MODEL' : ROOT.kAzure+1,
    'SingleH'     : ROOT.kViolet-9,
    'ZZ_4B' : ROOT.kGray,
    'QCD'    : 92,
    'QCD_HT_200_300' :   ROOT.kRed+1,
    'QCD_HT_300_500' :   ROOT.kRed+2,
    'QCD_HT_500_700' :   ROOT.kRed+3,
    'QCD_HT_700_1000' :  ROOT.kRed+4,
    'QCD_HT_1000_1500' : ROOT.kMagenta,
    'QCD_HT_1500_2000' : ROOT.kMagenta+1,
    'QCD_HT_2000_Inf' :  ROOT.kMagenta+2,
}

## here are some colors for reference
# 'singleH' : ROOT.kRed-7,
# 'QCD'   : 606,
# 'DY'    : 92,
# 'DY0b'    : 92,
# 'DY1b'    : 94,
# 'DY2b'    : 96,
# 'ZH'    : ROOT.TColor.GetColor('#64B6E4'),
# 'ttH'   : 2, 
# 'ZH'    : 3,
# 'WH'    : 4,
# 'VBFH'  : 5,
# 'ggH'   : 6,
# 'VV'    : ROOT.kRed-7,
# 'WJets' : ROOT.kGray,
