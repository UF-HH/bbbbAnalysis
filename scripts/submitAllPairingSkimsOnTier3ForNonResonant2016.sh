############### block for nominal pairing
python scripts/submitPairingSkimOnTier3.py --tag=$1 --xrdcp-tar-only --do-tar --do-xrdcp-tar
slopeparameter=(
  0.90
  0.95
  1.00
  1.01
  1.02
  1.03
  1.04
  1.05
  1.06
  1.07
  1.08
  1.09
  1.10
  1.15
  1.20
)
#Loop over the parameters
for parameter in "${slopeparameter[@]}"; do
       python scripts/submitPairingSkimOnTier3.py --bbbbChoice BothClosestToDiagonal --option 1 --mh1mh2 ${parameter} --outputName=GluGluToHHTo4B_node_cHHH1_TuneCUETP8M1_PSWeights_13TeV-powheg-pythia8_${parameter}                --input=inputFiles/2016NonResonantDiHiggs4BDataSets/GluGluToHHTo4B_node_cHHH1_TuneCUETP8M1_PSWeights_13TeV-powheg-pythia8.txt                  --puWeight=weights/2016/PUweights_2016.root --tag=$1 --cfg=config/skim_2016NonResonantDiHiggs4B.cfg --is-signal --xs=0.010517  --njobs=20  --is-nlo-sig
       python scripts/submitPairingSkimOnTier3.py --bbbbChoice BothClosestToDiagonal --option 1 --mh1mh2 ${parameter} --outputName=GluGluToHHTo4B_node_cHHH5_TuneCUETP8M1_PSWeights_13TeV-powheg-pythia8_${parameter}                --input=inputFiles/2016NonResonantDiHiggs4BDataSets/GluGluToHHTo4B_node_cHHH5_TuneCUETP8M1_PSWeights_13TeV-powheg-pythia8.txt                  --puWeight=weights/2016/PUweights_2016.root --tag=$1 --cfg=config/skim_2016NonResonantDiHiggs4B.cfg --is-signal --xs=0.031072  --njobs=20  --is-nlo-sig
       python scripts/submitPairingSkimOnTier3.py --bbbbChoice BothClosestToDiagonal --option 1 --mh1mh2 ${parameter} --outputName=VBF_HH_CV_1_C2V_1_C3_1_dipoleRecoilOff-TuneCUETP8M1_PSweights_13TeV-madgraph-pythia8_${parameter} --input=inputFiles/2016NonResonantDiHiggs4BDataSets/VBF_HH_CV_1_C2V_1_C3_1_dipoleRecoilOff-TuneCUETP8M1_PSweights_13TeV-madgraph-pythia8.txt   --puWeight=weights/2016/PUweights_2016.root --tag=$1 --cfg=config/skim_2016NonResonantDiHiggs4B.cfg --is-signal --xs=0.000585  --njobs=20  --is-VBF-sig
       python scripts/submitPairingSkimOnTier3.py --bbbbChoice BothClosestToDiagonal --option 1 --mh1mh2 ${parameter} --outputName=VBF_HH_CV_1_C2V_2_C3_1_dipoleRecoilOff-TuneCUETP8M1_PSweights_13TeV-madgraph-pythia8_${parameter} --input=inputFiles/2016NonResonantDiHiggs4BDataSets/VBF_HH_CV_1_C2V_2_C3_1_dipoleRecoilOff-TuneCUETP8M1_PSweights_13TeV-madgraph-pythia8.txt   --puWeight=weights/2016/PUweights_2016.root --tag=$1 --cfg=config/skim_2016NonResonantDiHiggs4B.cfg --is-signal --xs=0.004823  --njobs=20  --is-VBF-sig  
done
#python scripts/submitPairingSkimOnTier3.py --bbbbChoice BothClosestToDiagonal --option 1 --mh1mh2 1.05 --outputName=Data_1.05 --input=inputFiles/2016NonResonantDiHiggs4BDataSets/Data.txt  --tag=$1  --cfg=config/skim_2016NonResonantDiHiggs4B.cfg --is-data  --njobs=700


#######This block to study the alternatives we can have in the full analysis (if necessary)
options=(
 0
 1
 2
)
for opt in "${options[@]}"; do
	   python scripts/submitPairingSkimOnTier3.py --tag=$1${opt} --xrdcp-tar-only --do-tar --do-xrdcp-tar
       python scripts/submitPairingSkimOnTier3.py --bbbbChoice BothClosestToDiagonal --option ${opt} --mh1mh2 1.05 --input=inputFiles/2016NonResonantDiHiggs4BDataSets/GluGluToHHTo4B_node_cHHH1_TuneCUETP8M1_PSWeights_13TeV-powheg-pythia8.txt                  --puWeight=weights/2016/PUweights_2016.root --tag=$1${opt} --cfg=config/skim_2016NonResonantDiHiggs4B.cfg --is-signal --xs=0.010517  --njobs=10  --is-nlo-sig
       python scripts/submitPairingSkimOnTier3.py --bbbbChoice BothClosestToDiagonal --option ${opt} --mh1mh2 1.05 --input=inputFiles/2016NonResonantDiHiggs4BDataSets/GluGluToHHTo4B_node_cHHH5_TuneCUETP8M1_PSWeights_13TeV-powheg-pythia8.txt                  --puWeight=weights/2016/PUweights_2016.root --tag=$1${opt} --cfg=config/skim_2016NonResonantDiHiggs4B.cfg --is-signal --xs=0.031072  --njobs=10  --is-nlo-sig
       python scripts/submitPairingSkimOnTier3.py --bbbbChoice BothClosestToDiagonal --option ${opt} --mh1mh2 1.05 --input=inputFiles/2016NonResonantDiHiggs4BDataSets/VBF_HH_CV_1_C2V_1_C3_1_dipoleRecoilOff-TuneCUETP8M1_PSweights_13TeV-madgraph-pythia8.txt   --puWeight=weights/2016/PUweights_2016.root --tag=$1${opt} --cfg=config/skim_2016NonResonantDiHiggs4B.cfg --is-signal --xs=0.000585  --njobs=10  --is-VBF-sig
       python scripts/submitPairingSkimOnTier3.py --bbbbChoice BothClosestToDiagonal --option ${opt} --mh1mh2 1.05 --input=inputFiles/2016NonResonantDiHiggs4BDataSets/VBF_HH_CV_1_C2V_2_C3_1_dipoleRecoilOff-TuneCUETP8M1_PSweights_13TeV-madgraph-pythia8.txt   --puWeight=weights/2016/PUweights_2016.root --tag=$1${opt} --cfg=config/skim_2016NonResonantDiHiggs4B.cfg --is-signal --xs=0.004823  --njobs=10  --is-VBF-sig  
       python scripts/submitPairingSkimOnTier3.py --bbbbChoice BothClosestToDiagonal --option ${opt} --mh1mh2 1.05 --input=inputFiles/2016NonResonantDiHiggs4BDataSets/Data.txt                                                                                                                               --tag=$1${opt} --cfg=config/skim_2016NonResonantDiHiggs4B.cfg --is-data   --njobs=700
done
