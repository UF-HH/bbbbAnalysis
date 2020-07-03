YEAR=$1

if [ "$YEAR" == "2018" ] ; then
   cd NtuplesV7/2018/
   hadd -f SKIM_GluGluToHHTo4B_node_cHHH1_full_TuneCP5_PSWeights_13TeV-powheg-pythia8_PM.root  SKIM_GluGluToHHTo4B_node_cHHH0_TuneCP5_PSWeights_13TeV-powheg-pythia8_PM.root SKIM_GluGluToHHTo4B_node_cHHH1_TuneCP5_PSWeights_13TeV-powheg-pythia8_PM.root SKIM_GluGluToHHTo4B_node_cHHH2p45_TuneCP5_PSWeights_13TeV-powheg-pythia8_PM.root SKIM_GluGluToHHTo4B_node_cHHH5_TuneCP5_PSWeights_13TeV-powheg-pythia8_PM.root
   cd ../..
elif [ "$YEAR" == "2017" ]; then
   cd NtuplesV7/2017/
   hadd -f SKIM_GluGluToHHTo4B_node_cHHH1_full_TuneCP5_PSWeights_13TeV-powheg-pythia8_PM.root  SKIM_GluGluToHHTo4B_node_cHHH0_TuneCP5_PSWeights_13TeV-powheg-pythia8_PM.root SKIM_GluGluToHHTo4B_node_cHHH1_TuneCP5_PSWeights_13TeV-powheg-pythia8_PM.root SKIM_GluGluToHHTo4B_node_cHHH2p45_TuneCP5_PSWeights_13TeV-powheg-pythia8_PM.root SKIM_GluGluToHHTo4B_node_cHHH5_TuneCP5_PSWeights_13TeV-powheg-pythia8_PM.root
   cd ../..
elif [ "$YEAR" == "2016" ]; then
   cd NtuplesV7/2016/
   hadd -f SKIM_GluGluToHHTo4B_node_cHHH1_full_TuneCUETP8M1_PSWeights_13TeV-powheg-pythia8_PM.root  SKIM_GluGluToHHTo4B_node_cHHH0_TuneCUETP8M1_PSWeights_13TeV-powheg-pythia8_PM.root SKIM_GluGluToHHTo4B_node_cHHH1_TuneCUETP8M1_PSWeights_13TeV-powheg-pythia8_PM.root SKIM_GluGluToHHTo4B_node_cHHH2p45_TuneCUETP8M1_PSWeights_13TeV-powheg-pythia8_PM.root SKIM_GluGluToHHTo4B_node_cHHH5_TuneCUETP8M1_PSWeights_13TeV-powheg-pythia8_PM.root
   cd ../..
else
  echo "[ERROR] Wrong dataset!"
fi