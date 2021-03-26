# this prepares the tar and ships it to EOS - do not remove this line!
python scripts/submitSkimOnTier3.py --tag=$1 --xrdcp-tar-only --do-tar --do-xrdcp-tar

#############################################################################################

#####Data 
python scripts/submitSkimOnTier3.py --input=inputFiles/2018NonResonantDiHiggs4BDataSets/Data.txt  --tag=$1  --cfg=config/skim_2018NonResonantDiHiggs4B.cfg --is-data  --njobs=700

######Signals
python scripts/submitSkimOnTier3.py --input=inputFiles/2018NonResonantDiHiggs4BDataSets/GluGluToHHTo4B_node_cHHH1_TuneCP5_PSWeights_13TeV-powheg-pythia8.txt                  --puWeight=weights/2018/PUweights_2018.root --tag=$1 --cfg=config/skim_2018NonResonantDiHiggs4B.cfg --is-signal --xs=0.010517  --njobs=10  --is-nlo-sig
python scripts/submitSkimOnTier3.py --input=inputFiles/2018NonResonantDiHiggs4BDataSets/GluGluToHHTo4B_node_cHHH0_TuneCP5_PSWeights_13TeV-powheg-pythia8.txt                  --puWeight=weights/2018/PUweights_2018.root --tag=$1 --cfg=config/skim_2018NonResonantDiHiggs4B.cfg --is-signal --xs=0.023618  --njobs=10  --is-nlo-sig
python scripts/submitSkimOnTier3.py --input=inputFiles/2018NonResonantDiHiggs4BDataSets/GluGluToHHTo4B_node_cHHH2p45_TuneCP5_PSWeights_13TeV-powheg-pythia8.txt               --puWeight=weights/2018/PUweights_2018.root --tag=$1 --cfg=config/skim_2018NonResonantDiHiggs4B.cfg --is-signal --xs=0.004455  --njobs=10  --is-nlo-sig
python scripts/submitSkimOnTier3.py --input=inputFiles/2018NonResonantDiHiggs4BDataSets/GluGluToHHTo4B_node_cHHH5_TuneCP5_PSWeights_13TeV-powheg-pythia8.txt                  --puWeight=weights/2018/PUweights_2018.root --tag=$1 --cfg=config/skim_2018NonResonantDiHiggs4B.cfg --is-signal --xs=0.031072  --njobs=10  --is-nlo-sig
python scripts/submitSkimOnTier3.py --input=inputFiles/2018NonResonantDiHiggs4BDataSets/VBF_HH_CV_1_C2V_0_C3_1_dipoleRecoilOff-TuneCP5_PSweights_13TeV-madgraph-pythia8.txt   --puWeight=weights/2018/PUweights_2018.root --tag=$1 --cfg=config/skim_2018NonResonantDiHiggs4B.cfg --is-signal --xs=0.009169  --njobs=10
python scripts/submitSkimOnTier3.py --input=inputFiles/2018NonResonantDiHiggs4BDataSets/VBF_HH_CV_1_C2V_1_C3_1_dipoleRecoilOff-TuneCP5_PSweights_13TeV-madgraph-pythia8.txt   --puWeight=weights/2018/PUweights_2018.root --tag=$1 --cfg=config/skim_2018NonResonantDiHiggs4B.cfg --is-signal --xs=0.000585  --njobs=10 --is-VBF-sig
python scripts/submitSkimOnTier3.py --input=inputFiles/2018NonResonantDiHiggs4BDataSets/VBF_HH_CV_1_C2V_2_C3_1_dipoleRecoilOff-TuneCP5_PSweights_13TeV-madgraph-pythia8.txt   --puWeight=weights/2018/PUweights_2018.root --tag=$1 --cfg=config/skim_2018NonResonantDiHiggs4B.cfg --is-signal --xs=0.004823  --njobs=10 --is-VBF-sig
python scripts/submitSkimOnTier3.py --input=inputFiles/2018NonResonantDiHiggs4BDataSets/VBF_HH_CV_1_C2V_1_C3_2_dipoleRecoilOff-TuneCP5_PSweights_13TeV-madgraph-pythia8.txt   --puWeight=weights/2018/PUweights_2018.root --tag=$1 --cfg=config/skim_2018NonResonantDiHiggs4B.cfg --is-signal --xs=0.000482  --njobs=10
python scripts/submitSkimOnTier3.py --input=inputFiles/2018NonResonantDiHiggs4BDataSets/VBF_HH_CV_1_C2V_1_C3_0_dipoleRecoilOff-TuneCP5_PSweights_13TeV-madgraph-pythia8.txt   --puWeight=weights/2018/PUweights_2018.root --tag=$1 --cfg=config/skim_2018NonResonantDiHiggs4B.cfg --is-signal --xs=0.001558  --njobs=10
python scripts/submitSkimOnTier3.py --input=inputFiles/2018NonResonantDiHiggs4BDataSets/VBF_HH_CV_0_5_C2V_1_C3_1_dipoleRecoilOff-TuneCP5_PSweights_13TeV-madgraph-pythia8.txt --puWeight=weights/2018/PUweights_2018.root --tag=$1 --cfg=config/skim_2018NonResonantDiHiggs4B.cfg --is-signal --xs=0.003656  --njobs=10
python scripts/submitSkimOnTier3.py --input=inputFiles/2018NonResonantDiHiggs4BDataSets/VBF_HH_CV_1_5_C2V_1_C3_1_dipoleRecoilOff-TuneCP5_PSweights_13TeV-madgraph-pythia8.txt --puWeight=weights/2018/PUweights_2018.root --tag=$1 --cfg=config/skim_2018NonResonantDiHiggs4B.cfg --is-signal --xs=0.022412  --njobs=10

##Extra signals
#python scripts/submitSkimOnTier3.py --input=inputFiles/2018NonResonantDiHiggs4BDataSets/gg_HH_bbbb_012j_nanoAOD_Petrucciani_2018.txt                          --puWeight=weights/2018/PUweights_2018.root --tag=$1 --cfg=config/skim_2018NonResonantDiHiggs4B.cfg --is-signal --xs=0.010517  --njobs=10
#python scripts/submitSkimOnTier3.py --input=inputFiles/2018NonResonantDiHiggs4BDataSets/VBF_HH_CV_1_C2V_1_C3_1-TuneCP5_PSweights_13TeV-madgraph-pythia8.txt   --puWeight=weights/2018/PUweights_2018.root --tag=$1 --cfg=config/skim_2018NonResonantDiHiggs4B.cfg --is-signal --xs=0.000585  --njobs=10
#python scripts/submitSkimOnTier3.py --input=inputFiles/2018NonResonantDiHiggs4BDataSets/VBF_HH_CV_1_C2V_2_C3_1-TuneCP5_PSweights_13TeV-madgraph-pythia8.txt   --puWeight=weights/2018/PUweights_2018.root --tag=$1 --cfg=config/skim_2018NonResonantDiHiggs4B.cfg --is-signal --xs=0.004823  --njobs=10

