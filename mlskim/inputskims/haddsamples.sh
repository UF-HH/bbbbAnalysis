tag=$1
##HADD DATA AND SM MC SAMPLES
mkdir $tag
mkdir $tag/New2016
rm $tag/New2016/*.root
hadd -f $tag/New2016/SKIM_Data.root                                                    `xrdfsls -u /eos/uscms/store/user/guerrero/bbbb_ntuples/$tag/New2016/SKIM_Data_BTagCSV/output/ | grep '\.root'`
hadd -f $tag/New2016/SKIM_QCD_HT200to300_TuneCUETP8M1_13TeV-madgraphMLM-pythia8.root   `xrdfsls -u /eos/uscms/store/user/guerrero/bbbb_ntuples/$tag/New2016/SKIM_QCD_HT200to300_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/output/ | grep '\.root'`
hadd -f $tag/New2016/SKIM_QCD_HT300to500_TuneCUETP8M1_13TeV-madgraphMLM-pythia8.root   `xrdfsls -u /eos/uscms/store/user/guerrero/bbbb_ntuples/$tag/New2016/SKIM_QCD_HT300to500_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/output/ | grep '\.root'`
hadd -f $tag/New2016/SKIM_QCD_HT500to700_TuneCUETP8M1_13TeV-madgraphMLM-pythia8.root   `xrdfsls -u /eos/uscms/store/user/guerrero/bbbb_ntuples/$tag/New2016/SKIM_QCD_HT500to700_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/output/ | grep '\.root'`
hadd -f $tag/New2016/SKIM_QCD_HT700to1000_TuneCUETP8M1_13TeV-madgraphMLM-pythia8.root  `xrdfsls -u /eos/uscms/store/user/guerrero/bbbb_ntuples/$tag/New2016/SKIM_QCD_HT700to1000_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/output/ | grep '\.root'`
hadd -f $tag/New2016/SKIM_QCD_HT1000to1500_TuneCUETP8M1_13TeV-madgraphMLM-pythia8.root `xrdfsls -u /eos/uscms/store/user/guerrero/bbbb_ntuples/$tag/New2016/SKIM_QCD_HT1000to1500_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/output/ | grep '\.root'`
hadd -f $tag/New2016/SKIM_QCD_HT1500to2000_TuneCUETP8M1_13TeV-madgraphMLM-pythia8.root `xrdfsls -u /eos/uscms/store/user/guerrero/bbbb_ntuples/$tag/New2016/SKIM_QCD_HT1500to2000_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/output/ | grep '\.root'`
hadd -f $tag/New2016/SKIM_QCD_HT2000toInf_TuneCUETP8M1_13TeV-madgraphMLM-pythia8.root  `xrdfsls -u /eos/uscms/store/user/guerrero/bbbb_ntuples/$tag/New2016/SKIM_QCD_HT2000toInf_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/output/ | grep '\.root'`
hadd -f $tag/New2016/SKIM_TT_TuneCUETP8M2T4_13TeV-powheg-pythia8.root                  `xrdfsls -u /eos/uscms/store/user/guerrero/bbbb_ntuples/$tag/New2016/SKIM_TT_TuneCUETP8M2T4_13TeV-powheg-pythia8/output/ | grep '\.root'`
hadd -f $tag/New2016/SKIM_VBFHHTo4B_CV_0_5_C2V_1_C3_1_13TeV-madgraph.root              `xrdfsls -u /eos/uscms/store/user/guerrero/bbbb_ntuples/$tag/New2016/SKIM_VBFHHTo4B_CV_0_5_C2V_1_C3_1_13TeV-madgraph//output/ | grep '\.root'`
hadd -f $tag/New2016/SKIM_VBFHHTo4B_CV_1_5_C2V_1_C3_1_13TeV-madgraph.root              `xrdfsls -u /eos/uscms/store/user/guerrero/bbbb_ntuples/$tag/New2016/SKIM_VBFHHTo4B_CV_1_5_C2V_1_C3_1_13TeV-madgraph//output/ | grep '\.root'`
hadd -f $tag/New2016/SKIM_VBFHHTo4B_CV_1_C2V_1_C3_0_13TeV-madgraph.root                `xrdfsls -u /eos/uscms/store/user/guerrero/bbbb_ntuples/$tag/New2016/SKIM_VBFHHTo4B_CV_1_C2V_1_C3_0_13TeV-madgraph//output/ | grep '\.root'`
hadd -f $tag/New2016/SKIM_VBFHHTo4B_CV_1_C2V_1_C3_2_13TeV-madgraph.root                `xrdfsls -u /eos/uscms/store/user/guerrero/bbbb_ntuples/$tag/New2016/SKIM_VBFHHTo4B_CV_1_C2V_1_C3_2_13TeV-madgraph//output/ | grep '\.root'`
hadd -f $tag/New2016/SKIM_VBFHHTo4B_CV_1_C2V_2_C3_1_13TeV-madgraph.root                `xrdfsls -u /eos/uscms/store/user/guerrero/bbbb_ntuples/$tag/New2016/SKIM_VBFHHTo4B_CV_1_C2V_2_C3_1_13TeV-madgraph/output/ | grep '\.root'`
hadd -f $tag/New2016/SKIM_VBFHHTo4B_CV_1_C2V_1_C3_1_13TeV-madgraph.root                `xrdfsls -u /eos/uscms/store/user/guerrero/bbbb_ntuples/$tag/New2016/SKIM_VBFHHTo4B_CV_1_C2V_1_C3_1_13TeV-madgraph/output/ | grep '\.root'`
hadd -f $tag/New2016/SKIM_GluGluToHHTo4B_node_SM_13TeV-madgraph.root                   `xrdfsls -u /eos/uscms/store/user/guerrero/bbbb_ntuples/$tag/New2016/SKIM_GluGluToHHTo4B_node_SM_13TeV-madgraph/output/ | grep '\.root'`
mkdir $tag
mkdir $tag/New2017
rm $tag/New2017/*.root
hadd -f $tag/New2017/SKIM_Data.root                                                       `xrdfsls -u /eos/uscms/store/user/guerrero/bbbb_ntuples/$tag/New2017/SKIM_Data_BTagCSV/output/ | grep '\.root'`
hadd -f $tag/New2017/SKIM_GluGluToHHTo4B_node_SM_13TeV-madgraph_correctedcfg.root         `xrdfsls -u /eos/uscms/store/user/guerrero/bbbb_ntuples/$tag/New2017/SKIM_GluGluToHHTo4B_node_SM_13TeV-madgraph_correctedcfg/output/ | grep '\.root'`
hadd -f $tag/New2017/SKIM_VBFHHTo4B_CV_1_C2V_1_C3_1_13TeV-madgraph.root                   `xrdfsls -u /eos/uscms/store/user/guerrero/bbbb_ntuples/$tag/New2017/SKIM_VBFHHTo4B_CV_1_C2V_1_C3_1_13TeV-madgraph/output/ | grep '\.root'`
hadd -f $tag/New2017/SKIM_VBFHHTo4B_CV_1_C2V_2_C3_1_13TeV-madgraph.root                   `xrdfsls -u /eos/uscms/store/user/guerrero/bbbb_ntuples/$tag/New2017/SKIM_VBFHHTo4B_CV_1_C2V_2_C3_1_13TeV-madgraph/output/ | grep '\.root'`
hadd -f $tag/New2017/SKIM_VBFHHTo4B_CV_1_5_C2V_1_C3_1_13TeV-madgraph.root                 `xrdfsls -u /eos/uscms/store/user/guerrero/bbbb_ntuples/$tag/New2017/SKIM_VBFHHTo4B_CV_1_5_C2V_1_C3_1_13TeV-madgraph//output/ | grep '\.root'`
hadd -f $tag/New2017/SKIM_VBFHHTo4B_CV_1_C2V_1_C3_0_13TeV-madgraph.root                   `xrdfsls -u /eos/uscms/store/user/guerrero/bbbb_ntuples/$tag/New2017/SKIM_VBFHHTo4B_CV_1_C2V_1_C3_0_13TeV-madgraph//output/ | grep '\.root'`
hadd -f $tag/New2017/SKIM_VBFHHTo4B_CV_1_C2V_1_C3_2_13TeV-madgraph.root                   `xrdfsls -u /eos/uscms/store/user/guerrero/bbbb_ntuples/$tag/New2017/SKIM_VBFHHTo4B_CV_1_C2V_1_C3_2_13TeV-madgraph//output/ | grep '\.root'`
hadd -f $tag/New2017/SKIM_QCD_HT200to300_TuneCP5_13TeV-madgraph-pythia8.root              `xrdfsls -u /eos/uscms/store/user/guerrero/bbbb_ntuples/$tag/New2017/SKIM_QCD_HT200to300_TuneCP5_13TeV-madgraph-pythia8/output/ | grep '\.root'`
hadd -f $tag/New2017/SKIM_QCD_HT300to500_TuneCP5_13TeV-madgraph-pythia8.root              `xrdfsls -u /eos/uscms/store/user/guerrero/bbbb_ntuples/$tag/New2017/SKIM_QCD_HT300to500_TuneCP5_13TeV-madgraph-pythia8/output/ | grep '\.root'`
hadd -f $tag/New2017/SKIM_QCD_HT500to700_TuneCP5_13TeV-madgraph-pythia8.root              `xrdfsls -u /eos/uscms/store/user/guerrero/bbbb_ntuples/$tag/New2017/SKIM_QCD_HT500to700_TuneCP5_13TeV-madgraph-pythia8/output/ | grep '\.root'`
hadd -f $tag/New2017/SKIM_QCD_HT700to1000_TuneCP5_13TeV-madgraph-pythia8.root             `xrdfsls -u /eos/uscms/store/user/guerrero/bbbb_ntuples/$tag/New2017/SKIM_QCD_HT700to1000_TuneCP5_13TeV-madgraph-pythia8/output/ | grep '\.root'`
hadd -f $tag/New2017/SKIM_QCD_HT1000to1500_TuneCP5_13TeV-madgraph-pythia8.root            `xrdfsls -u /eos/uscms/store/user/guerrero/bbbb_ntuples/$tag/New2017/SKIM_QCD_HT1000to1500_TuneCP5_13TeV-madgraph-pythia8/output/ | grep '\.root'`
hadd -f $tag/New2017/SKIM_QCD_HT1500to2000_TuneCP5_13TeV-madgraph-pythia8.root            `xrdfsls -u /eos/uscms/store/user/guerrero/bbbb_ntuples/$tag/New2017/SKIM_QCD_HT1500to2000_TuneCP5_13TeV-madgraph-pythia8/output/ | grep '\.root'`
hadd -f $tag/New2017/SKIM_QCD_HT2000toInf_TuneCP5_13TeV-madgraph-pythia8.root             `xrdfsls -u /eos/uscms/store/user/guerrero/bbbb_ntuples/$tag/New2017/SKIM_QCD_HT2000toInf_TuneCP5_13TeV-madgraph-pythia8/output/ | grep '\.root'`
hadd -f $tag/New2017/SKIM_QCD_bEnriched_HT300to500_TuneCP5_13TeV-madgraph-pythia8.root    `xrdfsls -u /eos/uscms/store/user/guerrero/bbbb_ntuples/$tag/New2017/SKIM_QCD_bEnriched_HT300to500_TuneCP5_13TeV-madgraph-pythia8/output/ | grep '\.root'`
hadd -f $tag/New2017/SKIM_QCD_bEnriched_HT500to700_TuneCP5_13TeV-madgraph-pythia8.root    `xrdfsls -u /eos/uscms/store/user/guerrero/bbbb_ntuples/$tag/New2017/SKIM_QCD_bEnriched_HT500to700_TuneCP5_13TeV-madgraph-pythia8/output/ | grep '\.root'`
hadd -f $tag/New2017/SKIM_QCD_bEnriched_HT700to1000_TuneCP5_13TeV-madgraph-pythia8.root   `xrdfsls -u /eos/uscms/store/user/guerrero/bbbb_ntuples/$tag/New2017/SKIM_QCD_bEnriched_HT700to1000_TuneCP5_13TeV-madgraph-pythia8/output/ | grep '\.root'`
hadd -f $tag/New2017/SKIM_QCD_bEnriched_HT1000to1500_TuneCP5_13TeV-madgraph-pythia8.root  `xrdfsls -u /eos/uscms/store/user/guerrero/bbbb_ntuples/$tag/New2017/SKIM_QCD_bEnriched_HT1000to1500_TuneCP5_13TeV-madgraph-pythia8/output/ | grep '\.root'`
hadd -f $tag/New2017/SKIM_QCD_bEnriched_HT1500to2000_TuneCP5_13TeV-madgraph-pythia8.root  `xrdfsls -u /eos/uscms/store/user/guerrero/bbbb_ntuples/$tag/New2017/SKIM_QCD_bEnriched_HT1500to2000_TuneCP5_13TeV-madgraph-pythia8/output/ | grep '\.root'`
hadd -f $tag/New2017/SKIM_QCD_bEnriched_HT2000toInf_TuneCP5_13TeV-madgraph-pythia8.root   `xrdfsls -u /eos/uscms/store/user/guerrero/bbbb_ntuples/$tag/New2017/SKIM_QCD_bEnriched_HT2000toInf_TuneCP5_13TeV-madgraph-pythia8/output/ | grep '\.root'`
hadd -f $tag/New2017/SKIM_TTToHadronic_TuneCP5_13TeV-powheg-pythia8.root                  `xrdfsls -u /eos/uscms/store/user/guerrero/bbbb_ntuples/$tag/New2017/SKIM_TTToHadronic_TuneCP5_13TeV-powheg-pythia8/output/ | grep '\.root'`
hadd -f $tag/New2017/SKIM_TTToSemiLeptonic_TuneCP5_13TeV-powheg-pythia8.root              `xrdfsls -u /eos/uscms/store/user/guerrero/bbbb_ntuples/$tag/New2017/SKIM_TTToSemiLeptonic_TuneCP5_13TeV-powheg-pythia8/output/ | grep '\.root'`
hadd -f $tag/New2017/SKIM_TTTo2L2Nu_TuneCP5_13TeV-powheg-pythia8.root                     `xrdfsls -u /eos/uscms/store/user/guerrero/bbbb_ntuples/$tag/New2017/SKIM_TTTo2L2Nu_TuneCP5_13TeV-powheg-pythia8/output/ | grep '\.root'`
mkdir $tag
mkdir $tag/New2018
rm $tag/New2018/*.root
hadd -f $tag/New2018/SKIM_Data.root                                                                  `xrdfsls -u /eos/uscms/store/user/guerrero/bbbb_ntuples/$tag/New2018/SKIM_Data_BTagCSV/output/ | grep '\.root'`
hadd -f $tag/New2018/SKIM_QCD_HT200to300_TuneCP5_13TeV-madgraphMLM-pythia8.root                      `xrdfsls -u /eos/uscms/store/user/guerrero/bbbb_ntuples/$tag/New2018/SKIM_QCD_HT200to300_TuneCP5_13TeV-madgraphMLM-pythia8/output/ | grep '\.root'`
hadd -f $tag/New2018/SKIM_QCD_HT300to500_TuneCP5_13TeV-madgraphMLM-pythia8.root                      `xrdfsls -u /eos/uscms/store/user/guerrero/bbbb_ntuples/$tag/New2018/SKIM_QCD_HT300to500_TuneCP5_13TeV-madgraphMLM-pythia8/output/ | grep '\.root'`
hadd -f $tag/New2018/SKIM_QCD_HT500to700_TuneCP5_13TeV-madgraphMLM-pythia8.root                      `xrdfsls -u /eos/uscms/store/user/guerrero/bbbb_ntuples/$tag/New2018/SKIM_QCD_HT500to700_TuneCP5_13TeV-madgraphMLM-pythia8/output/ | grep '\.root'`
hadd -f $tag/New2018/SKIM_QCD_HT700to1000_TuneCP5_13TeV-madgraphMLM-pythia8.root                     `xrdfsls -u /eos/uscms/store/user/guerrero/bbbb_ntuples/$tag/New2018/SKIM_QCD_HT700to1000_TuneCP5_13TeV-madgraphMLM-pythia8/output/ | grep '\.root'`
hadd -f $tag/New2018/SKIM_QCD_HT1000to1500_TuneCP5_13TeV-madgraphMLM-pythia8.root                    `xrdfsls -u /eos/uscms/store/user/guerrero/bbbb_ntuples/$tag/New2018/SKIM_QCD_HT1000to1500_TuneCP5_13TeV-madgraphMLM-pythia8/output/ | grep '\.root'`
hadd -f $tag/New2018/SKIM_QCD_HT1500to2000_TuneCP5_13TeV-madgraphMLM-pythia8.root                    `xrdfsls -u /eos/uscms/store/user/guerrero/bbbb_ntuples/$tag/New2018/SKIM_QCD_HT1500to2000_TuneCP5_13TeV-madgraphMLM-pythia8/output/ | grep '\.root'`
hadd -f $tag/New2018/SKIM_QCD_HT2000toInf_TuneCP5_13TeV-madgraphMLM-pythia8.root                     `xrdfsls -u /eos/uscms/store/user/guerrero/bbbb_ntuples/$tag/New2018/SKIM_QCD_HT2000toInf_TuneCP5_13TeV-madgraphMLM-pythia8/output/ | grep '\.root'`
hadd -f $tag/New2018/SKIM_QCD_bEnriched_HT300to500_TuneCP5_13TeV-madgraphMLM-pythia8.root            `xrdfsls -u /eos/uscms/store/user/guerrero/bbbb_ntuples/$tag/New2018/SKIM_QCD_bEnriched_HT300to500_TuneCP5_13TeV-madgraphMLM-pythia8/output/ | grep '\.root'`
hadd -f $tag/New2018/SKIM_QCD_bEnriched_HT500to700_TuneCP5_13TeV-madgraphMLM-pythia8.root            `xrdfsls -u /eos/uscms/store/user/guerrero/bbbb_ntuples/$tag/New2018/SKIM_QCD_bEnriched_HT500to700_TuneCP5_13TeV-madgraphMLM-pythia8/output/ | grep '\.root'`
hadd -f $tag/New2018/SKIM_QCD_bEnriched_HT700to1000_TuneCP5_13TeV-madgraphMLM-pythia8.root           `xrdfsls -u /eos/uscms/store/user/guerrero/bbbb_ntuples/$tag/New2018/SKIM_QCD_bEnriched_HT700to1000_TuneCP5_13TeV-madgraphMLM-pythia8/output/ | grep '\.root'`
hadd -f $tag/New2018/SKIM_QCD_bEnriched_HT1000to1500_TuneCP5_13TeV-madgraphMLM-pythia8.root          `xrdfsls -u /eos/uscms/store/user/guerrero/bbbb_ntuples/$tag/New2018/SKIM_QCD_bEnriched_HT1000to1500_TuneCP5_13TeV-madgraphMLM-pythia8/output/ | grep '\.root'`
hadd -f $tag/New2018/SKIM_QCD_bEnriched_HT1500to2000_TuneCP5_13TeV-madgraphMLM-pythia8.root          `xrdfsls -u /eos/uscms/store/user/guerrero/bbbb_ntuples/$tag/New2018/SKIM_QCD_bEnriched_HT1500to2000_TuneCP5_13TeV-madgraphMLM-pythia8/output/ | grep '\.root'`
hadd -f $tag/New2018/SKIM_QCD_bEnriched_HT2000toInf_TuneCP5_13TeV-madgraphMLM-pythia8.root           `xrdfsls -u /eos/uscms/store/user/guerrero/bbbb_ntuples/$tag/New2018/SKIM_QCD_bEnriched_HT2000toInf_TuneCP5_13TeV-madgraphMLM-pythia8/output/ | grep '\.root'`
hadd -f $tag/New2018/SKIM_TTTo2L2Nu_TuneCP5_13TeV-powheg-pythia8.root                                `xrdfsls -u /eos/uscms/store/user/guerrero/bbbb_ntuples/$tag/New2018/SKIM_TTTo2L2Nu_TuneCP5_13TeV-powheg-pythia8/output/ | grep '\.root'`
hadd -f $tag/New2018/SKIM_TTToHadronic_TuneCP5_13TeV-powheg-pythia8.root                             `xrdfsls -u /eos/uscms/store/user/guerrero/bbbb_ntuples/$tag/New2018/SKIM_TTToHadronic_TuneCP5_13TeV-powheg-pythia8/output/ | grep '\.root'`
hadd -f $tag/New2018/SKIM_TTToSemiLeptonic_TuneCP5_13TeV-powheg-pythia8.root                         `xrdfsls -u /eos/uscms/store/user/guerrero/bbbb_ntuples/$tag/New2018/SKIM_TTToSemiLeptonic_TuneCP5_13TeV-powheg-pythia8/output/ | grep '\.root'`
hadd -f $tag/New2018/SKIM_VBFHHTo4B_CV_0_5_C2V_1_C3_1_TuneCP5_PSWeights_13TeV-madgraph-pythia8.root  `xrdfsls -u /eos/uscms/store/user/guerrero/bbbb_ntuples/$tag/New2018/SKIM_VBFHHTo4B_CV_0_5_C2V_1_C3_1_TuneCP5_PSWeights_13TeV-madgraph-pythia8/output/ | grep '\.root'`
hadd -f $tag/New2018/SKIM_VBFHHTo4B_CV_1_5_C2V_1_C3_1_TuneCP5_PSWeights_13TeV-madgraph-pythia8.root  `xrdfsls -u /eos/uscms/store/user/guerrero/bbbb_ntuples/$tag/New2018/SKIM_VBFHHTo4B_CV_1_5_C2V_1_C3_1_TuneCP5_PSWeights_13TeV-madgraph-pythia8/output/ | grep '\.root'`
hadd -f $tag/New2018/SKIM_VBFHHTo4B_CV_1_C2V_1_C3_0_TuneCP5_PSWeights_13TeV-madgraph-pythia8.root    `xrdfsls -u /eos/uscms/store/user/guerrero/bbbb_ntuples/$tag/New2018/SKIM_VBFHHTo4B_CV_1_C2V_1_C3_0_TuneCP5_PSWeights_13TeV-madgraph-pythia8/output/ | grep '\.root'`
hadd -f $tag/New2018/SKIM_VBFHHTo4B_CV_1_C2V_1_C3_2_TuneCP5_PSWeights_13TeV-madgraph-pythia8.root    `xrdfsls -u /eos/uscms/store/user/guerrero/bbbb_ntuples/$tag/New2018/SKIM_VBFHHTo4B_CV_1_C2V_1_C3_2_TuneCP5_PSWeights_13TeV-madgraph-pythia8/output/ | grep '\.root'`
hadd -f $tag/New2018/SKIM_VBFHHTo4B_CV_1_C2V_2_C3_1_TuneCP5_PSWeights_13TeV-madgraph-pythia8.root    `xrdfsls -u /eos/uscms/store/user/guerrero/bbbb_ntuples/$tag/New2018/SKIM_VBFHHTo4B_CV_1_C2V_2_C3_1_TuneCP5_PSWeights_13TeV-madgraph-pythia8/output/ | grep '\.root'`
hadd -f $tag/New2018/SKIM_VBFHHTo4B_CV_1_C2V_1_C3_1_TuneCP5_PSWeights_13TeV-madgraph-pythia8.root    `xrdfsls -u /eos/uscms/store/user/guerrero/bbbb_ntuples/$tag/New2018/SKIM_VBFHHTo4B_CV_1_C2V_1_C3_1_TuneCP5_PSWeights_13TeV-madgraph-pythia8/output/ | grep '\.root'`
hadd -f $tag/New2018/SKIM_GluGluToHHTo4B_node_SM_TuneCP5_PSWeights_13TeV-madgraph-pythia8.root       `xrdfsls -u /eos/uscms/store/user/guerrero/bbbb_ntuples/$tag/New2018/SKIM_GluGluToHHTo4B_node_SM_TuneCP5_PSWeights_13TeV-madgraph-pythia8/output/ | grep '\.root'`
##HADD: klambda variations
for kl in `seq -f %g -20 1 20`; do
     klname=${kl/./d}
     if [[ $klname == *-* ]]; then
         klname=${klname/-/m_};
     else
         klname=p_${klname}
     fi
     hadd $tag/New2016/SKIM_GGHH4B_rew_kl_${klname}.root   `xrdfsls -u /eos/uscms/store/user/guerrero/bbbb_ntuples/$tag/New2016/SKIM_GGHH4B_rew_kl_${klname}/output/ | grep '\.root'`
done
##HADD: klambda variations
for kl in `seq -f %g -20 1 20`; do
     klname=${kl/./d}
     if [[ $klname == *-* ]]; then
         klname=${klname/-/m_};
     else
         klname=p_${klname}
     fi
     hadd $tag/New2017/SKIM_GGHH4B_rew_kl_${klname}.root   `xrdfsls -u /eos/uscms/store/user/guerrero/bbbb_ntuples/$tag/New2017/SKIM_GGHH4B_rew_kl_${klname}/output/ | grep '\.root'`
done
##HADD: klambda variations
for kl in `seq -f %g -20 1 20`; do
     klname=${kl/./d}
     if [[ $klname == *-* ]]; then
         klname=${klname/-/m_};
     else
         klname=p_${klname}
     fi
     hadd $tag/New2018/SKIM_GGHH4B_rew_kl_${klname}.root   `xrdfsls -u /eos/uscms/store/user/guerrero/bbbb_ntuples/$tag/New2018/SKIM_GGHH4B_rew_kl_${klname}/output/ | grep '\.root'`
done