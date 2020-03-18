# this prepares the tar and ships it to EOS - do not remove this line!
python scripts/submitSkimOnTier3.py --tag=$1 --xrdcp-tar-only --do-tar --do-xrdcp-tar

#############################################################################################

###Signals
python scripts/submitSkimOnTier3.py --input=inputFiles/2017NonResonantDiHiggs4BDataSets/GluGluToHHTo4B_node_SM_13TeV-madgraph_correctedcfg.txt --puWeight=weights/2017/GluGluToHHTo4B_node_SM_13TeV-madgraph_correctedcfg_PUweights.root --tag=$1  --cfg=config/skim_2017NonResonantDiHiggs4B.cfg --is-signal  --xs=0.01043 --njobs=100  
python scripts/submitSkimOnTier3.py --input=inputFiles/2017NonResonantDiHiggs4BDataSets/VBFHHTo4B_CV_1_C2V_2_C3_1_13TeV-madgraph.txt   --puWeight=weights/2017/VBFHHTo4B_CV_1_C2V_2_C3_1_13TeV-madgraph_PUweights.root    --tag=$1  --cfg=config/skim_2017NonResonantDiHiggs4B.cfg --is-signal  --xs=0.00472   --njobs=100
python scripts/submitSkimOnTier3.py --input=inputFiles/2017NonResonantDiHiggs4BDataSets/VBFHHTo4B_CV_1_C2V_1_C3_1_13TeV-madgraph.txt   --puWeight=weights/2017/VBFHHTo4B_CV_1_C2V_1_C3_1_13TeV-madgraph_PUweights.root    --tag=$1  --cfg=config/skim_2017NonResonantDiHiggs4B.cfg --is-signal  --xs=0.00054   --njobs=100
python scripts/submitSkimOnTier3.py --input=inputFiles/2017NonResonantDiHiggs4BDataSets/VBFHHTo4B_CV_1_C2V_1_C3_2_13TeV-madgraph.txt   --puWeight=weights/2017/VBFHHTo4B_CV_1_C2V_1_C3_2_13TeV-madgraph_PUweights.root    --tag=$1  --cfg=config/skim_2017NonResonantDiHiggs4B.cfg --is-signal  --xs=0.00044   --njobs=100
python scripts/submitSkimOnTier3.py --input=inputFiles/2017NonResonantDiHiggs4BDataSets/VBFHHTo4B_CV_1_C2V_1_C3_0_13TeV-madgraph.txt   --puWeight=weights/2017/VBFHHTo4B_CV_1_C2V_1_C3_0_13TeV-madgraph_PUweights.root    --tag=$1  --cfg=config/skim_2017NonResonantDiHiggs4B.cfg --is-signal  --xs=0.00145   --njobs=100
python scripts/submitSkimOnTier3.py --input=inputFiles/2017NonResonantDiHiggs4BDataSets/VBFHHTo4B_CV_1_5_C2V_1_C3_1_13TeV-madgraph.txt --puWeight=weights/2017/VBFHHTo4B_CV_1_5_C2V_1_C3_1_13TeV-madgraph_PUweights.root  --tag=$1  --cfg=config/skim_2017NonResonantDiHiggs4B.cfg --is-signal  --xs=0.02149   --njobs=100