#############Backgrounds:QCD, TT
python scripts/submitSkimOnTier3.py --input=inputFiles/2018NonResonantDiHiggs4BDataSets/TTToHadronic_TuneCP5_13TeV-powheg-pythia8.txt           --puWeight=weights/2018/PUweights_2018.root  --tag=$1  --cfg=config/skim_2018NonResonantDiHiggs4B.cfg --xs=377.96    --njobs=100
python scripts/submitSkimOnTier3.py --input=inputFiles/2018NonResonantDiHiggs4BDataSets/TTToSemiLeptonic_TuneCP5_13TeV-powheg-pythia8.txt       --puWeight=weights/2018/PUweights_2018.root  --tag=$1  --cfg=config/skim_2018NonResonantDiHiggs4B.cfg --xs=365.34    --njobs=100
python scripts/submitSkimOnTier3.py --input=inputFiles/2018NonResonantDiHiggs4BDataSets/TTTo2L2Nu_TuneCP5_13TeV-powheg-pythia8.txt              --puWeight=weights/2018/PUweights_2018.root  --tag=$1  --cfg=config/skim_2018NonResonantDiHiggs4B.cfg --xs=88.29     --njobs=100
python scripts/submitSkimOnTier3.py --input=inputFiles/2018NonResonantDiHiggs4BDataSets/QCD_HT200to300_TuneCP5_13TeV-madgraphMLM-pythia8.txt    --puWeight=weights/2018/PUweights_2018.root  --tag=$1  --cfg=config/skim_2018NonResonantDiHiggs4B.cfg --xs=1547000   --njobs=100
python scripts/submitSkimOnTier3.py --input=inputFiles/2018NonResonantDiHiggs4BDataSets/QCD_HT300to500_TuneCP5_13TeV-madgraphMLM-pythia8.txt    --puWeight=weights/2018/PUweights_2018.root  --tag=$1  --cfg=config/skim_2018NonResonantDiHiggs4B.cfg --xs=322600    --njobs=100
python scripts/submitSkimOnTier3.py --input=inputFiles/2018NonResonantDiHiggs4BDataSets/QCD_HT500to700_TuneCP5_13TeV-madgraphMLM-pythia8.txt    --puWeight=weights/2018/PUweights_2018.root  --tag=$1  --cfg=config/skim_2018NonResonantDiHiggs4B.cfg --xs=29980     --njobs=100
python scripts/submitSkimOnTier3.py --input=inputFiles/2018NonResonantDiHiggs4BDataSets/QCD_HT700to1000_TuneCP5_13TeV-madgraphMLM-pythia8.txt   --puWeight=weights/2018/PUweights_2018.root  --tag=$1  --cfg=config/skim_2018NonResonantDiHiggs4B.cfg --xs=6334      --njobs=100
python scripts/submitSkimOnTier3.py --input=inputFiles/2018NonResonantDiHiggs4BDataSets/QCD_HT1000to1500_TuneCP5_13TeV-madgraphMLM-pythia8.txt  --puWeight=weights/2018/PUweights_2018.root  --tag=$1  --cfg=config/skim_2018NonResonantDiHiggs4B.cfg --xs=1088      --njobs=100
python scripts/submitSkimOnTier3.py --input=inputFiles/2018NonResonantDiHiggs4BDataSets/QCD_HT1500to2000_TuneCP5_13TeV-madgraphMLM-pythia8.txt  --puWeight=weights/2018/PUweights_2018.root  --tag=$1  --cfg=config/skim_2018NonResonantDiHiggs4B.cfg --xs=99.11     --njobs=100
python scripts/submitSkimOnTier3.py --input=inputFiles/2018NonResonantDiHiggs4BDataSets/QCD_HT2000toInf_TuneCP5_13TeV-madgraphMLM-pythia8.txt   --puWeight=weights/2018/PUweights_2018.root  --tag=$1  --cfg=config/skim_2018NonResonantDiHiggs4B.cfg --xs=20.23     --njobs=100

##############Background: Single Higgs, ZZ
python scripts/submitSkimOnTier3.py --input=inputFiles/2018NonResonantDiHiggs4BDataSets/GluGluHToBB_M125_13TeV_powheg_pythia8.txt         --puWeight=weights/2018/PUweights_2018.root   --tag=$1 --cfg=config/skim_2018NonResonantDiHiggs4B.cfg  --xs=30.52      --njobs=100
python scripts/submitSkimOnTier3.py --input=inputFiles/2018NonResonantDiHiggs4BDataSets/VBFHToBB_M-125_13TeV_powheg_pythia8_weightfix.txt --puWeight=weights/2018/PUweights_2018.root   --tag=$1 --cfg=config/skim_2018NonResonantDiHiggs4B.cfg  --xs=3.861      --njobs=100
python scripts/submitSkimOnTier3.py --input=inputFiles/2018NonResonantDiHiggs4BDataSets/WminusH_HToBB_WToQQ_M125_13TeV_powheg_pythia8.txt --puWeight=weights/2018/PUweights_2018.root   --tag=$1 --cfg=config/skim_2018NonResonantDiHiggs4B.cfg  --xs=0.3587     --njobs=100
python scripts/submitSkimOnTier3.py --input=inputFiles/2018NonResonantDiHiggs4BDataSets/WplusH_HToBB_WToQQ_M125_13TeV_powheg_pythia8.txt  --puWeight=weights/2018/PUweights_2018.root   --tag=$1 --cfg=config/skim_2018NonResonantDiHiggs4B.cfg  --xs=0.5716     --njobs=100
python scripts/submitSkimOnTier3.py --input=inputFiles/2018NonResonantDiHiggs4BDataSets/ZH_HToBB_ZToQQ_M125_13TeV_powheg_pythia8.txt      --puWeight=weights/2018/PUweights_2018.root   --tag=$1 --cfg=config/skim_2018NonResonantDiHiggs4B.cfg  --xs=0.5242     --njobs=100
python scripts/submitSkimOnTier3.py --input=inputFiles/2018NonResonantDiHiggs4BDataSets/ttHTobb_M125_TuneCP5_13TeV-powheg-pythia8.txt     --puWeight=weights/2018/PUweights_2018.root   --tag=$1 --cfg=config/skim_2018NonResonantDiHiggs4B.cfg  --xs=0.5269     --njobs=100
python scripts/submitSkimOnTier3.py --input=inputFiles/2018NonResonantDiHiggs4BDataSets/ZZTo4bQ01j_5f_TuneCP5_amcatNLO_FXFX_pythia8.txt   --puWeight=weights/2018/PUweights_2018.root   --tag=$1 --cfg=config/skim_2018NonResonantDiHiggs4B.cfg  --xs=0.3682     --njobs=100

#######################################################################################################################

################## block for systematics variations of JEC
#jecSystFullList=(
#    AbsoluteMPFBias
#    AbsoluteScale
#    AbsoluteStat
#    FlavorQCD
#    Fragmentation
#    PileUpDataMC
#    PileUpPtBB
#    PileUpPtEC1
#    PileUpPtEC2
#    PileUpPtHF
#    PileUpPtRef
#    RelativeFSR
#    RelativeJEREC1
#    RelativeJEREC2
#    RelativeJERHF
#    RelativePtBB
#    RelativePtEC1
#    RelativePtEC2
#    RelativePtHF
#    RelativeBal
#    RelativeSample
#    RelativeStatEC
#    RelativeStatFSR
#    RelativeStatHF
#    SinglePionECAL
#    SinglePionHCAL
#    TimePtEta
#)

jecSystList=(
  Absolute
  Absolute_2018
  BBEC1
  BBEC1_2018
  EC2
  EC2_2018
  FlavorQCD
  HF
  HF_2018
  RelativeBal
  RelativeSample_2018
)


