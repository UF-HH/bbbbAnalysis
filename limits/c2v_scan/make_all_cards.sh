### make datacards
YEAR=$1

for c2v in `seq -f %.1f -5.5 0.1 7.5`; do

     c2vname=${c2v}
     if [[ $c2vname == *-* ]]; then
         c2vname=${c2vname/-/m_};
     else
         c2vname=p_${c2vname}
     fi
    
    echo
    echo " ============================================== "
    echo "... running on c2v $c2v ($c2vname)"
    echo " ============================================== "
    echo

    CARDNAME="card_c2v_${c2vname}.txt"
    WSNAME=card_c2v_${c2vname}.root
    FLDRNAME="c2vcards_${YEAR}"

    mkdir $FLDRNAME

    #### step 1 : generate datacards
    echo "... making card for injection of $c2v ($c2vname)"
    python make_datacard.py \
        --fileIn /uscms/home/guerrero/nobackup/Run2/HH2019/Fall2019/CMSSW_10_2_5/src/bbbbAnalysis/limits/c2v_scan/c2v_${YEAR}/outPlotter.root \
        --sigs VBFHH4B_rew_c2v_${c2vname} \
        --var BDT2 --sel Btag4_VBFcateg_SR_110_Histogram \
        --cardOut ${FLDRNAME}/${CARDNAME}

    #### step 2 : generate workspace
    echo "... making workspace for injection of $c2v ($c2vname)"
    cd ${FLDRNAME}
    text2workspace.py $CARDNAME -D DATA 
    cd ..

    #### step 3 : run the limit
    cd ${FLDRNAME}
    combine \
        -M AsymptoticLimits -D DATA --run blind --freezeParameters allConstrainedNuisances \
        -n c2v_${c2vname} \
        ${WSNAME}
    cd ..
done    


############################################################################
########### cross check for the SM

CARDNAME="card_SM.txt"
WSNAME=card_SM.root
FLDRNAME="SMcards_${YEAR}"

mkdir $FLDRNAME

#### step 1 : generate datacards
echo "... making card for injection of SM"
python make_datacard.py \
    --fileIn /uscms/home/guerrero/nobackup/Run2/HH2019/Fall2019/CMSSW_10_2_5/src/bbbbAnalysis/limits/c2v_scan/c2v_${YEAR}/outPlotter.root \
    --sigs VBF_HH \
    --var BDT2 --sel Btag4_VBFcateg_SR_110_Histogram \
    --cardOut ${FLDRNAME}/${CARDNAME}

#### step 2 : generate workspace
echo "... making workspace for injection of SM"
cd ${FLDRNAME}
text2workspace.py $CARDNAME -D DATA 
cd ..

#### step 3 : run the limit
cd ${FLDRNAME}
combine \
    -M AsymptoticLimits -D DATA --run blind --freezeParameters allConstrainedNuisances \
    -n SM \
    ${WSNAME}
cd ..
