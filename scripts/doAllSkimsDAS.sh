### HH xs : 31.05, H->bb BR = 0.5824 --> xs * BR(bbbb) = 10.53
skim_for_das.exe --input inputFiles/CMSDAS/Samples2016/gg_HH_bbbb_SM.txt --output gg_HH_bbbb.root --xs 10.53 --is-signal --cfg config/skim_CMSDAS_2016DatMC.cfg --puWeight weights/2016/GluGluToHHTo4B_node_SM_13TeV-madgraph_PUweights.root 

skim_for_das.exe --input inputFiles/CMSDAS/Samples2016/data_BTagCSV_Run2016B_ver1.txt --output data_BTagCSV_Run2016B_ver1.root --is-data --cfg config/skim_CMSDAS_2016DatMC.cfg
skim_for_das.exe --input inputFiles/CMSDAS/Samples2016/data_BTagCSV_Run2016B_ver2.txt --output data_BTagCSV_Run2016B_ver2.root --is-data --cfg config/skim_CMSDAS_2016DatMC.cfg
skim_for_das.exe --input inputFiles/CMSDAS/Samples2016/data_BTagCSV_Run2016C.txt      --output data_BTagCSV_Run2016C.root      --is-data --cfg config/skim_CMSDAS_2016DatMC.cfg
skim_for_das.exe --input inputFiles/CMSDAS/Samples2016/data_BTagCSV_Run2016D.txt      --output data_BTagCSV_Run2016D.root      --is-data --cfg config/skim_CMSDAS_2016DatMC.cfg
skim_for_das.exe --input inputFiles/CMSDAS/Samples2016/data_BTagCSV_Run2016E.txt      --output data_BTagCSV_Run2016E.root      --is-data --cfg config/skim_CMSDAS_2016DatMC.cfg
skim_for_das.exe --input inputFiles/CMSDAS/Samples2016/data_BTagCSV_Run2016F.txt      --output data_BTagCSV_Run2016F.root      --is-data --cfg config/skim_CMSDAS_2016DatMC.cfg
skim_for_das.exe --input inputFiles/CMSDAS/Samples2016/data_BTagCSV_Run2016G.txt      --output data_BTagCSV_Run2016G.root      --is-data --cfg config/skim_CMSDAS_2016DatMC.cfg
skim_for_das.exe --input inputFiles/CMSDAS/Samples2016/data_BTagCSV_Run2016H.txt      --output data_BTagCSV_Run2016H.root      --is-data --cfg config/skim_CMSDAS_2016DatMC.cfg


skim_for_das.exe --input QCD_HT100to200_TuneCUETP8M1_13TeV-madgraphMLM-pythia8.txt    --output QCD_HT100to200.root             --cfg  config/skim_CMSDAS_2016DatMC.cfg --xs 28060000
skim_for_das.exe --input QCD_HT200to300_TuneCUETP8M1_13TeV-madgraphMLM-pythia8.txt    --output QCD_HT200to300.root             --cfg  config/skim_CMSDAS_2016DatMC.cfg --xs 1710000
skim_for_das.exe --input QCD_HT300to500_TuneCUETP8M1_13TeV-madgraphMLM-pythia8.txt    --output QCD_HT300to500.root             --cfg  config/skim_CMSDAS_2016DatMC.cfg --xs 347500 
skim_for_das.exe --input QCD_HT500to700_TuneCUETP8M1_13TeV-madgraphMLM-pythia8.txt    --output QCD_HT500to700.root             --cfg  config/skim_CMSDAS_2016DatMC.cfg --xs 32060  
skim_for_das.exe --input QCD_HT700to1000_TuneCUETP8M1_13TeV-madgraphMLM-pythia8.txt   --output QCD_HT700to1000.root            --cfg  config/skim_CMSDAS_2016DatMC.cfg --xs 6829   
skim_for_das.exe --input QCD_HT1000to1500_TuneCUETP8M1_13TeV-madgraphMLM-pythia8.txt  --output QCD_HT1000to1500.root           --cfg  config/skim_CMSDAS_2016DatMC.cfg --xs 1207   
skim_for_das.exe --input QCD_HT1500to2000_TuneCUETP8M1_13TeV-madgraphMLM-pythia8.txt  --output QCD_HT1500to2000.root           --cfg  config/skim_CMSDAS_2016DatMC.cfg --xs 120    
skim_for_das.exe --input QCD_HT2000toInf_TuneCUETP8M1_13TeV-madgraphMLM-pythia8.txt   --output QCD_HT2000toInf.root            --cfg  config/skim_CMSDAS_2016DatMC.cfg --xs 25.25  