for jecsyst in "${jecSystList[@]}"; do
    for systdir in up down ; do
         python scripts/submitSkimOnTier3.py --jes-shift-syst ${jecsyst}:${systdir} --outputName GluGluToHHTo4B_node_cHHH1_TuneCP5_PSWeights_13TeV-powheg-pythia8_${jecsyst}_${systdir}                       --input=inputFiles/2018NonResonantDiHiggs4BDataSets/GluGluToHHTo4B_node_cHHH1_TuneCP5_PSWeights_13TeV-powheg-pythia8.txt                  --puWeight=weights/2018/PUweights_2018.root --tag=$1 --cfg=config/skim_2018NonResonantDiHiggs4B.cfg --is-signal --xs=0.010517  --njobs=10  --is-nlo-sig
         python scripts/submitSkimOnTier3.py --jes-shift-syst ${jecsyst}:${systdir} --outputName GluGluToHHTo4B_node_cHHH0_TuneCP5_PSWeights_13TeV-powheg-pythia8_${jecsyst}_${systdir}                       --input=inputFiles/2018NonResonantDiHiggs4BDataSets/GluGluToHHTo4B_node_cHHH0_TuneCP5_PSWeights_13TeV-powheg-pythia8.txt                  --puWeight=weights/2018/PUweights_2018.root --tag=$1 --cfg=config/skim_2018NonResonantDiHiggs4B.cfg --is-signal --xs=0.023618  --njobs=10  --is-nlo-sig
         python scripts/submitSkimOnTier3.py --jes-shift-syst ${jecsyst}:${systdir} --outputName GluGluToHHTo4B_node_cHHH2p45_TuneCP5_PSWeights_13TeV-powheg-pythia8_${jecsyst}_${systdir}                    --input=inputFiles/2018NonResonantDiHiggs4BDataSets/GluGluToHHTo4B_node_cHHH2p45_TuneCP5_PSWeights_13TeV-powheg-pythia8.txt               --puWeight=weights/2018/PUweights_2018.root --tag=$1 --cfg=config/skim_2018NonResonantDiHiggs4B.cfg --is-signal --xs=0.004455  --njobs=10  --is-nlo-sig
         python scripts/submitSkimOnTier3.py --jes-shift-syst ${jecsyst}:${systdir} --outputName GluGluToHHTo4B_node_cHHH5_TuneCP5_PSWeights_13TeV-powheg-pythia8_${jecsyst}_${systdir}                       --input=inputFiles/2018NonResonantDiHiggs4BDataSets/GluGluToHHTo4B_node_cHHH5_TuneCP5_PSWeights_13TeV-powheg-pythia8.txt                  --puWeight=weights/2018/PUweights_2018.root --tag=$1 --cfg=config/skim_2018NonResonantDiHiggs4B.cfg --is-signal --xs=0.031072  --njobs=10  --is-nlo-sig
         python scripts/submitSkimOnTier3.py --jes-shift-syst ${jecsyst}:${systdir} --outputName VBF_HH_CV_1_C2V_0_C3_1_dipoleRecoilOff-TuneCP5_PSweights_13TeV-madgraph-pythia8_${jecsyst}_${systdir}        --input=inputFiles/2018NonResonantDiHiggs4BDataSets/VBF_HH_CV_1_C2V_0_C3_1_dipoleRecoilOff-TuneCP5_PSweights_13TeV-madgraph-pythia8.txt   --puWeight=weights/2018/PUweights_2018.root --tag=$1 --cfg=config/skim_2018NonResonantDiHiggs4B.cfg --is-signal --xs=0.009169  --njobs=10
         python scripts/submitSkimOnTier3.py --jes-shift-syst ${jecsyst}:${systdir} --outputName VBF_HH_CV_1_C2V_1_C3_1_dipoleRecoilOff-TuneCP5_PSweights_13TeV-madgraph-pythia8_${jecsyst}_${systdir}        --input=inputFiles/2018NonResonantDiHiggs4BDataSets/VBF_HH_CV_1_C2V_1_C3_1_dipoleRecoilOff-TuneCP5_PSweights_13TeV-madgraph-pythia8.txt   --puWeight=weights/2018/PUweights_2018.root --tag=$1 --cfg=config/skim_2018NonResonantDiHiggs4B.cfg --is-signal --xs=0.000585  --njobs=10
         python scripts/submitSkimOnTier3.py --jes-shift-syst ${jecsyst}:${systdir} --outputName VBF_HH_CV_1_C2V_2_C3_1_dipoleRecoilOff-TuneCP5_PSweights_13TeV-madgraph-pythia8_${jecsyst}_${systdir}        --input=inputFiles/2018NonResonantDiHiggs4BDataSets/VBF_HH_CV_1_C2V_2_C3_1_dipoleRecoilOff-TuneCP5_PSweights_13TeV-madgraph-pythia8.txt   --puWeight=weights/2018/PUweights_2018.root --tag=$1 --cfg=config/skim_2018NonResonantDiHiggs4B.cfg --is-signal --xs=0.004823  --njobs=10
         python scripts/submitSkimOnTier3.py --jes-shift-syst ${jecsyst}:${systdir} --outputName VBF_HH_CV_1_C2V_1_C3_2_dipoleRecoilOff-TuneCP5_PSweights_13TeV-madgraph-pythia8_${jecsyst}_${systdir}        --input=inputFiles/2018NonResonantDiHiggs4BDataSets/VBF_HH_CV_1_C2V_1_C3_2_dipoleRecoilOff-TuneCP5_PSweights_13TeV-madgraph-pythia8.txt   --puWeight=weights/2018/PUweights_2018.root --tag=$1 --cfg=config/skim_2018NonResonantDiHiggs4B.cfg --is-signal --xs=0.000482  --njobs=10
         python scripts/submitSkimOnTier3.py --jes-shift-syst ${jecsyst}:${systdir} --outputName VBF_HH_CV_1_C2V_1_C3_0_dipoleRecoilOff-TuneCP5_PSweights_13TeV-madgraph-pythia8_${jecsyst}_${systdir}        --input=inputFiles/2018NonResonantDiHiggs4BDataSets/VBF_HH_CV_1_C2V_1_C3_0_dipoleRecoilOff-TuneCP5_PSweights_13TeV-madgraph-pythia8.txt   --puWeight=weights/2018/PUweights_2018.root --tag=$1 --cfg=config/skim_2018NonResonantDiHiggs4B.cfg --is-signal --xs=0.001558  --njobs=10
         python scripts/submitSkimOnTier3.py --jes-shift-syst ${jecsyst}:${systdir} --outputName VBF_HH_CV_0_5_C2V_1_C3_1_dipoleRecoilOff-TuneCP5_PSweights_13TeV-madgraph-pythia8_${jecsyst}_${systdir}      --input=inputFiles/2018NonResonantDiHiggs4BDataSets/VBF_HH_CV_0_5_C2V_1_C3_1_dipoleRecoilOff-TuneCP5_PSweights_13TeV-madgraph-pythia8.txt --puWeight=weights/2018/PUweights_2018.root --tag=$1 --cfg=config/skim_2018NonResonantDiHiggs4B.cfg --is-signal --xs=0.003656  --njobs=10
         python scripts/submitSkimOnTier3.py --jes-shift-syst ${jecsyst}:${systdir} --outputName VBF_HH_CV_1_5_C2V_1_C3_1_dipoleRecoilOff-TuneCP5_PSweights_13TeV-madgraph-pythia8_${jecsyst}_${systdir}      --input=inputFiles/2018NonResonantDiHiggs4BDataSets/VBF_HH_CV_1_5_C2V_1_C3_1_dipoleRecoilOff-TuneCP5_PSweights_13TeV-madgraph-pythia8.txt --puWeight=weights/2018/PUweights_2018.root --tag=$1 --cfg=config/skim_2018NonResonantDiHiggs4B.cfg --is-signal --xs=0.022412  --njobs=10
    done
done

#######################################################################################################################

