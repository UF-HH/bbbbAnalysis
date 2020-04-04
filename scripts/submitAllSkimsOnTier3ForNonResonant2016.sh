# this prepares the tar and ships it to EOS - do not remove this line!
python scripts/submitSkimOnTier3.py --tag=$1 --xrdcp-tar-only --do-tar --do-xrdcp-tar

#############################################################################################

##########Bata 
python scripts/submitSkimOnTier3.py --input=inputFiles/2016NonResonantDiHiggs4BDataSets/Data.txt  --tag=$1 --cfg=config/skim_2016NonResonantDiHiggs4B.cfg --is-data  --njobs=500

######Signals
python scripts/submitSkimOnTier3.py --input=inputFiles/2016NonResonantDiHiggs4BDataSets/GluGluToHHTo4B_node_cHHH1_TuneCUETP8M1_PSWeights_13TeV-powheg-pythia8.txt     --puWeight=weights/2016/GluGluToHHTo4B_node_SM_13TeV-madgraph_PUweights.root      --tag=$1 --cfg=config/skim_2016NonResonantDiHiggs4B.cfg --is-signal --xs=0.010517   --njobs=10  --is-nlo-sig
python scripts/submitSkimOnTier3.py --input=inputFiles/2016NonResonantDiHiggs4BDataSets/GluGluToHHTo4B_node_cHHH0_TuneCUETP8M1_PSWeights_13TeV-powheg-pythia8.txt     --puWeight=weights/2016/GluGluToHHTo4B_node_SM_13TeV-madgraph_PUweights.root      --tag=$1 --cfg=config/skim_2016NonResonantDiHiggs4B.cfg --is-signal --xs=0.023618   --njobs=10  --is-nlo-sig
python scripts/submitSkimOnTier3.py --input=inputFiles/2016NonResonantDiHiggs4BDataSets/GluGluToHHTo4B_node_cHHH2p45_TuneCUETP8M1_PSWeights_13TeV-powheg-pythia8.txt  --puWeight=weights/2016/GluGluToHHTo4B_node_SM_13TeV-madgraph_PUweights.root      --tag=$1 --cfg=config/skim_2016NonResonantDiHiggs4B.cfg --is-signal --xs=0.004455   --njobs=10  --is-nlo-sig
#python scripts/submitSkimOnTier3.py --input=inputFiles/2016NonResonantDiHiggs4BDataSets/GluGluToHHTo4B_node_cHHH5_TuneCUETP8M1_PSWeights_13TeV-powheg-pythia8.txt     --puWeight=weights/2016/GluGluToHHTo4B_node_SM_13TeV-madgraph_PUweights.root  --tag=$1 --cfg=config/skim_2016NonResonantDiHiggs4B.cfg --is-signal  --xs=0.031072  --njobs=10  --is-nlo-sig
python scripts/submitSkimOnTier3.py --input=inputFiles/2016NonResonantDiHiggs4BDataSets/VBFHHTo4B_CV_1_C2V_1_C3_1_13TeV-madgraph.txt                                  --puWeight=weights/2016/VBFHHTo4B_CV_1_C2V_1_C3_1_13TeV-madgraph_PUweights.root   --tag=$1 --cfg=config/skim_2016NonResonantDiHiggs4B.cfg --is-signal --xs=0.000585    --njobs=10
python scripts/submitSkimOnTier3.py --input=inputFiles/2016NonResonantDiHiggs4BDataSets/VBFHHTo4B_CV_1_C2V_2_C3_1_13TeV-madgraph.txt                                  --puWeight=weights/2016/VBFHHTo4B_CV_1_C2V_2_C3_1_13TeV-madgraph_PUweights.root   --tag=$1 --cfg=config/skim_2016NonResonantDiHiggs4B.cfg --is-signal --xs=0.004688    --njobs=10
python scripts/submitSkimOnTier3.py --input=inputFiles/2016NonResonantDiHiggs4BDataSets/VBFHHTo4B_CV_1_C2V_1_C3_2_13TeV-madgraph.txt                                  --puWeight=weights/2016/VBFHHTo4B_CV_1_C2V_1_C3_2_13TeV-madgraph_PUweights.root   --tag=$1 --cfg=config/skim_2016NonResonantDiHiggs4B.cfg --is-signal --xs=0.000482    --njobs=10
python scripts/submitSkimOnTier3.py --input=inputFiles/2016NonResonantDiHiggs4BDataSets/VBFHHTo4B_CV_1_C2V_1_C3_0_13TeV-madgraph.txt                                  --puWeight=weights/2016/VBFHHTo4B_CV_1_C2V_1_C3_0_13TeV-madgraph_PUweights.root   --tag=$1 --cfg=config/skim_2016NonResonantDiHiggs4B.cfg --is-signal --xs=0.001585    --njobs=10
python scripts/submitSkimOnTier3.py --input=inputFiles/2016NonResonantDiHiggs4BDataSets/VBFHHTo4B_CV_0_5_C2V_1_C3_1_13TeV-madgraph.txt                                --puWeight=weights/2016/VBFHHTo4B_CV_0_5_C2V_1_C3_1_13TeV-madgraph_PUweights.root --tag=$1 --cfg=config/skim_2016NonResonantDiHiggs4B.cfg --is-signal --xs=0.003586    --njobs=10
python scripts/submitSkimOnTier3.py --input=inputFiles/2016NonResonantDiHiggs4BDataSets/VBFHHTo4B_CV_1_5_C2V_1_C3_1_13TeV-madgraph.txt                                --puWeight=weights/2016/VBFHHTo4B_CV_1_5_C2V_1_C3_1_13TeV-madgraph_PUweights.root --tag=$1 --cfg=config/skim_2016NonResonantDiHiggs4B.cfg --is-signal --xs=0.022297    --njobs=10