python scripts/submitSkimOnTier3.py --input=inputFiles/2016NonResonantDiHiggs4BDataSets/GluGluToHHTo4B_node_SM_13TeV-madgraph.txt      --puWeight=weights/2016/GluGluToHHTo4B_node_SM_13TeV-madgraph_PUweights.root       --tag=$1 --cfg=config/skim_2016NonResonantDiHiggs4B.cfg --is-signal --xs=0.01043  --njobs=200
python scripts/submitSkimOnTier3.py --input=inputFiles/2016NonResonantDiHiggs4BDataSets/VBFHHTo4B_CV_1_C2V_1_C3_1_13TeV-madgraph.txt   --puWeight=weights/2016/VBFHHTo4B_CV_1_C2V_1_C3_1_13TeV-madgraph_PUweights.root    --tag=$1 --cfg=config/skim_2016NonResonantDiHiggs4B.cfg --is-signal --xs=0.00054  --no-tar --no-xrdcp-tar --njobs=200
python scripts/submitSkimOnTier3.py --input=inputFiles/2016NonResonantDiHiggs4BDataSets/VBFHHTo4B_CV_1_C2V_2_C3_1_13TeV-madgraph.txt   --puWeight=weights/2016/VBFHHTo4B_CV_1_C2V_2_C3_1_13TeV-madgraph_PUweights.root    --tag=$1 --cfg=config/skim_2016NonResonantDiHiggs4B.cfg --is-signal --xs=0.00472  --no-tar --no-xrdcp-tar --njobs=200
python scripts/submitSkimOnTier3.py --input=inputFiles/2016NonResonantDiHiggs4BDataSets/VBFHHTo4B_CV_1_C2V_1_C3_2_13TeV-madgraph.txt   --puWeight=weights/2016/VBFHHTo4B_CV_1_C2V_1_C3_2_13TeV-madgraph_PUweights.root    --tag=$1 --cfg=config/skim_2016NonResonantDiHiggs4B.cfg --is-signal --xs=0.00044  --no-tar --no-xrdcp-tar --njobs=200
python scripts/submitSkimOnTier3.py --input=inputFiles/2016NonResonantDiHiggs4BDataSets/VBFHHTo4B_CV_1_C2V_1_C3_0_13TeV-madgraph.txt   --puWeight=weights/2016/VBFHHTo4B_CV_1_C2V_1_C3_0_13TeV-madgraph_PUweights.root    --tag=$1 --cfg=config/skim_2016NonResonantDiHiggs4B.cfg --is-signal --xs=0.00145  --no-tar --no-xrdcp-tar --njobs=200
python scripts/submitSkimOnTier3.py --input=inputFiles/2016NonResonantDiHiggs4BDataSets/VBFHHTo4B_CV_0_5_C2V_1_C3_1_13TeV-madgraph.txt --puWeight=weights/2016/VBFHHTo4B_CV_0_5_C2V_1_C3_1_13TeV-madgraph_PUweights.root  --tag=$1 --cfg=config/skim_2016NonResonantDiHiggs4B.cfg --is-signal --xs=0.00353  --no-tar --no-xrdcp-tar --njobs=200
python scripts/submitSkimOnTier3.py --input=inputFiles/2016NonResonantDiHiggs4BDataSets/VBFHHTo4B_CV_1_5_C2V_1_C3_1_13TeV-madgraph.txt --puWeight=weights/2016/VBFHHTo4B_CV_1_5_C2V_1_C3_1_13TeV-madgraph_PUweights.root  --tag=$1 --cfg=config/skim_2016NonResonantDiHiggs4B.cfg --is-signal --xs=0.02149  --no-tar --no-xrdcp-tar --njobs=200
python scripts/submitSkimOnTier3.py --input=inputFiles/2016NonResonantDiHiggs4BDataSets/TT_TuneCUETP8M2T4_13TeV-powheg-pythia8.txt     --puWeight=weights/2016/TT_TuneCUETP8M2T4_13TeV-powheg-pythia8_PUweights.root      --tag=$1 --cfg=config/skim_2016NonResonantDiHiggs4B.cfg                              --xs=831.76   --no-tar --no-xrdcp-tar --njobs=200