######## systematic variations of JER
for jersyst in jer bjer ; do
    for systdir in up down ; do
         python scripts/submitSkimOnTier3.py --jer-shift-syst ${jersyst}:${systdir} --outputName GluGluToHHTo4B_node_cHHH1_TuneCP5_PSWeights_13TeV-powheg-pythia8_${jersyst}_${systdir}                       --input=inputFiles/2018NonResonantDiHiggs4BDataSets/GluGluToHHTo4B_node_cHHH1_TuneCP5_PSWeights_13TeV-powheg-pythia8.txt                  --puWeight=weights/2018/PUweights_2018.root --tag=$1 --cfg=config/skim_2018NonResonantDiHiggs4B.cfg --is-signal --xs=0.010517  --njobs=10  --is-nlo-sig
         python scripts/submitSkimOnTier3.py --jer-shift-syst ${jersyst}:${systdir} --outputName GluGluToHHTo4B_node_cHHH0_TuneCP5_PSWeights_13TeV-powheg-pythia8_${jersyst}_${systdir}                       --input=inputFiles/2018NonResonantDiHiggs4BDataSets/GluGluToHHTo4B_node_cHHH0_TuneCP5_PSWeights_13TeV-powheg-pythia8.txt                  --puWeight=weights/2018/PUweights_2018.root --tag=$1 --cfg=config/skim_2018NonResonantDiHiggs4B.cfg --is-signal --xs=0.023618  --njobs=10  --is-nlo-sig
         python scripts/submitSkimOnTier3.py --jer-shift-syst ${jersyst}:${systdir} --outputName GluGluToHHTo4B_node_cHHH2p45_TuneCP5_PSWeights_13TeV-powheg-pythia8_${jersyst}_${systdir}                    --input=inputFiles/2018NonResonantDiHiggs4BDataSets/GluGluToHHTo4B_node_cHHH2p45_TuneCP5_PSWeights_13TeV-powheg-pythia8.txt               --puWeight=weights/2018/PUweights_2018.root --tag=$1 --cfg=config/skim_2018NonResonantDiHiggs4B.cfg --is-signal --xs=0.004455  --njobs=10  --is-nlo-sig
         python scripts/submitSkimOnTier3.py --jer-shift-syst ${jersyst}:${systdir} --outputName GluGluToHHTo4B_node_cHHH5_TuneCP5_PSWeights_13TeV-powheg-pythia8_${jersyst}_${systdir}                       --input=inputFiles/2018NonResonantDiHiggs4BDataSets/GluGluToHHTo4B_node_cHHH5_TuneCP5_PSWeights_13TeV-powheg-pythia8.txt                  --puWeight=weights/2018/PUweights_2018.root --tag=$1 --cfg=config/skim_2018NonResonantDiHiggs4B.cfg --is-signal --xs=0.031072  --njobs=10  --is-nlo-sig
         python scripts/submitSkimOnTier3.py --jer-shift-syst ${jersyst}:${systdir} --outputName VBF_HH_CV_1_C2V_0_C3_1_dipoleRecoilOff-TuneCP5_PSweights_13TeV-madgraph-pythia8_${jersyst}_${systdir}        --input=inputFiles/2018NonResonantDiHiggs4BDataSets/VBF_HH_CV_1_C2V_0_C3_1_dipoleRecoilOff-TuneCP5_PSweights_13TeV-madgraph-pythia8.txt   --puWeight=weights/2018/PUweights_2018.root --tag=$1 --cfg=config/skim_2018NonResonantDiHiggs4B.cfg --is-signal --xs=0.009169  --njobs=10
         python scripts/submitSkimOnTier3.py --jer-shift-syst ${jersyst}:${systdir} --outputName VBF_HH_CV_1_C2V_1_C3_1_dipoleRecoilOff-TuneCP5_PSweights_13TeV-madgraph-pythia8_${jersyst}_${systdir}        --input=inputFiles/2018NonResonantDiHiggs4BDataSets/VBF_HH_CV_1_C2V_1_C3_1_dipoleRecoilOff-TuneCP5_PSweights_13TeV-madgraph-pythia8.txt   --puWeight=weights/2018/PUweights_2018.root --tag=$1 --cfg=config/skim_2018NonResonantDiHiggs4B.cfg --is-signal --xs=0.000585  --njobs=10
         python scripts/submitSkimOnTier3.py --jer-shift-syst ${jersyst}:${systdir} --outputName VBF_HH_CV_1_C2V_2_C3_1_dipoleRecoilOff-TuneCP5_PSweights_13TeV-madgraph-pythia8_${jersyst}_${systdir}        --input=inputFiles/2018NonResonantDiHiggs4BDataSets/VBF_HH_CV_1_C2V_2_C3_1_dipoleRecoilOff-TuneCP5_PSweights_13TeV-madgraph-pythia8.txt   --puWeight=weights/2018/PUweights_2018.root --tag=$1 --cfg=config/skim_2018NonResonantDiHiggs4B.cfg --is-signal --xs=0.004823  --njobs=10
         python scripts/submitSkimOnTier3.py --jer-shift-syst ${jersyst}:${systdir} --outputName VBF_HH_CV_1_C2V_1_C3_2_dipoleRecoilOff-TuneCP5_PSweights_13TeV-madgraph-pythia8_${jersyst}_${systdir}        --input=inputFiles/2018NonResonantDiHiggs4BDataSets/VBF_HH_CV_1_C2V_1_C3_2_dipoleRecoilOff-TuneCP5_PSweights_13TeV-madgraph-pythia8.txt   --puWeight=weights/2018/PUweights_2018.root --tag=$1 --cfg=config/skim_2018NonResonantDiHiggs4B.cfg --is-signal --xs=0.000482  --njobs=10
         python scripts/submitSkimOnTier3.py --jer-shift-syst ${jersyst}:${systdir} --outputName VBF_HH_CV_1_C2V_1_C3_0_dipoleRecoilOff-TuneCP5_PSweights_13TeV-madgraph-pythia8_${jersyst}_${systdir}        --input=inputFiles/2018NonResonantDiHiggs4BDataSets/VBF_HH_CV_1_C2V_1_C3_0_dipoleRecoilOff-TuneCP5_PSweights_13TeV-madgraph-pythia8.txt   --puWeight=weights/2018/PUweights_2018.root --tag=$1 --cfg=config/skim_2018NonResonantDiHiggs4B.cfg --is-signal --xs=0.001558  --njobs=10
         python scripts/submitSkimOnTier3.py --jer-shift-syst ${jersyst}:${systdir} --outputName VBF_HH_CV_0_5_C2V_1_C3_1_dipoleRecoilOff-TuneCP5_PSweights_13TeV-madgraph-pythia8_${jersyst}_${systdir}      --input=inputFiles/2018NonResonantDiHiggs4BDataSets/VBF_HH_CV_0_5_C2V_1_C3_1_dipoleRecoilOff-TuneCP5_PSweights_13TeV-madgraph-pythia8.txt --puWeight=weights/2018/PUweights_2018.root --tag=$1 --cfg=config/skim_2018NonResonantDiHiggs4B.cfg --is-signal --xs=0.003656  --njobs=10
         python scripts/submitSkimOnTier3.py --jer-shift-syst ${jersyst}:${systdir} --outputName VBF_HH_CV_1_5_C2V_1_C3_1_dipoleRecoilOff-TuneCP5_PSweights_13TeV-madgraph-pythia8_${jersyst}_${systdir}      --input=inputFiles/2018NonResonantDiHiggs4BDataSets/VBF_HH_CV_1_5_C2V_1_C3_1_dipoleRecoilOff-TuneCP5_PSweights_13TeV-madgraph-pythia8.txt --puWeight=weights/2018/PUweights_2018.root --tag=$1 --cfg=config/skim_2018NonResonantDiHiggs4B.cfg --is-signal --xs=0.022412  --njobs=10
    done
done

####################################################################################################################################

### EFT benchmarks LO

python scripts/submitSkimOnTier3.py --kl-rew 7.5    --kt-rew 1  --c2-rew  -1    --cg-rew  0      --c2g-rew  0    --kl-map weights/gg_HH_4B_EFTnodes_2018.root --outputName GGHH4B_rew_bench_1 \
        --input=inputFiles/2018NonResonantDiHiggs4BDataSets/GluGluToHHTo4B_EFTnodes_TuneCP5_PSWeights_13TeV-madgraph-pythia8.txt     --puWeight=weights/2018/PUweights_2018.root     --tag=$1 --cfg=config/skim_2018NonResonantDiHiggs4B.cfg --is-signal  --xs=0.001 --njobs=10
python scripts/submitSkimOnTier3.py --kl-rew 1      --kt-rew 1  --c2-rew  0.5   --cg-rew  -0.8   --c2g-rew  0.6  --kl-map weights/gg_HH_4B_EFTnodes_2018.root --outputName GGHH4B_rew_bench_2 \
        --input=inputFiles/2018NonResonantDiHiggs4BDataSets/GluGluToHHTo4B_EFTnodes_TuneCP5_PSWeights_13TeV-madgraph-pythia8.txt     --puWeight=weights/2018/PUweights_2018.root     --tag=$1 --cfg=config/skim_2018NonResonantDiHiggs4B.cfg --is-signal  --xs=0.001 --njobs=10
