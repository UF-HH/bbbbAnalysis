##### combined limits - assumes datacards already have been created for 2016 and 2017 with make_all_cards.sh

YEAR=COMB

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

    #### step 1 : combine datacards
    CARDNAME_2016="c2vcards_2016/card_c2v_${c2vname}.txt"
    CARDNAME_2017="c2vcards_2017/card_c2v_${c2vname}.txt"
    CARDNAME_2018="c2vcards_2018/card_c2v_${c2vname}.txt"

    ../../../HiggsAnalysis/CombinedLimit/scripts/combineCards.py c2016=${CARDNAME_2016} c2017=${CARDNAME_2017} c2018=${CARDNAME_2018} > ${FLDRNAME}/${CARDNAME}

    #### step 2 : generate workspace
    echo "... making workspace for injection of $kl ($klname)"
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

#### step 1 : combine datacards
CARDNAME_2016="SMcards_2016/card_SM.txt"
CARDNAME_2017="SMcards_2017/card_SM.txt"
CARDNAME_2018="SMcards_2018/card_SM.txt"

../../../HiggsAnalysis/CombinedLimit/scripts/combineCards.py c2016=${CARDNAME_2016} c2017=${CARDNAME_2017} c2018=${CARDNAME_2018} > ${FLDRNAME}/${CARDNAME}

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