python scripts/submitSkimOnTier3.py --input=inputFiles/2016NonResonantDiHiggs4BDataSets/QCD_HT200to300_TuneCUETP8M1_13TeV-madgraphMLM-pythia8.txt   --puWeight=weights/2016/QCD_HT200to300_TuneCUETP8M1_13TeV-madgraphMLM-pythia8_PUweights.root      --tag=$1 --cfg=config/skim_2016NonResonantDiHiggs4B.cfg  --xs=1710000  --no-tar --no-xrdcp-tar --njobs=200
python scripts/submitSkimOnTier3.py --input=inputFiles/2016NonResonantDiHiggs4BDataSets/QCD_HT300to500_TuneCUETP8M1_13TeV-madgraphMLM-pythia8.txt   --puWeight=weights/2016/QCD_HT300to500_TuneCUETP8M1_13TeV-madgraphMLM-pythia8_PUweights.root      --tag=$1 --cfg=config/skim_2016NonResonantDiHiggs4B.cfg  --xs=347500   --no-tar --no-xrdcp-tar --njobs=200
python scripts/submitSkimOnTier3.py --input=inputFiles/2016NonResonantDiHiggs4BDataSets/QCD_HT500to700_TuneCUETP8M1_13TeV-madgraphMLM-pythia8.txt   --puWeight=weights/2016/QCD_HT500to700_TuneCUETP8M1_13TeV-madgraphMLM-pythia8_PUweights.root      --tag=$1 --cfg=config/skim_2016NonResonantDiHiggs4B.cfg  --xs=32060    --no-tar --no-xrdcp-tar --njobs=200
python scripts/submitSkimOnTier3.py --input=inputFiles/2016NonResonantDiHiggs4BDataSets/QCD_HT700to1000_TuneCUETP8M1_13TeV-madgraphMLM-pythia8.txt  --puWeight=weights/2016/QCD_HT700to1000_TuneCUETP8M1_13TeV-madgraphMLM-pythia8_PUweights.root     --tag=$1 --cfg=config/skim_2016NonResonantDiHiggs4B.cfg  --xs=6829     --no-tar --no-xrdcp-tar --njobs=200
python scripts/submitSkimOnTier3.py --input=inputFiles/2016NonResonantDiHiggs4BDataSets/QCD_HT1000to1500_TuneCUETP8M1_13TeV-madgraphMLM-pythia8.txt --puWeight=weights/2016/QCD_HT1000to1500_TuneCUETP8M1_13TeV-madgraphMLM-pythia8_PUweights.root    --tag=$1 --cfg=config/skim_2016NonResonantDiHiggs4B.cfg  --xs=1207     --no-tar --no-xrdcp-tar --njobs=200
python scripts/submitSkimOnTier3.py --input=inputFiles/2016NonResonantDiHiggs4BDataSets/QCD_HT1500to2000_TuneCUETP8M1_13TeV-madgraphMLM-pythia8.txt --puWeight=weights/2016/QCD_HT1500to2000_TuneCUETP8M1_13TeV-madgraphMLM-pythia8_PUweights.root    --tag=$1 --cfg=config/skim_2016NonResonantDiHiggs4B.cfg  --xs=120      --no-tar --no-xrdcp-tar --njobs=200
python scripts/submitSkimOnTier3.py --input=inputFiles/2016NonResonantDiHiggs4BDataSets/QCD_HT2000toInf_TuneCUETP8M1_13TeV-madgraphMLM-pythia8.txt  --puWeight=weights/2016/QCD_HT2000toInf_TuneCUETP8M1_13TeV-madgraphMLM-pythia8_PUweights.root     --tag=$1 --cfg=config/skim_2016NonResonantDiHiggs4B.cfg  --xs=25.25    --no-tar --no-xrdcp-tar --njobs=200


python scripts/submitSkimOnTier3.py --input=inputFiles/2016NonResonantDiHiggs4BDataSets/Data_BTagCSV.txt  --tag=$1 --cfg=config/skim_2016NonResonantDiHiggs4B.cfg --is-data  --no-tar --no-xrdcp-tar --njobs=500