python scripts/submitSkimOnTier3.py --kl-rew 1      --kt-rew 1  --c2-rew  -1.5  --cg-rew  0      --c2g-rew  -0.8 --kl-map weights/gg_HH_4B_EFTnodes_2018.root --outputName GGHH4B_rew_bench_3 \
        --input=inputFiles/2018NonResonantDiHiggs4BDataSets/GluGluToHHTo4B_EFTnodes_TuneCP5_PSWeights_13TeV-madgraph-pythia8.txt     --puWeight=weights/2018/PUweights_2018.root     --tag=$1 --cfg=config/skim_2018NonResonantDiHiggs4B.cfg --is-signal  --xs=0.001 --njobs=10
python scripts/submitSkimOnTier3.py --kl-rew -3.5   --kt-rew 1.5 --c2-rew  -3    --cg-rew  0      --c2g-rew  0    --kl-map weights/gg_HH_4B_EFTnodes_2018.root --outputName GGHH4B_rew_bench_4 \
        --input=inputFiles/2018NonResonantDiHiggs4BDataSets/GluGluToHHTo4B_EFTnodes_TuneCP5_PSWeights_13TeV-madgraph-pythia8.txt     --puWeight=weights/2018/PUweights_2018.root     --tag=$1 --cfg=config/skim_2018NonResonantDiHiggs4B.cfg --is-signal  --xs=0.001 --njobs=10
python scripts/submitSkimOnTier3.py --kl-rew 1      --kt-rew 1  --c2-rew  0     --cg-rew  0.8    --c2g-rew  -1   --kl-map weights/gg_HH_4B_EFTnodes_2018.root --outputName GGHH4B_rew_bench_5 \
        --input=inputFiles/2018NonResonantDiHiggs4BDataSets/GluGluToHHTo4B_EFTnodes_TuneCP5_PSWeights_13TeV-madgraph-pythia8.txt     --puWeight=weights/2018/PUweights_2018.root     --tag=$1 --cfg=config/skim_2018NonResonantDiHiggs4B.cfg --is-signal  --xs=0.001 --njobs=10
python scripts/submitSkimOnTier3.py --kl-rew 2.4    --kt-rew 1  --c2-rew  0     --cg-rew  0.2    --c2g-rew  -0.2 --kl-map weights/gg_HH_4B_EFTnodes_2018.root --outputName GGHH4B_rew_bench_6 \
        --input=inputFiles/2018NonResonantDiHiggs4BDataSets/GluGluToHHTo4B_EFTnodes_TuneCP5_PSWeights_13TeV-madgraph-pythia8.txt     --puWeight=weights/2018/PUweights_2018.root     --tag=$1 --cfg=config/skim_2018NonResonantDiHiggs4B.cfg --is-signal  --xs=0.001 --njobs=10
python scripts/submitSkimOnTier3.py --kl-rew 5      --kt-rew 1  --c2-rew  0     --cg-rew  0.2    --c2g-rew  -0.2 --kl-map weights/gg_HH_4B_EFTnodes_2018.root --outputName GGHH4B_rew_bench_7 \
        --input=inputFiles/2018NonResonantDiHiggs4BDataSets/GluGluToHHTo4B_EFTnodes_TuneCP5_PSWeights_13TeV-madgraph-pythia8.txt     --puWeight=weights/2018/PUweights_2018.root     --tag=$1 --cfg=config/skim_2018NonResonantDiHiggs4B.cfg --is-signal  --xs=0.001 --njobs=10
python scripts/submitSkimOnTier3.py --kl-rew 15     --kt-rew 1  --c2-rew  0     --cg-rew  -1     --c2g-rew  1    --kl-map weights/gg_HH_4B_EFTnodes_2018.root --outputName GGHH4B_rew_bench_8 \
        --input=inputFiles/2018NonResonantDiHiggs4BDataSets/GluGluToHHTo4B_EFTnodes_TuneCP5_PSWeights_13TeV-madgraph-pythia8.txt     --puWeight=weights/2018/PUweights_2018.root     --tag=$1 --cfg=config/skim_2018NonResonantDiHiggs4B.cfg --is-signal  --xs=0.001 --njobs=10
python scripts/submitSkimOnTier3.py --kl-rew 1      --kt-rew 1  --c2-rew  1     --cg-rew  -0.6   --c2g-rew  0.6  --kl-map weights/gg_HH_4B_EFTnodes_2018.root --outputName GGHH4B_rew_bench_9 \
        --input=inputFiles/2018NonResonantDiHiggs4BDataSets/GluGluToHHTo4B_EFTnodes_TuneCP5_PSWeights_13TeV-madgraph-pythia8.txt     --puWeight=weights/2018/PUweights_2018.root     --tag=$1 --cfg=config/skim_2018NonResonantDiHiggs4B.cfg --is-signal  --xs=0.001 --njobs=10
python scripts/submitSkimOnTier3.py --kl-rew 10     --kt-rew 1.5 --c2-rew  -1    --cg-rew  0      --c2g-rew  0    --kl-map weights/gg_HH_4B_EFTnodes_2018.root --outputName GGHH4B_rew_bench_10 \
        --input=inputFiles/2018NonResonantDiHiggs4BDataSets/GluGluToHHTo4B_EFTnodes_TuneCP5_PSWeights_13TeV-madgraph-pythia8.txt     --puWeight=weights/2018/PUweights_2018.root     --tag=$1 --cfg=config/skim_2018NonResonantDiHiggs4B.cfg --is-signal  --xs=0.001 --njobs=10
python scripts/submitSkimOnTier3.py --kl-rew 2.4    --kt-rew 1  --c2-rew  0     --cg-rew  1      --c2g-rew  -1   --kl-map weights/gg_HH_4B_EFTnodes_2018.root --outputName GGHH4B_rew_bench_11 \
        --input=inputFiles/2018NonResonantDiHiggs4BDataSets/GluGluToHHTo4B_EFTnodes_TuneCP5_PSWeights_13TeV-madgraph-pythia8.txt     --puWeight=weights/2018/PUweights_2018.root     --tag=$1 --cfg=config/skim_2018NonResonantDiHiggs4B.cfg --is-signal  --xs=0.001 --njobs=10
python scripts/submitSkimOnTier3.py --kl-rew 15     --kt-rew 1  --c2-rew  1     --cg-rew  0      --c2g-rew  0    --kl-map weights/gg_HH_4B_EFTnodes_2018.root --outputName GGHH4B_rew_bench_12 \
        --input=inputFiles/2018NonResonantDiHiggs4BDataSets/GluGluToHHTo4B_EFTnodes_TuneCP5_PSWeights_13TeV-madgraph-pythia8.txt     --puWeight=weights/2018/PUweights_2018.root     --tag=$1 --cfg=config/skim_2018NonResonantDiHiggs4B.cfg --is-signal  --xs=0.001 --njobs=10
python scripts/submitSkimOnTier3.py --kl-rew 1      --kt-rew 1  --c2-rew  0     --cg-rew  0      --c2g-rew  0    --kl-map weights/gg_HH_4B_EFTnodes_2018.root --outputName GGHH4B_rew_bench_SM \
        --input=inputFiles/2018NonResonantDiHiggs4BDataSets/GluGluToHHTo4B_EFTnodes_TuneCP5_PSWeights_13TeV-madgraph-pythia8.txt     --puWeight=weights/2018/PUweights_2018.root     --tag=$1 --cfg=config/skim_2018NonResonantDiHiggs4B.cfg --is-signal  --xs=0.001 --njobs=10

