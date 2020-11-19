import ROOT
ROOT.gROOT.SetBatch(True)

import argparse
parser = argparse.ArgumentParser('command line options for trigger plots')
parser.add_argument('--year', dest='year', type=int, default=2016)

args = parser.parse_args()

print "... plots for year :", args.year

# fIn = ROOT.TFile('/uscms/home/fravera/nobackup/DiHiggs_v1/CMSSW_10_2_5/src/bbbbAnalysis/TriggerEfficiencies_MuonPt30_matched_TTBarCut.root')

##################################################################################

if args.year == 2016:
    # fIn = ROOT.TFile('inputs/TriggerEfficiencies_2016_TTBarCut.root')
    fIn = ROOT.TFile('inputs/TriggerEfficiency_Fit_2016.root')
    
    folder = 'plots_2016'
    eff_proto = '{sample}_{trigger}_Efficiency_{filter}'

    triggers = [
        'Double90Quad30',
        'Quad45',
        'And',
    ]

    filters = {
        'Double90Quad30' : [
            "L1filterHT",
            "QuadCentralJet30",
            "DoubleCentralJet90",
            "BTagCaloCSVp087Triple",
            "QuadPFCentralJetLooseID30",
            "DoublePFCentralJetLooseID90",
        ],
        'Quad45' : [
            "L1filterHT",
            "QuadCentralJet45",
            "BTagCaloCSVp087Triple",
            "QuadPFCentralJetLooseID45",
        ],
        'And' : [
            "L1filterQuad45HT",
            "QuadCentralJet45",
            "QuadPFCentralJetLooseID45",
        ]

    }

    xvars = {
        'Double90Quad30' : {
            "L1filterHT"                   : 'H_{T} [GeV]',
            "QuadCentralJet30"             : 'Fourth jet p_{T} [GeV]',
            "DoubleCentralJet90"           : 'Second jet p_{T} [GeV]',
            "BTagCaloCSVp087Triple"        : 'Deep flavour score',
            "QuadPFCentralJetLooseID30"    : 'Fourth jet p_{T} [GeV]',
            "DoublePFCentralJetLooseID90"  : 'Second jet p_{T} [GeV]',
        },
        'Quad45' : {
            "L1filterHT"                   : 'H_{T} [GeV]',
            "QuadCentralJet45"             : 'Fourth jet p_{T} [GeV]',
            "BTagCaloCSVp087Triple"        : 'Deep flavour score',
            "QuadPFCentralJetLooseID45"    : 'Fourth jet p_{T} [GeV]',
        },
        'And' : {
            "L1filterQuad45HT"             : 'H_{T} [GeV]',
            "QuadCentralJet45"             : 'Fourth jet p_{T} [GeV]',
            "QuadPFCentralJetLooseID45"    : 'Fourth jet p_{T} [GeV]',
        }

    }

    trgnames = {
        'Double90Quad30' : 'DoubleJet90_Double30_TripleBTagCSV',
        'Quad45'         : 'HLT_QuadJet45_TripleBTagCSV',
        'And'            : '2016 trigger AND',
    }

    filternames = {
        'Double90Quad30' : {
            "L1filterHT"                   : 'L1 seed',
            "QuadCentralJet30"             : 'QuadCentralJet30',
            "DoubleCentralJet90"           : 'DoubleCentralJet90',
            "BTagCaloCSVp087Triple"        : 'BTagCaloCSVp087Triple',
            "QuadPFCentralJetLooseID30"    : 'QuadPFCentralJetLooseID30',
            "DoublePFCentralJetLooseID90"  : 'DoublePFCentralJetLooseID90',
        },
        'Quad45' : {
            "L1filterHT"                   : 'L1 seed',
            "QuadCentralJet45"             : 'QuadCentralJet45',
            "BTagCaloCSVp087Triple"        : 'BTagCaloCSVp087Triple',
            "QuadPFCentralJetLooseID45"    : 'QuadPFCentralJetLooseID45',
        },
        'And' : {
            "L1filterQuad45HT"             : 'L1 seed',
            "QuadCentralJet45"             : 'QuadCentralJet45',
            "QuadPFCentralJetLooseID45"    : 'QuadPFCentralJetLooseID45',
        }
    }

##################################################################################