###Background: QCD, TT
python scripts/submitSkimOnTier3.py --input=inputFiles/2017NonResonantDiHiggs4BDataSets/TTToHadronic_TuneCP5_13TeV-powheg-pythia8.txt          --puWeight=weights/2017/TTToHadronic_TuneCP5_13TeV-powheg-pythia8_PUweights.root          --tag=$1  --cfg=config/skim_2017NonResonantDiHiggs4B.cfg   --xs=377.96    --njobs=100
python scripts/submitSkimOnTier3.py --input=inputFiles/2017NonResonantDiHiggs4BDataSets/TTToSemiLeptonic_TuneCP5_13TeV-powheg-pythia8.txt      --puWeight=weights/2017/TTToSemiLeptonic_TuneCP5_13TeV-powheg-pythia8_PUweights.root      --tag=$1  --cfg=config/skim_2017NonResonantDiHiggs4B.cfg   --xs=365.34    --njobs=100
python scripts/submitSkimOnTier3.py --input=inputFiles/2017NonResonantDiHiggs4BDataSets/TTTo2L2Nu_TuneCP5_13TeV-powheg-pythia8.txt             --puWeight=weights/2017/TTTo2L2Nu_TuneCP5_13TeV-powheg-pythia8_PUweights.root             --tag=$1  --cfg=config/skim_2017NonResonantDiHiggs4B.cfg   --xs=88.29     --njobs=100
python scripts/submitSkimOnTier3.py --input=inputFiles/2017NonResonantDiHiggs4BDataSets/QCD_HT200to300_TuneCP5_13TeV-madgraph-pythia8.txt      --puWeight=weights/2017/QCD_HT200to300_TuneCP5_13TeV-madgraph-pythia8_PUweights.root      --tag=$1  --cfg=config/skim_2017NonResonantDiHiggs4B.cfg   --xs=1547000   --njobs=100
python scripts/submitSkimOnTier3.py --input=inputFiles/2017NonResonantDiHiggs4BDataSets/QCD_HT300to500_TuneCP5_13TeV-madgraph-pythia8.txt      --puWeight=weights/2017/QCD_HT300to500_TuneCP5_13TeV-madgraph-pythia8_PUweights.root      --tag=$1  --cfg=config/skim_2017NonResonantDiHiggs4B.cfg   --xs=322600    --njobs=100
python scripts/submitSkimOnTier3.py --input=inputFiles/2017NonResonantDiHiggs4BDataSets/QCD_HT500to700_TuneCP5_13TeV-madgraph-pythia8.txt      --puWeight=weights/2017/QCD_HT500to700_TuneCP5_13TeV-madgraph-pythia8_PUweights.root      --tag=$1  --cfg=config/skim_2017NonResonantDiHiggs4B.cfg   --xs=29980     --njobs=100
python scripts/submitSkimOnTier3.py --input=inputFiles/2017NonResonantDiHiggs4BDataSets/QCD_HT700to1000_TuneCP5_13TeV-madgraph-pythia8.txt     --puWeight=weights/2017/QCD_HT700to1000_TuneCP5_13TeV-madgraph-pythia8_PUweights.root     --tag=$1  --cfg=config/skim_2017NonResonantDiHiggs4B.cfg   --xs=6334      --njobs=100
python scripts/submitSkimOnTier3.py --input=inputFiles/2017NonResonantDiHiggs4BDataSets/QCD_HT1000to1500_TuneCP5_13TeV-madgraph-pythia8.txt    --puWeight=weights/2017/QCD_HT1000to1500_TuneCP5_13TeV-madgraph-pythia8_PUweights.root    --tag=$1  --cfg=config/skim_2017NonResonantDiHiggs4B.cfg   --xs=1088      --njobs=100
python scripts/submitSkimOnTier3.py --input=inputFiles/2017NonResonantDiHiggs4BDataSets/QCD_HT1500to2000_TuneCP5_13TeV-madgraph-pythia8.txt    --puWeight=weights/2017/QCD_HT1500to2000_TuneCP5_13TeV-madgraph-pythia8_PUweights.root    --tag=$1  --cfg=config/skim_2017NonResonantDiHiggs4B.cfg   --xs=99.11     --njobs=100
python scripts/submitSkimOnTier3.py --input=inputFiles/2017NonResonantDiHiggs4BDataSets/QCD_HT2000toInf_TuneCP5_13TeV-madgraph-pythia8.txt     --puWeight=weights/2017/QCD_HT2000toInf_TuneCP5_13TeV-madgraph-pythia8_PUweights.root     --tag=$1  --cfg=config/skim_2017NonResonantDiHiggs4B.cfg   --xs=20.23     --njobs=100