for jecsyst in "${jecSystList[@]}"; do
    for systdir in up down ; do
        python scripts/submitSkimOnTier3.py  --jes-shift-syst ${jecsyst}:${systdir} --outputName GGHH4B_rew_bench_1_${jecsyst}_${systdir}  --kl-rew   7.5   --kt-rew   1   --c2-rew  -1    --cg-rew   0   --c2g-rew  0    --kl-map weights/gg_HH_4B_EFTnodes_2018.root  --input=inputFiles/2018NonResonantDiHiggs4BDataSets/GluGluToHHTo4B_EFTnodes_TuneCP5_PSWeights_13TeV-madgraph-pythia8.txt --puWeight=weights/2018/PUweights_2018.root --tag=$1 --cfg=config/skim_2018NonResonantDiHiggs4B.cfg --is-signal  --xs=0.001 --njobs=10
        python scripts/submitSkimOnTier3.py  --jes-shift-syst ${jecsyst}:${systdir} --outputName GGHH4B_rew_bench_2_${jecsyst}_${systdir}  --kl-rew    1    --kt-rew   1   --c2-rew  0.5   --cg-rew -0.8  --c2g-rew  0.6  --kl-map weights/gg_HH_4B_EFTnodes_2018.root  --input=inputFiles/2018NonResonantDiHiggs4BDataSets/GluGluToHHTo4B_EFTnodes_TuneCP5_PSWeights_13TeV-madgraph-pythia8.txt --puWeight=weights/2018/PUweights_2018.root --tag=$1 --cfg=config/skim_2018NonResonantDiHiggs4B.cfg --is-signal  --xs=0.001 --njobs=10
        python scripts/submitSkimOnTier3.py  --jes-shift-syst ${jecsyst}:${systdir} --outputName GGHH4B_rew_bench_3_${jecsyst}_${systdir}  --kl-rew    1    --kt-rew   1   --c2-rew  -1.5  --cg-rew   0   --c2g-rew -0.8  --kl-map weights/gg_HH_4B_EFTnodes_2018.root  --input=inputFiles/2018NonResonantDiHiggs4BDataSets/GluGluToHHTo4B_EFTnodes_TuneCP5_PSWeights_13TeV-madgraph-pythia8.txt --puWeight=weights/2018/PUweights_2018.root --tag=$1 --cfg=config/skim_2018NonResonantDiHiggs4B.cfg --is-signal  --xs=0.001 --njobs=10
        python scripts/submitSkimOnTier3.py  --jes-shift-syst ${jecsyst}:${systdir} --outputName GGHH4B_rew_bench_4_${jecsyst}_${systdir}  --kl-rew  -3.5   --kt-rew  1.5  --c2-rew  -3    --cg-rew   0   --c2g-rew   0   --kl-map weights/gg_HH_4B_EFTnodes_2018.root  --input=inputFiles/2018NonResonantDiHiggs4BDataSets/GluGluToHHTo4B_EFTnodes_TuneCP5_PSWeights_13TeV-madgraph-pythia8.txt --puWeight=weights/2018/PUweights_2018.root --tag=$1 --cfg=config/skim_2018NonResonantDiHiggs4B.cfg --is-signal  --xs=0.001 --njobs=10
        python scripts/submitSkimOnTier3.py  --jes-shift-syst ${jecsyst}:${systdir} --outputName GGHH4B_rew_bench_5_${jecsyst}_${systdir}  --kl-rew    1    --kt-rew   1   --c2-rew   0    --cg-rew  0.8  --c2g-rew  -1   --kl-map weights/gg_HH_4B_EFTnodes_2018.root  --input=inputFiles/2018NonResonantDiHiggs4BDataSets/GluGluToHHTo4B_EFTnodes_TuneCP5_PSWeights_13TeV-madgraph-pythia8.txt --puWeight=weights/2018/PUweights_2018.root --tag=$1 --cfg=config/skim_2018NonResonantDiHiggs4B.cfg --is-signal  --xs=0.001 --njobs=10
        python scripts/submitSkimOnTier3.py  --jes-shift-syst ${jecsyst}:${systdir} --outputName GGHH4B_rew_bench_6_${jecsyst}_${systdir}  --kl-rew   2.4   --kt-rew   1   --c2-rew   0    --cg-rew  0.2  --c2g-rew  -0.2 --kl-map weights/gg_HH_4B_EFTnodes_2018.root  --input=inputFiles/2018NonResonantDiHiggs4BDataSets/GluGluToHHTo4B_EFTnodes_TuneCP5_PSWeights_13TeV-madgraph-pythia8.txt --puWeight=weights/2018/PUweights_2018.root --tag=$1 --cfg=config/skim_2018NonResonantDiHiggs4B.cfg --is-signal  --xs=0.001 --njobs=10
        python scripts/submitSkimOnTier3.py  --jes-shift-syst ${jecsyst}:${systdir} --outputName GGHH4B_rew_bench_7_${jecsyst}_${systdir}  --kl-rew    5    --kt-rew   1   --c2-rew   0    --cg-rew  0.2  --c2g-rew  -0.2 --kl-map weights/gg_HH_4B_EFTnodes_2018.root  --input=inputFiles/2018NonResonantDiHiggs4BDataSets/GluGluToHHTo4B_EFTnodes_TuneCP5_PSWeights_13TeV-madgraph-pythia8.txt --puWeight=weights/2018/PUweights_2018.root --tag=$1 --cfg=config/skim_2018NonResonantDiHiggs4B.cfg --is-signal  --xs=0.001 --njobs=10
        python scripts/submitSkimOnTier3.py  --jes-shift-syst ${jecsyst}:${systdir} --outputName GGHH4B_rew_bench_8_${jecsyst}_${systdir}  --kl-rew   15    --kt-rew   1   --c2-rew   0    --cg-rew  -1   --c2g-rew   1   --kl-map weights/gg_HH_4B_EFTnodes_2018.root  --input=inputFiles/2018NonResonantDiHiggs4BDataSets/GluGluToHHTo4B_EFTnodes_TuneCP5_PSWeights_13TeV-madgraph-pythia8.txt --puWeight=weights/2018/PUweights_2018.root --tag=$1 --cfg=config/skim_2018NonResonantDiHiggs4B.cfg --is-signal  --xs=0.001 --njobs=10
        python scripts/submitSkimOnTier3.py  --jes-shift-syst ${jecsyst}:${systdir} --outputName GGHH4B_rew_bench_9_${jecsyst}_${systdir}  --kl-rew    1    --kt-rew   1   --c2-rew   1    --cg-rew  -0.6 --c2g-rew  0.6  --kl-map weights/gg_HH_4B_EFTnodes_2018.root  --input=inputFiles/2018NonResonantDiHiggs4BDataSets/GluGluToHHTo4B_EFTnodes_TuneCP5_PSWeights_13TeV-madgraph-pythia8.txt --puWeight=weights/2018/PUweights_2018.root --tag=$1 --cfg=config/skim_2018NonResonantDiHiggs4B.cfg --is-signal  --xs=0.001 --njobs=10
        python scripts/submitSkimOnTier3.py  --jes-shift-syst ${jecsyst}:${systdir} --outputName GGHH4B_rew_bench_10_${jecsyst}_${systdir} --kl-rew   10    --kt-rew 1.5   --c2-rew  -1    --cg-rew   0   --c2g-rew  0    --kl-map weights/gg_HH_4B_EFTnodes_2018.root  --input=inputFiles/2018NonResonantDiHiggs4BDataSets/GluGluToHHTo4B_EFTnodes_TuneCP5_PSWeights_13TeV-madgraph-pythia8.txt --puWeight=weights/2018/PUweights_2018.root --tag=$1 --cfg=config/skim_2018NonResonantDiHiggs4B.cfg --is-signal  --xs=0.001 --njobs=10
        python scripts/submitSkimOnTier3.py  --jes-shift-syst ${jecsyst}:${systdir} --outputName GGHH4B_rew_bench_11_${jecsyst}_${systdir} --kl-rew   2.4   --kt-rew   1   --c2-rew   0    --cg-rew   1   --c2g-rew  -1   --kl-map weights/gg_HH_4B_EFTnodes_2018.root  --input=inputFiles/2018NonResonantDiHiggs4BDataSets/GluGluToHHTo4B_EFTnodes_TuneCP5_PSWeights_13TeV-madgraph-pythia8.txt --puWeight=weights/2018/PUweights_2018.root --tag=$1 --cfg=config/skim_2018NonResonantDiHiggs4B.cfg --is-signal  --xs=0.001 --njobs=10
        python scripts/submitSkimOnTier3.py  --jes-shift-syst ${jecsyst}:${systdir} --outputName GGHH4B_rew_bench_12_${jecsyst}_${systdir} --kl-rew   15    --kt-rew   1   --c2-rew   1    --cg-rew   0   --c2g-rew   0   --kl-map weights/gg_HH_4B_EFTnodes_2018.root  --input=inputFiles/2018NonResonantDiHiggs4BDataSets/GluGluToHHTo4B_EFTnodes_TuneCP5_PSWeights_13TeV-madgraph-pythia8.txt --puWeight=weights/2018/PUweights_2018.root --tag=$1 --cfg=config/skim_2018NonResonantDiHiggs4B.cfg --is-signal  --xs=0.001 --njobs=10
        python scripts/submitSkimOnTier3.py  --jes-shift-syst ${jecsyst}:${systdir} --outputName GGHH4B_rew_bench_SM_${jecsyst}_${systdir} --kl-rew    1    --kt-rew   1   --c2-rew   0    --cg-rew   0   --c2g-rew   0   --kl-map weights/gg_HH_4B_EFTnodes_2018.root  --input=inputFiles/2018NonResonantDiHiggs4BDataSets/GluGluToHHTo4B_EFTnodes_TuneCP5_PSWeights_13TeV-madgraph-pythia8.txt --puWeight=weights/2018/PUweights_2018.root --tag=$1 --cfg=config/skim_2018NonResonantDiHiggs4B.cfg --is-signal  --xs=0.001 --njobs=10
    done
