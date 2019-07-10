### make datacards
YEAR=2016

for kl in `seq -f %g -10 1 10`; do

     klname=${kl/./d}
     if [[ $klname == *-* ]]; then
         klname=${klname/-/m_};
     else
         klname=p_${klname}
     fi
    
    echo
    echo " ============================================== "
    echo "... running on klambda $kl ($klname)"
    echo " ============================================== "
    echo

    CARDNAME="card_kl_${klname}.txt"
    WSNAME=card_kl_${klname}.root
    FLDRNAME="klambda_${YEAR}"

    mkdir $FLDRNAME

    #### step 1 : generate datacards
    echo "... making card for injection of $kl ($klname)"
    python make_datacard.py \
        --fileIn /uscms/home/lcadamur/nobackup/bbbbAnalysis_4Lug2019/CMSSW_10_2_5/src/bbbbAnalysis/Histos${YEAR}skim++/outPlotter.root \
        --sigs GGHH4B_rew_kl_${klname} \
        --var BDT3 --sel Btag4_GGFcateg_SR_110_Histogram \
        --cardOut ${FLDRNAME}/${CARDNAME}

    #### step 2 : generate workspace
    echo "... making workspace for injection of $kl ($klname)"
    cd ${FLDRNAME}
    text2workspace.py $CARDNAME -D DATA 
    cd ..

    #### step 3 : run the limit
    cd ${FLDRNAME}
    combine \
        -M AsymptoticLimits -D DATA --run blind --freezeParameters allConstrainedNuisances \
        -n kl_${klname} \
        ${WSNAME}
    cd ..
done    


############################################################################
########### cross check for the SM

CARDNAME="card_SM.txt"
WSNAME=card_SM.root
FLDRNAME="SM_${YEAR}"

mkdir $FLDRNAME

#### step 1 : generate datacards
echo "... making card for injection of SM"
python make_datacard.py \
    --fileIn /uscms/home/lcadamur/nobackup/bbbbAnalysis_4Lug2019/CMSSW_10_2_5/src/bbbbAnalysis/Histos${YEAR}skim++/outPlotter.root \
    --sigs GGF_HH \
    --var BDT3 --sel Btag4_GGFcateg_SR_110_Histogram \
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