###Backgrounds: Single Higgs and ZZ
python scripts/submitSkimOnTier3.py --input=inputFiles/2017NonResonantDiHiggs4BDataSets/GluGluHToBB_M125_TuneCP5_13TeV-powheg-pythia8.txt --puWeight=weights/2017/GluGluHToBB_M125_TuneCP5_13TeV-powheg-pythia8_PUweights.root --tag=$1 --cfg=config/skim_2017NonResonantDiHiggs4B.cfg --xs=30.52      --njobs=200
python scripts/submitSkimOnTier3.py --input=inputFiles/2017NonResonantDiHiggs4BDataSets/VBFHToBB_M-125_13TeV_powheg_pythia8_weightfix.txt --puWeight=weights/2017/VBFHToBB_M-125_13TeV_powheg_pythia8_weightfix_PUweights.root --tag=$1 --cfg=config/skim_2017NonResonantDiHiggs4B.cfg --xs=3.861      --njobs=200
python scripts/submitSkimOnTier3.py --input=inputFiles/2017NonResonantDiHiggs4BDataSets/WminusH_HToBB_WToQQ_M125_13TeV_powheg_pythia8.txt --puWeight=weights/2017/WminusH_HToBB_WToQQ_M125_13TeV_powheg_pythia8_PUweights.root --tag=$1 --cfg=config/skim_2017NonResonantDiHiggs4B.cfg --xs=0.3654     --njobs=200
python scripts/submitSkimOnTier3.py --input=inputFiles/2017NonResonantDiHiggs4BDataSets/WplusH_HToBB_WToQQ_M125_13TeV_powheg_pythia8.txt  --puWeight=weights/2017/WplusH_HToBB_WToQQ_M125_13TeV_powheg_pythia8_PUweights.root  --tag=$1 --cfg=config/skim_2017NonResonantDiHiggs4B.cfg --xs=0.5716     --njobs=200
python scripts/submitSkimOnTier3.py --input=inputFiles/2017NonResonantDiHiggs4BDataSets/ZH_HToBB_ZToQQ_M125_13TeV_powheg_pythia8.txt      --puWeight=weights/2017/ZH_HToBB_ZToQQ_M125_13TeV_powheg_pythia8_PUweights.root      --tag=$1 --cfg=config/skim_2017NonResonantDiHiggs4B.cfg --xs=0.5242     --njobs=200
python scripts/submitSkimOnTier3.py --input=inputFiles/2017NonResonantDiHiggs4BDataSets/ttHTobb_M125_TuneCP5_13TeV-powheg-pythia8.txt     --puWeight=weights/2017/ttHTobb_M125_TuneCP5_13TeV-powheg-pythia8_PUweights.root     --tag=$1 --cfg=config/skim_2017NonResonantDiHiggs4B.cfg --xs=0.5269     --njobs=200
python scripts/submitSkimOnTier3.py --input=inputFiles/2017NonResonantDiHiggs4BDataSets/ZZTo4bQ01j_5f_TuneCP5_amcatNLO_FXFX_pythia8.txt   --puWeight=weights/2017/ZZTo4bQ01j_5f_TuneCP5_amcatNLO_FXFX_pythia8_PUweights.root   --tag=$1 --cfg=config/skim_2017NonResonantDiHiggs4B.cfg --xs=0.3682     --njobs=200

###Data in 2017
python scripts/submitSkimOnTier3.py --input=inputFiles/2017NonResonantDiHiggs4BDataSets/Data_BTagCSV.txt --tag=$1  --cfg=config/skim_2017NonResonantDiHiggs4B.cfg --is-data   --njobs=1000

### klambda reweighting skims
### first one just to send the tar file
kl=-20
klname=${kl/./d}
if [[ $klname == *-* ]]; then
    klname=${klname/-/m_};
else
    klname=p_${klname}
fi
python scripts/submitSkimOnTier3.py --kl-rew $kl --kl-map weights/gg_HH_4B_EFTnodes_2017.root --outputName GGHH4B_rew_kl_${klname} \
        --input=inputFiles/2017NonResonantDiHiggs4BDataSets/GluGluToHHTo4B_EFTnodes_13TeV-madgraph_correctedcfg.txt --puWeight=weights/2017/GluGluToHHTo4B_node_SM_13TeV-madgraph_correctedcfg_PUweights.root --tag=$1  --cfg=config/skim_2017NonResonantDiHiggs4B.cfg --is-signal  --xs=0.01043 --njobs=100  
for kl in `seq -f %g -19 1 20`; do
    klname=${kl/./d}
    if [[ $klname == *-* ]]; then
        klname=${klname/-/m_};
    else
        klname=p_${klname}
    fi
    # echo $kl, $klname
    python scripts/submitSkimOnTier3.py --kl-rew $kl --kl-map weights/gg_HH_4B_EFTnodes_2017.root --outputName GGHH4B_rew_kl_${klname} \
        --input=inputFiles/2017NonResonantDiHiggs4BDataSets/GluGluToHHTo4B_EFTnodes_13TeV-madgraph_correctedcfg.txt --puWeight=weights/2017/GluGluToHHTo4B_node_SM_13TeV-madgraph_correctedcfg_PUweights.root --tag=$1  --cfg=config/skim_2017NonResonantDiHiggs4B.cfg --is-signal  --xs=0.01043   --njobs=100  
done

### klambda reweighting skims using SM nodes (only for checks)
### first one just to send the tar file
kl=1
klname=${kl/./d}
if [[ $klname == *-* ]]; then
    klname=${klname/-/m_};
else
    klname=p_${klname}
fi
python scripts/submitSkimOnTier3.py --kl-rew $kl --kl-map weights/gg_HH_4B_SM_2017.root --outputName GGHH4BSM_rew_kl_${klname} \
        --input=inputFiles/2017NonResonantDiHiggs4BDataSets/GluGluToHHTo4B_node_SM_13TeV-madgraph_correctedcfg.txt --puWeight=weights/2017/GluGluToHHTo4B_node_SM_13TeV-madgraph_correctedcfg_PUweights.root --tag=$1  --cfg=config/skim_2017NonResonantDiHiggs4B.cfg --is-signal  --xs=0.01043 --njobs=100  