if args.year == 2017:
    # fIn = ROOT.TFile('inputs/TriggerEfficiencies_2017_TTBarCut.root')
    fIn = ROOT.TFile('inputs/TriggerEfficiency_Fit_2017.root')
    folder = 'plots_2017'
    eff_proto = '{sample}__Efficiency_{filter}'

    triggers = [
        'Quad_75_60_45_40_3b',
    ]

    filters = {
        'Quad_75_60_45_40_3b' : [
            "L1filterHT",                     
            "QuadCentralJet30",               
            "CaloQuadJet30HT300",             
            "BTagCaloCSVp05Double",           
            "PFCentralJetLooseIDQuad30",      
            "1PFCentralJetLooseID75",         
            "2PFCentralJetLooseID60",         
            "3PFCentralJetLooseID45",         
            "4PFCentralJetLooseID40",         
            "PFCentralJetsLooseIDQuad30HT300",
            "BTagPFCSVp070Triple",            
        ]
    }

    xvars = {
            'Quad_75_60_45_40_3b' : {
                "L1filterHT"                       : 'H_{T} [GeV]',                     
                "QuadCentralJet30"                 : 'Fourth jet p_{T} [GeV]',
                "CaloQuadJet30HT300"               : 'H_{T} [GeV]',
                "BTagCaloCSVp05Double"             : 'Deep flavour score',
                "PFCentralJetLooseIDQuad30"        : 'Fourth jet p_{T} [GeV]',
                "1PFCentralJetLooseID75"           : 'First jet p_{T} [GeV]',
                "2PFCentralJetLooseID60"           : 'Second jet p_{T} [GeV]',
                "3PFCentralJetLooseID45"           : 'Third jet p_{T} [GeV]',
                "4PFCentralJetLooseID40"           : 'Fourth jet p_{T} [GeV]',
                "PFCentralJetsLooseIDQuad30HT300"  : 'H_{T} [GeV]',
                "BTagPFCSVp070Triple"              : 'Deep flavour score',
            },        
    }

    trgnames = {
        'Quad_75_60_45_40_3b' : 'HLT_PFHT300PT30_QuadPFJet_75_60_45_40_TriplePFBTagCSV'
    }

    filternames = {
        'Quad_75_60_45_40_3b' : {
            "L1filterHT"                       : 'L1 seed',                     
            "QuadCentralJet30"                 : 'QuadCentralJet30',
            "CaloQuadJet30HT300"               : 'CaloQuadJet30HT300',
            "BTagCaloCSVp05Double"             : 'BTagCaloCSVp05Double',
            "PFCentralJetLooseIDQuad30"        : 'PFCentralJetLooseIDQuad30',
            "1PFCentralJetLooseID75"           : '1PFCentralJetLooseID75',
            "2PFCentralJetLooseID60"           : '2PFCentralJetLooseID60',
            "3PFCentralJetLooseID45"           : '3PFCentralJetLooseID45',
            "4PFCentralJetLooseID40"           : '4PFCentralJetLooseID40',
            "PFCentralJetsLooseIDQuad30HT300"  : 'PFCentralJetsLooseIDQuad30HT300',
            "BTagPFCSVp070Triple"              : 'BTagPFCSVp070Triple',
        }
    }

##################################################################################


if args.year == 2018:
    # fIn = ROOT.TFile('inputs/TriggerEfficiencies_2018_TTBarCut.root')
    fIn = ROOT.TFile('inputs/TriggerEfficiency_Fit_2018.root')
    folder = 'plots_2018'
    eff_proto = '{sample}__Efficiency_{filter}'

    triggers = [
        'Quad_75_60_45_40_3b',
    ]

    filters = {
        'Quad_75_60_45_40_3b' : [
            "L1filterHT",                     
            "QuadCentralJet30",               
            "CaloQuadJet30HT320",             
            "BTagCaloDeepCSVp17Double",           
            "PFCentralJetLooseIDQuad30",      
            "1PFCentralJetLooseID75",         
            "2PFCentralJetLooseID60",         
            "3PFCentralJetLooseID45",         
            "4PFCentralJetLooseID40",         
            "PFCentralJetsLooseIDQuad30HT330",
            "BTagPFDeepCSV4p5Triple",            
        ]
    }

    xvars = {
            'Quad_75_60_45_40_3b' : {
                "L1filterHT"                       : 'H_{T} [GeV]',                     
                "QuadCentralJet30"                 : 'Fourth jet p_{T} [GeV]',
                "CaloQuadJet30HT320"               : 'H_{T} [GeV]',
                "BTagCaloDeepCSVp17Double"         : 'Deep flavour score',
                "PFCentralJetLooseIDQuad30"        : 'Fourth jet p_{T} [GeV]',
                "1PFCentralJetLooseID75"           : 'First jet p_{T} [GeV]',
                "2PFCentralJetLooseID60"           : 'Second jet p_{T} [GeV]',
                "3PFCentralJetLooseID45"           : 'Third jet p_{T} [GeV]',
                "4PFCentralJetLooseID40"           : 'Fourth jet p_{T} [GeV]',
                "PFCentralJetsLooseIDQuad30HT330"  : 'H_{T} [GeV]',
                "BTagPFDeepCSV4p5Triple"           : 'Deep flavour score',
            },        
    }

    trgnames = {
        'Quad_75_60_45_40_3b' : 'HLT PFHT330PT30 QuadPFJet 75 60 45 40 TriplePFBTagDeepCSV'
    }

    filternames = {
        'Quad_75_60_45_40_3b' : {
            "L1filterHT"                       : 'L1 seed',                     
            "QuadCentralJet30"                 : 'QuadCentralJet30',
            "CaloQuadJet30HT320"               : 'CaloQuadJet30HT320',
            "BTagCaloDeepCSVp17Double"         : 'BTagCaloDeepCSVp17Double',
            "PFCentralJetLooseIDQuad30"        : 'PFCentralJetLooseIDQuad30',
            "1PFCentralJetLooseID75"           : '1PFCentralJetLooseID75',
            "2PFCentralJetLooseID60"           : '2PFCentralJetLooseID60',
            "3PFCentralJetLooseID45"           : '3PFCentralJetLooseID45',
            "4PFCentralJetLooseID40"           : '4PFCentralJetLooseID40',
            "PFCentralJetsLooseIDQuad30HT330"  : 'PFCentralJetsLooseIDQuad30HT330',
            "BTagPFDeepCSV4p5Triple"           : 'BTagPFDeepCSV4p5Triple',
        }
    }