done

for jersyst in jer bjer ; do
    for systdir in up down ; do
        python scripts/submitSkimOnTier3.py  --jer-shift-syst ${jersyst}:${systdir} --outputName GGHH4B_rew_bench_1_${jersyst}_${systdir}  --kl-rew   7.5   --kt-rew   1   --c2-rew  -1    --cg-rew   0   --c2g-rew  0    --kl-map weights/gg_HH_4B_EFTnodes_2018.root  --input=inputFiles/2018NonResonantDiHiggs4BDataSets/GluGluToHHTo4B_EFTnodes_TuneCP5_PSWeights_13TeV-madgraph-pythia8.txt --puWeight=weights/2018/PUweights_2018.root --tag=$1 --cfg=config/skim_2018NonResonantDiHiggs4B.cfg --is-signal  --xs=0.001 --njobs=10
        python scripts/submitSkimOnTier3.py  --jer-shift-syst ${jersyst}:${systdir} --outputName GGHH4B_rew_bench_2_${jersyst}_${systdir}  --kl-rew    1    --kt-rew   1   --c2-rew  0.5   --cg-rew -0.8  --c2g-rew  0.6  --kl-map weights/gg_HH_4B_EFTnodes_2018.root  --input=inputFiles/2018NonResonantDiHiggs4BDataSets/GluGluToHHTo4B_EFTnodes_TuneCP5_PSWeights_13TeV-madgraph-pythia8.txt --puWeight=weights/2018/PUweights_2018.root --tag=$1 --cfg=config/skim_2018NonResonantDiHiggs4B.cfg --is-signal  --xs=0.001 --njobs=10
        python scripts/submitSkimOnTier3.py  --jer-shift-syst ${jersyst}:${systdir} --outputName GGHH4B_rew_bench_3_${jersyst}_${systdir}  --kl-rew    1    --kt-rew   1   --c2-rew  -1.5  --cg-rew   0   --c2g-rew -0.8  --kl-map weights/gg_HH_4B_EFTnodes_2018.root  --input=inputFiles/2018NonResonantDiHiggs4BDataSets/GluGluToHHTo4B_EFTnodes_TuneCP5_PSWeights_13TeV-madgraph-pythia8.txt --puWeight=weights/2018/PUweights_2018.root --tag=$1 --cfg=config/skim_2018NonResonantDiHiggs4B.cfg --is-signal  --xs=0.001 --njobs=10
        python scripts/submitSkimOnTier3.py  --jer-shift-syst ${jersyst}:${systdir} --outputName GGHH4B_rew_bench_4_${jersyst}_${systdir}  --kl-rew  -3.5   --kt-rew  1.5  --c2-rew  -3    --cg-rew   0   --c2g-rew   0   --kl-map weights/gg_HH_4B_EFTnodes_2018.root  --input=inputFiles/2018NonResonantDiHiggs4BDataSets/GluGluToHHTo4B_EFTnodes_TuneCP5_PSWeights_13TeV-madgraph-pythia8.txt --puWeight=weights/2018/PUweights_2018.root --tag=$1 --cfg=config/skim_2018NonResonantDiHiggs4B.cfg --is-signal  --xs=0.001 --njobs=10
        python scripts/submitSkimOnTier3.py  --jer-shift-syst ${jersyst}:${systdir} --outputName GGHH4B_rew_bench_5_${jersyst}_${systdir}  --kl-rew    1    --kt-rew   1   --c2-rew   0    --cg-rew  0.8  --c2g-rew  -1   --kl-map weights/gg_HH_4B_EFTnodes_2018.root  --input=inputFiles/2018NonResonantDiHiggs4BDataSets/GluGluToHHTo4B_EFTnodes_TuneCP5_PSWeights_13TeV-madgraph-pythia8.txt --puWeight=weights/2018/PUweights_2018.root --tag=$1 --cfg=config/skim_2018NonResonantDiHiggs4B.cfg --is-signal  --xs=0.001 --njobs=10
        python scripts/submitSkimOnTier3.py  --jer-shift-syst ${jersyst}:${systdir} --outputName GGHH4B_rew_bench_6_${jersyst}_${systdir}  --kl-rew   2.4   --kt-rew   1   --c2-rew   0    --cg-rew  0.2  --c2g-rew  -0.2 --kl-map weights/gg_HH_4B_EFTnodes_2018.root  --input=inputFiles/2018NonResonantDiHiggs4BDataSets/GluGluToHHTo4B_EFTnodes_TuneCP5_PSWeights_13TeV-madgraph-pythia8.txt --puWeight=weights/2018/PUweights_2018.root --tag=$1 --cfg=config/skim_2018NonResonantDiHiggs4B.cfg --is-signal  --xs=0.001 --njobs=10
        python scripts/submitSkimOnTier3.py  --jer-shift-syst ${jersyst}:${systdir} --outputName GGHH4B_rew_bench_7_${jersyst}_${systdir}  --kl-rew    5    --kt-rew   1   --c2-rew   0    --cg-rew  0.2  --c2g-rew  -0.2 --kl-map weights/gg_HH_4B_EFTnodes_2018.root  --input=inputFiles/2018NonResonantDiHiggs4BDataSets/GluGluToHHTo4B_EFTnodes_TuneCP5_PSWeights_13TeV-madgraph-pythia8.txt --puWeight=weights/2018/PUweights_2018.root --tag=$1 --cfg=config/skim_2018NonResonantDiHiggs4B.cfg --is-signal  --xs=0.001 --njobs=10
        python scripts/submitSkimOnTier3.py  --jer-shift-syst ${jersyst}:${systdir} --outputName GGHH4B_rew_bench_8_${jersyst}_${systdir}  --kl-rew   15    --kt-rew   1   --c2-rew   0    --cg-rew  -1   --c2g-rew   1   --kl-map weights/gg_HH_4B_EFTnodes_2018.root  --input=inputFiles/2018NonResonantDiHiggs4BDataSets/GluGluToHHTo4B_EFTnodes_TuneCP5_PSWeights_13TeV-madgraph-pythia8.txt --puWeight=weights/2018/PUweights_2018.root --tag=$1 --cfg=config/skim_2018NonResonantDiHiggs4B.cfg --is-signal  --xs=0.001 --njobs=10
        python scripts/submitSkimOnTier3.py  --jer-shift-syst ${jersyst}:${systdir} --outputName GGHH4B_rew_bench_9_${jersyst}_${systdir}  --kl-rew    1    --kt-rew   1   --c2-rew   1    --cg-rew  -0.6 --c2g-rew  0.6  --kl-map weights/gg_HH_4B_EFTnodes_2018.root  --input=inputFiles/2018NonResonantDiHiggs4BDataSets/GluGluToHHTo4B_EFTnodes_TuneCP5_PSWeights_13TeV-madgraph-pythia8.txt --puWeight=weights/2018/PUweights_2018.root --tag=$1 --cfg=config/skim_2018NonResonantDiHiggs4B.cfg --is-signal  --xs=0.001 --njobs=10
        python scripts/submitSkimOnTier3.py  --jer-shift-syst ${jersyst}:${systdir} --outputName GGHH4B_rew_bench_10_${jersyst}_${systdir} --kl-rew   10    --kt-rew 1.5   --c2-rew  -1    --cg-rew   0   --c2g-rew  0    --kl-map weights/gg_HH_4B_EFTnodes_2018.root  --input=inputFiles/2018NonResonantDiHiggs4BDataSets/GluGluToHHTo4B_EFTnodes_TuneCP5_PSWeights_13TeV-madgraph-pythia8.txt --puWeight=weights/2018/PUweights_2018.root --tag=$1 --cfg=config/skim_2018NonResonantDiHiggs4B.cfg --is-signal  --xs=0.001 --njobs=10
        python scripts/submitSkimOnTier3.py  --jer-shift-syst ${jersyst}:${systdir} --outputName GGHH4B_rew_bench_11_${jersyst}_${systdir} --kl-rew   2.4   --kt-rew   1   --c2-rew   0    --cg-rew   1   --c2g-rew  -1   --kl-map weights/gg_HH_4B_EFTnodes_2018.root  --input=inputFiles/2018NonResonantDiHiggs4BDataSets/GluGluToHHTo4B_EFTnodes_TuneCP5_PSWeights_13TeV-madgraph-pythia8.txt --puWeight=weights/2018/PUweights_2018.root --tag=$1 --cfg=config/skim_2018NonResonantDiHiggs4B.cfg --is-signal  --xs=0.001 --njobs=10
        python scripts/submitSkimOnTier3.py  --jer-shift-syst ${jersyst}:${systdir} --outputName GGHH4B_rew_bench_12_${jersyst}_${systdir} --kl-rew   15    --kt-rew   1   --c2-rew   1    --cg-rew   0   --c2g-rew   0   --kl-map weights/gg_HH_4B_EFTnodes_2018.root  --input=inputFiles/2018NonResonantDiHiggs4BDataSets/GluGluToHHTo4B_EFTnodes_TuneCP5_PSWeights_13TeV-madgraph-pythia8.txt --puWeight=weights/2018/PUweights_2018.root --tag=$1 --cfg=config/skim_2018NonResonantDiHiggs4B.cfg --is-signal  --xs=0.001 --njobs=10
        python scripts/submitSkimOnTier3.py  --jer-shift-syst ${jersyst}:${systdir} --outputName GGHH4B_rew_bench_SM_${jersyst}_${systdir} --kl-rew    1    --kt-rew   1   --c2-rew   0    --cg-rew   0   --c2g-rew   0   --kl-map weights/gg_HH_4B_EFTnodes_2018.root  --input=inputFiles/2018NonResonantDiHiggs4BDataSets/GluGluToHHTo4B_EFTnodes_TuneCP5_PSWeights_13TeV-madgraph-pythia8.txt --puWeight=weights/2018/PUweights_2018.root --tag=$1 --cfg=config/skim_2018NonResonantDiHiggs4B.cfg --is-signal  --xs=0.001 --njobs=10
    done