#######Backgrounds: QCD, TT
python scripts/submitSkimOnTier3.py --input=inputFiles/2016NonResonantDiHiggs4BDataSets/TT_TuneCUETP8M2T4_13TeV-powheg-pythia8.txt                  --puWeight=weights/2016/TT_TuneCUETP8M2T4_13TeV-powheg-pythia8_PUweights.root      --tag=$1 --cfg=config/skim_2016NonResonantDiHiggs4B.cfg                 --xs=831.76     --njobs=200
python scripts/submitSkimOnTier3.py --input=inputFiles/2016NonResonantDiHiggs4BDataSets/QCD_HT200to300_TuneCUETP8M1_13TeV-madgraphMLM-pythia8.txt   --puWeight=weights/2016/QCD_HT200to300_TuneCUETP8M1_13TeV-madgraphMLM-pythia8_PUweights.root      --tag=$1 --cfg=config/skim_2016NonResonantDiHiggs4B.cfg  --xs=1710000    --njobs=200
python scripts/submitSkimOnTier3.py --input=inputFiles/2016NonResonantDiHiggs4BDataSets/QCD_HT300to500_TuneCUETP8M1_13TeV-madgraphMLM-pythia8.txt   --puWeight=weights/2016/QCD_HT300to500_TuneCUETP8M1_13TeV-madgraphMLM-pythia8_PUweights.root      --tag=$1 --cfg=config/skim_2016NonResonantDiHiggs4B.cfg  --xs=347500     --njobs=200
python scripts/submitSkimOnTier3.py --input=inputFiles/2016NonResonantDiHiggs4BDataSets/QCD_HT500to700_TuneCUETP8M1_13TeV-madgraphMLM-pythia8.txt   --puWeight=weights/2016/QCD_HT500to700_TuneCUETP8M1_13TeV-madgraphMLM-pythia8_PUweights.root      --tag=$1 --cfg=config/skim_2016NonResonantDiHiggs4B.cfg  --xs=32060      --njobs=200
python scripts/submitSkimOnTier3.py --input=inputFiles/2016NonResonantDiHiggs4BDataSets/QCD_HT700to1000_TuneCUETP8M1_13TeV-madgraphMLM-pythia8.txt  --puWeight=weights/2016/QCD_HT700to1000_TuneCUETP8M1_13TeV-madgraphMLM-pythia8_PUweights.root     --tag=$1 --cfg=config/skim_2016NonResonantDiHiggs4B.cfg  --xs=6829       --njobs=200
python scripts/submitSkimOnTier3.py --input=inputFiles/2016NonResonantDiHiggs4BDataSets/QCD_HT1000to1500_TuneCUETP8M1_13TeV-madgraphMLM-pythia8.txt --puWeight=weights/2016/QCD_HT1000to1500_TuneCUETP8M1_13TeV-madgraphMLM-pythia8_PUweights.root    --tag=$1 --cfg=config/skim_2016NonResonantDiHiggs4B.cfg  --xs=1207       --njobs=200
python scripts/submitSkimOnTier3.py --input=inputFiles/2016NonResonantDiHiggs4BDataSets/QCD_HT1500to2000_TuneCUETP8M1_13TeV-madgraphMLM-pythia8.txt --puWeight=weights/2016/QCD_HT1500to2000_TuneCUETP8M1_13TeV-madgraphMLM-pythia8_PUweights.root    --tag=$1 --cfg=config/skim_2016NonResonantDiHiggs4B.cfg  --xs=120        --njobs=200
python scripts/submitSkimOnTier3.py --input=inputFiles/2016NonResonantDiHiggs4BDataSets/QCD_HT2000toInf_TuneCUETP8M1_13TeV-madgraphMLM-pythia8.txt  --puWeight=weights/2016/QCD_HT2000toInf_TuneCUETP8M1_13TeV-madgraphMLM-pythia8_PUweights.root     --tag=$1 --cfg=config/skim_2016NonResonantDiHiggs4B.cfg  --xs=25.25      --njobs=200

