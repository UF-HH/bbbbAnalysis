import ROOT

titles = {
    'QCD'   : 'Multijet',
    'TT'    : "t#bar{t}",
    'VBF_HH'     : "VBF HH",
    # 'VBF_HH_BSM' : "VBF HH BSM (#kappa_{#lambda}=1, C_{V}=1, C_{2V}=2)",
    'VBF_HH_BSM' : "VBF HH BSM (C_{2V} = 2)",
    'GGF_HH'      : "gg#rightarrowHH SM",
}

#########################################################################################

## 1 : solid, 7 : dashed
linestyles = {
}

#########################################################################################

linecolors = {
    'TT'          : ROOT.kGreen+3,
    'QCD'         : 94,
    'VBF_HH'      : ROOT.kBlack,
    'VBF_HH_BSM'  : ROOT.kBlue+1,
    'GGF_HH'      : ROOT.kRed,
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
    'QCD'    : 92,
    'other' : ROOT.kRed-7,
    'QCD_HT_100_200' :   ROOT.kRed,
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