###python scripts/submitSkimOnTier3.py --input=inputFiles/2017NonResonantDiHiggs4BDataSets/QCD_bEnriched_HT300to500_TuneCP5_13TeV-madgraph-pythia8.txt      --puWeight=weights/2017/QCD_HT300to500_TuneCP5_13TeV-madgraph-pythia8_PUweights.root      --tag=$1  --cfg=config/skim_2017NonResonantDiHiggs4B.cfg   --xs=16620.0   --njobs=100
###python scripts/submitSkimOnTier3.py --input=inputFiles/2017NonResonantDiHiggs4BDataSets/QCD_bEnriched_HT500to700_TuneCP5_13TeV-madgraph-pythia8.txt      --puWeight=weights/2017/QCD_HT500to700_TuneCP5_13TeV-madgraph-pythia8_PUweights.root      --tag=$1  --cfg=config/skim_2017NonResonantDiHiggs4B.cfg   --xs=1487.0    --njobs=100
###python scripts/submitSkimOnTier3.py --input=inputFiles/2017NonResonantDiHiggs4BDataSets/QCD_bEnriched_HT700to1000_TuneCP5_13TeV-madgraph-pythia8.txt     --puWeight=weights/2017/QCD_HT700to1000_TuneCP5_13TeV-madgraph-pythia8_PUweights.root     --tag=$1  --cfg=config/skim_2017NonResonantDiHiggs4B.cfg   --xs=296.5     --njobs=100
###python scripts/submitSkimOnTier3.py --input=inputFiles/2017NonResonantDiHiggs4BDataSets/QCD_bEnriched_HT1000to1500_TuneCP5_13TeV-madgraph-pythia8.txt    --puWeight=weights/2017/QCD_HT1000to1500_TuneCP5_13TeV-madgraph-pythia8_PUweights.root    --tag=$1  --cfg=config/skim_2017NonResonantDiHiggs4B.cfg   --xs=46.61     --njobs=100
###python scripts/submitSkimOnTier3.py --input=inputFiles/2017NonResonantDiHiggs4BDataSets/QCD_bEnriched_HT1500to2000_TuneCP5_13TeV-madgraph-pythia8.txt    --puWeight=weights/2017/QCD_HT1500to2000_TuneCP5_13TeV-madgraph-pythia8_PUweights.root    --tag=$1  --cfg=config/skim_2017NonResonantDiHiggs4B.cfg   --xs=3.72      --njobs=100
###python scripts/submitSkimOnTier3.py --input=inputFiles/2017NonResonantDiHiggs4BDataSets/QCD_bEnriched_HT2000toInf_TuneCP5_13TeV-madgraph-pythia8.txt     --puWeight=weights/2017/QCD_HT2000toInf_TuneCP5_13TeV-madgraph-pythia8_PUweights.root     --tag=$1  --cfg=config/skim_2017NonResonantDiHiggs4B.cfg   --xs=0.6462    --njobs=100


#######################################################################################################################


#### block for systematics variations of JEC
jecSystList=(
    AbsoluteMPFBias
    AbsoluteScale
    AbsoluteStat
    FlavorQCD
    Fragmentation
    PileUpDataMC
    PileUpPtBB
    PileUpPtEC1
    PileUpPtEC2
    PileUpPtHF
    PileUpPtRef
    RelativeFSR
    RelativeJEREC1
    RelativeJEREC2
    RelativeJERHF
    RelativePtBB
    RelativePtEC1
    RelativePtEC2
    RelativePtHF
    RelativeBal
    RelativeSample
    RelativeStatEC
    RelativeStatFSR
    RelativeStatHF
    SinglePionECAL
    SinglePionHCAL
    TimePtEta
)

for jecsyst in "${jecSystList[@]}"; do
    for systdir in up down ; do
        python scripts/submitSkimOnTier3.py --jes-shift-syst ${jecsyst}:${systdir} --outputName GluGluToHHTo4B_node_SM_13TeV-madgraph_correctedcfg_${jecsyst}_${systdir} --input=inputFiles/2017NonResonantDiHiggs4BDataSets/GluGluToHHTo4B_node_SM_13TeV-madgraph_correctedcfg.txt --puWeight=weights/2017/GluGluToHHTo4B_node_SM_13TeV-madgraph_correctedcfg_PUweights.root --tag=$1  --cfg=config/skim_2017NonResonantDiHiggs4B.cfg --is-signal  --xs=0.01043 --njobs=100  
    done
done