#########Backgrounds: Single Higgs and ZZ
python scripts/submitSkimOnTier3.py --input=inputFiles/2016NonResonantDiHiggs4BDataSets/GluGluHToBB_M125_13TeV_powheg_pythia8.txt         --puWeight=weights/2016/GluGluHToBB_M125_13TeV_powheg_pythia8_PUweights.root         --tag=$1 --cfg=config/skim_2016NonResonantDiHiggs4B.cfg  --xs=30.52     --njobs=200
python scripts/submitSkimOnTier3.py --input=inputFiles/2016NonResonantDiHiggs4BDataSets/VBFHToBB_M-125_13TeV_powheg_pythia8_weightfix.txt --puWeight=weights/2016/VBFHToBB_M-125_13TeV_powheg_pythia8_weightfix_PUweights.root --tag=$1 --cfg=config/skim_2016NonResonantDiHiggs4B.cfg  --xs=3.861     --njobs=200
python scripts/submitSkimOnTier3.py --input=inputFiles/2016NonResonantDiHiggs4BDataSets/WminusH_HToBB_WToQQ_M125_13TeV_powheg_pythia8.txt --puWeight=weights/2016/WminusH_HToBB_WToQQ_M125_13TeV_powheg_pythia8_PUweights.root --tag=$1 --cfg=config/skim_2016NonResonantDiHiggs4B.cfg  --xs=0.3587    --njobs=200
python scripts/submitSkimOnTier3.py --input=inputFiles/2016NonResonantDiHiggs4BDataSets/WplusH_HToBB_WToQQ_M125_13TeV_powheg_pythia8.txt  --puWeight=weights/2016/WplusH_HToBB_WToQQ_M125_13TeV_powheg_pythia8_PUweights.root  --tag=$1 --cfg=config/skim_2016NonResonantDiHiggs4B.cfg  --xs=0.5716    --njobs=200
python scripts/submitSkimOnTier3.py --input=inputFiles/2016NonResonantDiHiggs4BDataSets/ZH_HToBB_ZToQQ_M125_13TeV_powheg_pythia8.txt      --puWeight=weights/2016/ZH_HToBB_ZToQQ_M125_13TeV_powheg_pythia8_PUweights.root      --tag=$1 --cfg=config/skim_2016NonResonantDiHiggs4B.cfg  --xs=0.5242    --njobs=200
python scripts/submitSkimOnTier3.py --input=inputFiles/2016NonResonantDiHiggs4BDataSets/ttHTobb_M125_13TeV_powheg_pythia8.txt             --puWeight=weights/2016/ttHTobb_M125_13TeV_powheg_pythia8_PUweights.root             --tag=$1 --cfg=config/skim_2016NonResonantDiHiggs4B.cfg  --xs=0.5401    --njobs=200
python scripts/submitSkimOnTier3.py --input=inputFiles/2016NonResonantDiHiggs4BDataSets/ZZTo4Q_13TeV_amcatnloFXFX_madspin_pythia8.txt     --puWeight=weights/2016/ZZTo4Q_13TeV_amcatnloFXFX_madspin_pythia8_PUweights.root     --tag=$1 --cfg=config/skim_2016NonResonantDiHiggs4B.cfg  --xs=6.912     --njobs=200