done

################################################## klambda reweighting skims using EFTnodes at LO
#kl=1 #-20
#klname=${kl/./d}
#if [[ $klname == *-* ]]; then
#    klname=${klname/-/m_};
#else
#    klname=p_${klname}
#fi
#python scripts/submitSkimOnTier3.py --kl-rew $kl --kl-map weights/gg_HH_4B_EFTnodes_2018.root --outputName GGHH4B_rew_kl_${klname} \
#        --input=inputFiles/2018NonResonantDiHiggs4BDataSets/GluGluToHHTo4B_EFTnodes_TuneCP5_PSWeights_13TeV-madgraph-pythia8.txt     --puWeight=weights/2018/PUweights_2018.root     --tag=$1 --cfg=config/skim_2018NonResonantDiHiggs4B.cfg --is-signal  --xs=0.010517 --njobs=200


####for kl in `seq -f %g -19 1 20`; do
####    klname=${kl/./d}
####    if [[ $klname == *-* ]]; then
####        klname=${klname/-/m_};
####    else
####        klname=p_${klname}
####    fi
####    # echo $kl, $klname
####    python scripts/submitSkimOnTier3.py --kl-rew $kl --kl-map weights/gg_HH_4B_EFTnodes_2018.root --outputName GGHH4B_rew_kl_${klname} \
####        --input=inputFiles/2018NonResonantDiHiggs4BDataSets/GluGluToHHTo4B_EFTnodes_TuneCP5_PSWeights_13TeV-madgraph-pythia8.txt     --puWeight=weights/2018/PUweights_2018.root     --tag=$1 --cfg=config/skim_2018NonResonantDiHiggs4B.cfg --is-signal  --xs=0.010517    --njobs=200
####done

#######python scripts/submitSkimOnTier3.py --input=inputFiles/2018NonResonantDiHiggs4BDataSets/QCD_bEnriched_HT300to500_TuneCP5_13TeV-madgraphMLM-pythia8.txt    --puWeight=weights/2018/QCD_HT300to500_TuneCP5_13TeV-madgraphMLM-pythia8_PUweights.root     --tag=$1  --cfg=config/skim_2018NonResonantDiHiggs4B.cfg --xs=16620.0   --njobs=400
#######python scripts/submitSkimOnTier3.py --input=inputFiles/2018NonResonantDiHiggs4BDataSets/QCD_bEnriched_HT500to700_TuneCP5_13TeV-madgraphMLM-pythia8.txt    --puWeight=weights/2018/QCD_HT500to700_TuneCP5_13TeV-madgraphMLM-pythia8_PUweights.root     --tag=$1  --cfg=config/skim_2018NonResonantDiHiggs4B.cfg --xs=1487.0    --njobs=400
#######python scripts/submitSkimOnTier3.py --input=inputFiles/2018NonResonantDiHiggs4BDataSets/QCD_bEnriched_HT700to1000_TuneCP5_13TeV-madgraphMLM-pythia8.txt   --puWeight=weights/2018/QCD_HT700to1000_TuneCP5_13TeV-madgraphMLM-pythia8_PUweights.root    --tag=$1  --cfg=config/skim_2018NonResonantDiHiggs4B.cfg --xs=296.5     --njobs=400
#######python scripts/submitSkimOnTier3.py --input=inputFiles/2018NonResonantDiHiggs4BDataSets/QCD_bEnriched_HT1000to1500_TuneCP5_13TeV-madgraphMLM-pythia8.txt  --puWeight=weights/2018/QCD_HT1000to1500_TuneCP5_13TeV-madgraphMLM-pythia8_PUweights.root   --tag=$1  --cfg=config/skim_2018NonResonantDiHiggs4B.cfg --xs=46.61     --njobs=400
#######python scripts/submitSkimOnTier3.py --input=inputFiles/2018NonResonantDiHiggs4BDataSets/QCD_bEnriched_HT1500to2000_TuneCP5_13TeV-madgraphMLM-pythia8.txt  --puWeight=weights/2018/QCD_HT1500to2000_TuneCP5_13TeV-madgraphMLM-pythia8_PUweights.root   --tag=$1  --cfg=config/skim_2018NonResonantDiHiggs4B.cfg --xs=3.72      --njobs=400
#######python scripts/submitSkimOnTier3.py --input=inputFiles/2018NonResonantDiHiggs4BDataSets/QCD_bEnriched_HT2000toInf_TuneCP5_13TeV-madgraphMLM-pythia8.txt   --puWeight=weights/2018/QCD_HT2000toInf_TuneCP5_13TeV-madgraphMLM-pythia8_PUweights.root    --tag=$1  --cfg=config/skim_2018NonResonantDiHiggs4B.cfg --xs=0.6462     --njobs=400