samples = [
    'SingleMuon',
    'TTbar',
    # 'NMSSM_XYH_bbbb',
]

colors = {
    'SingleMuon'     : ROOT.kBlack,
    'TTbar'          : ROOT.kRed,
    'NMSSM_XYH_bbbb' : ROOT.kGreen+1,
}

legends = {
    'SingleMuon'     : 'Single #mu data',
    'TTbar'          : 't#bar{t} MC',
    'NMSSM_XYH_bbbb' : 'Signal MC',    
}




# SingleMuon_Double90Quad30_Efficiency_QuadCentralJet30

out_proto = '{folder}/{trigger}_Efficiency_{filter}.pdf'

c1 = ROOT.TCanvas('c1', 'c1', 600, 600)
c1.SetBottomMargin(0.12)
c1.SetLeftMargin(0.12)
c1.SetFrameLineWidth(3)

c1.SetGridx()
c1.SetGridy()

for tr in triggers:
    for fil in filters[tr]:
        graphs = {}
        mg = ROOT.TMultiGraph('mg', ';%s;Efficiency' % xvars[tr][fil])
        for sam in samples:
            g = fIn.Get(eff_proto.format(sample=sam, trigger=tr, filter=fil))
            graphs[sam] = g
            mg.Add(graphs[sam])

        # styles
        for sam, g in graphs.items():
            g.SetLineColor(colors[sam])
            g.SetMarkerColor(colors[sam])
            g.SetMarkerStyle(8)
            g.SetMarkerSize(0.8)

        leg = ROOT.TLegend(0.11, 0.92, 0.90, 0.99)
        leg.SetNColumns(len(samples))
        leg.SetFillStyle(0)
        leg.SetBorderSize(0)
        for sam in samples:
            leg.AddEntry(graphs[sam], legends[sam], 'PE')

        lab1 = ROOT.TLatex (0.37, 0.28, trgnames[tr])
        lab2 = ROOT.TLatex (0.37, 0.28-0.055, filternames[tr][fil])
        lab1.SetNDC()
        lab2.SetNDC()
        lab1.SetTextSize(0.03)
        lab2.SetTextSize(0.03)
        lab1.SetTextFont(42)
        lab2.SetTextFont(42)
        lab1.SetTextAlign(11)
        lab2.SetTextAlign(11)

        c1.cd()
        mg.Draw('AP')
        mg.SetMinimum(0)
        mg.SetMaximum(1.1)
        mg.GetXaxis().SetTitleSize(0.05)
        mg.GetXaxis().SetLabelSize(0.04)
        mg.GetYaxis().SetTitleSize(0.05)
        mg.GetYaxis().SetLabelSize(0.04)
        mg.GetXaxis().SetTitleOffset(1.07)
        mg.GetYaxis().SetTitleOffset(1.07)
        mg.Draw('AP')
        leg.Draw()
        lab1.Draw()
        lab2.Draw()

        c1.Print(out_proto.format(folder=folder, trigger=tr, filter=fil), 'pdf')