### klambda reweighting skims using EFTnodes sample
kl=-20
klname=${kl/./d}
if [[ $klname == *-* ]]; then
    klname=${klname/-/m_};
else
    klname=p_${klname}
fi
python scripts/submitSkimOnTier3.py --kl-rew $kl --kl-map weights/gg_HH_4B_EFTnodes_2016.root --outputName GGHH4B_rew_kl_${klname} \
        --input=inputFiles/2016NonResonantDiHiggs4BDataSets/GluGluToHHTo4B_EFTnodes_13TeV-madgraph.txt     --puWeight=weights/2016/GluGluToHHTo4B_node_SM_13TeV-madgraph_PUweights.root     --tag=$1 --cfg=config/skim_2016NonResonantDiHiggs4B.cfg --is-signal  --xs=0.010517 --njobs=200
for kl in `seq -f %g -19 1 20`; do
    klname=${kl/./d}
    if [[ $klname == *-* ]]; then
        klname=${klname/-/m_};
    else
        klname=p_${klname}
    fi
    # echo $kl, $klname
    python scripts/submitSkimOnTier3.py --kl-rew $kl --kl-map weights/gg_HH_4B_EFTnodes_2016.root --outputName GGHH4B_rew_kl_${klname} \
        --input=inputFiles/2016NonResonantDiHiggs4BDataSets/GluGluToHHTo4B_EFTnodes_13TeV-madgraph.txt     --puWeight=weights/2016/GluGluToHHTo4B_node_SM_13TeV-madgraph_PUweights.root     --tag=$1 --cfg=config/skim_2016NonResonantDiHiggs4B.cfg --is-signal  --xs=0.010517 --njobs=200
