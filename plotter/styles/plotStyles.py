import ROOT

titles = {
    'MODEL'   : 'Bkg. model',
    'QCD'   : 'Multijet',
    'TT'    : "t#bar{t}",
    'ZZTo4B': "ZZ#rightarrow4b",
    'VBF_HH'     : "VBF-HH x 10",
    'VBF_1_2_1'  : "VBF-HH (c2v=2) x10",
    'GGHH4B_rew_kl_p_1'     : "GGF-HH x 10",
    'GGF_HH'     : "GGF-HH x 10",
    'GGHH4B_rew_kl_m_5'     : "GGF-HH (k#lambda=-5) x 10",
}

#########################################################################################

## 1 : solid, 7 : dashed
linestyles = {
}

#########################################################################################

linecolors = {
    'TT'          : ROOT.kGreen+3,
    'ZZTo4B'      : ROOT.kAzure+3,
    'QCD'         : 94,
    'MODEL'       : ROOT.kAzure+3,
    'VBF_HH'                 : ROOT.kBlue+1,
    'VBF_1_2_1'              : ROOT.kOrange+2,
    'GGHH4B_rew_kl_p_1'      : ROOT.kRed,
    'GGF_HH'      : ROOT.kRed,
    'GGHH4B_rew_kl_m_5'      : ROOT.kGreen+3,
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
    'ZZTo4B' : ROOT.kAzure+2,
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