done

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
        python scripts/submitSkimOnTier3.py --jes-shift-syst ${jecsyst}:${systdir} --outputName GluGluToHHTo4B_node_cHHH1_TuneCUETP8M1_PSWeights_13TeV-powheg-pythia8_${jecsyst}_${systdir}    --input=inputFiles/2016NonResonantDiHiggs4BDataSets/GluGluToHHTo4B_node_cHHH1_TuneCP5_PSWeights_13TeV-powheg-pythia8.txt     --puWeight=weights/2016/GluGluToHHTo4B_node_SM_13TeV-madgraph_PUweights.root  --tag=$1 --cfg=config/skim_2016NonResonantDiHiggs4B.cfg --is-signal  --xs=0.010517  --njobs=10  --is-nlo-sig
        python scripts/submitSkimOnTier3.py --jes-shift-syst ${jecsyst}:${systdir} --outputName GluGluToHHTo4B_node_cHHH0_TuneCUETP8M1_PSWeights_13TeV-powheg-pythia8_${jecsyst}_${systdir}    --input=inputFiles/2016NonResonantDiHiggs4BDataSets/GluGluToHHTo4B_node_cHHH0_TuneCP5_PSWeights_13TeV-powheg-pythia8.txt     --puWeight=weights/2016/GluGluToHHTo4B_node_SM_13TeV-madgraph_PUweights.root  --tag=$1 --cfg=config/skim_2016NonResonantDiHiggs4B.cfg --is-signal  --xs=0.023618  --njobs=10  --is-nlo-sig
        python scripts/submitSkimOnTier3.py --jes-shift-syst ${jecsyst}:${systdir} --outputName GluGluToHHTo4B_node_cHHH2p45_TuneCUETP8M1_PSWeights_13TeV-powheg-pythia8_${jecsyst}_${systdir} --input=inputFiles/2016NonResonantDiHiggs4BDataSets/GluGluToHHTo4B_node_cHHH2p45_TuneCP5_PSWeights_13TeV-powheg-pythia8.txt  --puWeight=weights/2016/GluGluToHHTo4B_node_SM_13TeV-madgraph_PUweights.root  --tag=$1 --cfg=config/skim_2016NonResonantDiHiggs4B.cfg --is-signal  --xs=0.004455  --njobs=10  --is-nlo-sig
        #python scripts/submitSkimOnTier3.py --jes-shift-syst ${jecsyst}:${systdir} --outputName GluGluToHHTo4B_node_cHHH5_TuneCUETP8M1_PSWeights_13TeV-powheg-pythia8_${jecsyst}_${systdir}    --input=inputFiles/2016NonResonantDiHiggs4BDataSets/GluGluToHHTo4B_node_cHHH5_TuneCP5_PSWeights_13TeV-powheg-pythia8.txt     --puWeight=weights/2016/GluGluToHHTo4B_node_SM_13TeV-madgraph_PUweights.root       --tag=$1 --cfg=config/skim_2016NonResonantDiHiggs4B.cfg --is-signal  --xs=0.031072  --njobs=10  --is-nlo-sig
        python scripts/submitSkimOnTier3.py --jes-shift-syst ${jecsyst}:${systdir} --outputName VBFHHTo4B_CV_1_C2V_1_C3_1_13TeV-madgraph_${jecsyst}_${systdir}                            --input=inputFiles/2016NonResonantDiHiggs4BDataSets/VBFHHTo4B_CV_1_C2V_1_C3_1_13TeV-madgraph.txt                             --puWeight=weights/2016/VBFHHTo4B_CV_1_C2V_1_C3_1_13TeV-madgraph_PUweights.root    --tag=$1 --cfg=config/skim_2016NonResonantDiHiggs4B.cfg --is-signal  --xs=0.000585  --njobs=10
        python scripts/submitSkimOnTier3.py --jes-shift-syst ${jecsyst}:${systdir} --outputName VBFHHTo4B_CV_1_C2V_2_C3_1_13TeV-madgraph_${jecsyst}_${systdir}                            --input=inputFiles/2016NonResonantDiHiggs4BDataSets/VBFHHTo4B_CV_1_C2V_2_C3_1_13TeV-madgraph.txt                             --puWeight=weights/2016/VBFHHTo4B_CV_1_C2V_2_C3_1_13TeV-madgraph_PUweights.root    --tag=$1 --cfg=config/skim_2016NonResonantDiHiggs4B.cfg --is-signal  --xs=0.004688  --njobs=10
        python scripts/submitSkimOnTier3.py --jes-shift-syst ${jecsyst}:${systdir} --outputName VBFHHTo4B_CV_1_C2V_1_C3_2_13TeV-madgraph_${jecsyst}_${systdir}                            --input=inputFiles/2016NonResonantDiHiggs4BDataSets/VBFHHTo4B_CV_1_C2V_1_C3_2_13TeV-madgraph.txt                             --puWeight=weights/2016/VBFHHTo4B_CV_1_C2V_1_C3_2_13TeV-madgraph_PUweights.root    --tag=$1 --cfg=config/skim_2016NonResonantDiHiggs4B.cfg --is-signal  --xs=0.000482  --njobs=10
        python scripts/submitSkimOnTier3.py --jes-shift-syst ${jecsyst}:${systdir} --outputName VBFHHTo4B_CV_1_C2V_1_C3_0_13TeV-madgraph_${jecsyst}_${systdir}                            --input=inputFiles/2016NonResonantDiHiggs4BDataSets/VBFHHTo4B_CV_1_C2V_1_C3_0_13TeV-madgraph.txt                             --puWeight=weights/2016/VBFHHTo4B_CV_1_C2V_1_C3_0_13TeV-madgraph_PUweights.root    --tag=$1 --cfg=config/skim_2016NonResonantDiHiggs4B.cfg --is-signal  --xs=0.001585  --njobs=10
        python scripts/submitSkimOnTier3.py --jes-shift-syst ${jecsyst}:${systdir} --outputName VBFHHTo4B_CV_0_5_C2V_1_C3_1_13TeV-madgraph_${jecsyst}_${systdir}                          --input=inputFiles/2016NonResonantDiHiggs4BDataSets/VBFHHTo4B_CV_0_5_C2V_1_C3_1_13TeV-madgraph.txt                           --puWeight=weights/2016/VBFHHTo4B_CV_0_5_C2V_1_C3_1_13TeV-madgraph_PUweights.root  --tag=$1 --cfg=config/skim_2016NonResonantDiHiggs4B.cfg --is-signal  --xs=0.003586  --njobs=10
        python scripts/submitSkimOnTier3.py --jes-shift-syst ${jecsyst}:${systdir} --outputName VBFHHTo4B_CV_1_5_C2V_1_C3_1_13TeV-madgraph_${jecsyst}_${systdir}                          --input=inputFiles/2016NonResonantDiHiggs4BDataSets/VBFHHTo4B_CV_1_5_C2V_1_C3_1_13TeV-madgraph.txt                           --puWeight=weights/2016/VBFHHTo4B_CV_1_5_C2V_1_C3_1_13TeV-madgraph_PUweights.root  --tag=$1 --cfg=config/skim_2016NonResonantDiHiggs4B.cfg --is-signal  --xs=0.022297  --njobs=10
    done
done

#######################################################################################################################

##### systematic variations of JER
for jersyst in jer bjer ; do
    for systdir in up down ; do
        python scripts/submitSkimOnTier3.py --jer-shift-syst ${jersyst}:${systdir} --outputName GluGluToHHTo4B_node_cHHH1_TuneCUETP8M1_PSWeights_13TeV-powheg-pythia8_${jersyst}_${systdir}    --input=inputFiles/2016NonResonantDiHiggs4BDataSets/GluGluToHHTo4B_node_cHHH1_TuneCP5_PSWeights_13TeV-powheg-pythia8.txt     --puWeight=weights/2016/GluGluToHHTo4B_node_SM_13TeV-madgraph_PUweights.root       --tag=$1 --cfg=config/skim_2016NonResonantDiHiggs4B.cfg --is-signal  --xs=0.010517  --njobs=10  --is-nlo-sig
        python scripts/submitSkimOnTier3.py --jer-shift-syst ${jersyst}:${systdir} --outputName GluGluToHHTo4B_node_cHHH0_TTuneCUETP8M1_PSWeights_13TeV-powheg-pythia8_${jersyst}_${systdir}   --input=inputFiles/2016NonResonantDiHiggs4BDataSets/GluGluToHHTo4B_node_cHHH0_TuneCP5_PSWeights_13TeV-powheg-pythia8.txt     --puWeight=weights/2016/GluGluToHHTo4B_node_SM_13TeV-madgraph_PUweights.root       --tag=$1 --cfg=config/skim_2016NonResonantDiHiggs4B.cfg --is-signal  --xs=0.023618  --njobs=10  --is-nlo-sig
        python scripts/submitSkimOnTier3.py --jer-shift-syst ${jersyst}:${systdir} --outputName GluGluToHHTo4B_node_cHHH2p45_TuneCUETP8M1_PSWeights_13TeV-powheg-pythia8_${jersyst}_${systdir} --input=inputFiles/2016NonResonantDiHiggs4BDataSets/GluGluToHHTo4B_node_cHHH2p45_TuneCP5_PSWeights_13TeV-powheg-pythia8.txt  --puWeight=weights/2016/GluGluToHHTo4B_node_SM_13TeV-madgraph_PUweights.root       --tag=$1 --cfg=config/skim_2016NonResonantDiHiggs4B.cfg --is-signal  --xs=0.004455  --njobs=10  --is-nlo-sig
        #python scripts/submitSkimOnTier3.py --jer-shift-syst ${jersyst}:${systdir} --outputName GluGluToHHTo4B_node_cHHH5_TuneCUETP8M1_PSWeights_13TeV-powheg-pythia8_${jersyst}_${systdir}    --input=inputFiles/2016NonResonantDiHiggs4BDataSets/GluGluToHHTo4B_node_cHHH5_TuneCP5_PSWeights_13TeV-powheg-pythia8.txt     --puWeight=weights/2016/GluGluToHHTo4B_node_SM_13TeV-madgraph_PUweights.root       --tag=$1 --cfg=config/skim_2016NonResonantDiHiggs4B.cfg --is-signal  --xs=0.031072  --njobs=10  --is-nlo-sig
        python scripts/submitSkimOnTier3.py --jer-shift-syst ${jersyst}:${systdir} --outputName VBFHHTo4B_CV_1_C2V_1_C3_1_13TeV-madgraph_${jersyst}_${systdir}                                 --input=inputFiles/2016NonResonantDiHiggs4BDataSets/VBFHHTo4B_CV_1_C2V_1_C3_1_13TeV-madgraph.txt                             --puWeight=weights/2016/VBFHHTo4B_CV_1_C2V_1_C3_1_13TeV-madgraph_PUweights.root    --tag=$1 --cfg=config/skim_2016NonResonantDiHiggs4B.cfg --is-signal  --xs=0.000585  --njobs=10
        python scripts/submitSkimOnTier3.py --jer-shift-syst ${jersyst}:${systdir} --outputName VBFHHTo4B_CV_1_C2V_2_C3_1_13TeV-madgraph_${jersyst}_${systdir}                                 --input=inputFiles/2016NonResonantDiHiggs4BDataSets/VBFHHTo4B_CV_1_C2V_2_C3_1_13TeV-madgraph.txt                             --puWeight=weights/2016/VBFHHTo4B_CV_1_C2V_2_C3_1_13TeV-madgraph_PUweights.root    --tag=$1 --cfg=config/skim_2016NonResonantDiHiggs4B.cfg --is-signal  --xs=0.004688  --njobs=10
        python scripts/submitSkimOnTier3.py --jer-shift-syst ${jersyst}:${systdir} --outputName VBFHHTo4B_CV_1_C2V_1_C3_2_13TeV-madgraph_${jersyst}_${systdir}                                 --input=inputFiles/2016NonResonantDiHiggs4BDataSets/VBFHHTo4B_CV_1_C2V_1_C3_2_13TeV-madgraph.txt                             --puWeight=weights/2016/VBFHHTo4B_CV_1_C2V_1_C3_2_13TeV-madgraph_PUweights.root    --tag=$1 --cfg=config/skim_2016NonResonantDiHiggs4B.cfg --is-signal  --xs=0.000482  --njobs=10
        python scripts/submitSkimOnTier3.py --jer-shift-syst ${jersyst}:${systdir} --outputName VBFHHTo4B_CV_1_C2V_1_C3_0_13TeV-madgraph_${jersyst}_${systdir}                                 --input=inputFiles/2016NonResonantDiHiggs4BDataSets/VBFHHTo4B_CV_1_C2V_1_C3_0_13TeV-madgraph.txt                             --puWeight=weights/2016/VBFHHTo4B_CV_1_C2V_1_C3_0_13TeV-madgraph_PUweights.root    --tag=$1 --cfg=config/skim_2016NonResonantDiHiggs4B.cfg --is-signal  --xs=0.001585  --njobs=10
        python scripts/submitSkimOnTier3.py --jer-shift-syst ${jersyst}:${systdir} --outputName VBFHHTo4B_CV_0_5_C2V_1_C3_1_13TeV-madgraph_${jersyst}_${systdir}                               --input=inputFiles/2016NonResonantDiHiggs4BDataSets/VBFHHTo4B_CV_0_5_C2V_1_C3_1_13TeV-madgraph.txt                           --puWeight=weights/2016/VBFHHTo4B_CV_0_5_C2V_1_C3_1_13TeV-madgraph_PUweights.root  --tag=$1 --cfg=config/skim_2016NonResonantDiHiggs4B.cfg --is-signal  --xs=0.003586  --njobs=10
        python scripts/submitSkimOnTier3.py --jer-shift-syst ${jersyst}:${systdir} --outputName VBFHHTo4B_CV_1_5_C2V_1_C3_1_13TeV-madgraph_${jersyst}_${systdir}                               --input=inputFiles/2016NonResonantDiHiggs4BDataSets/VBFHHTo4B_CV_1_5_C2V_1_C3_1_13TeV-madgraph.txt                           --puWeight=weights/2016/VBFHHTo4B_CV_1_5_C2V_1_C3_1_13TeV-madgraph_PUweights.root  --tag=$1 --cfg=config/skim_2016NonResonantDiHiggs4B.cfg --is-signal  --xs=0.022297  --njobs=10
    done
done

########################################################################################################################

############ klambda reweighting skims using SM node (only for checks)
########### first one just to send the tar file
#########kl=1
#########klname=${kl/./d}
#########if [[ $klname == *-* ]]; then
#########    klname=${klname/-/m_};
#########else
#########    klname=p_${klname}
#########fi
#########python scripts/submitSkimOnTier3.py --kl-rew $kl --kl-map weights/gg_HH_4B_SM_2016.root --outputName GGHH4BSM_rew_kl_${klname} \
#########        --input=inputFiles/2016NonResonantDiHiggs4BDataSets/GluGluToHHTo4B_node_SM_13TeV-madgraph.txt     --puWeight=weights/2016/GluGluToHHTo4B_node_SM_13TeV-madgraph_PUweights.root     --tag=$1 --cfg=config/skim_2016NonResonantDiHiggs4B.cfg --is-signal  --xs=0.01043 --njobs=200
