### make datacards
for injname in 0 3000 6000 9000; do
    
    echo
    echo " ============================================== "
    echo "... running on injection $injname"
    echo " ============================================== "
    echo

    if [ "$injname" == "0" ]; then
        HNAME="MODEL"
        INJVAL="0"
    else
        HNAME="MODEL_GGF_$injname"
        INJVAL="$injname"
    fi

    #### NOTE: the number of signal events expected is sigma (HH) x BR (4b) x lumi = 
    #### GGF: 31.05 fb x 0.5824^2 x 36 fb-1 = 379
    #### VBF: 1.726 fb x 0.5824^2 x 36 fb-1 = 21
    NOMINAL="379"
    SIGSCALE=`bc -l <<< "$injname/379"`
    echo "... the signal scale is $SIGSCALE"

    CARDNAME="card_inj_${INJVAL}.txt"
    WSNAME=card_inj_${INJVAL}.root

    # #### step 1 : generate datacards
    # echo "... making card for injection of $injname"
    # python make_datacard.py \
    #     --fileIn /uscms/home/guerrero/nobackup/Run2/HH2019/newcommit/CMSSW_10_2_5/src/bbbbAnalysis/MyHistos/Histos2016/outPlotter.root \
    #     --var BDT3 --sel Btag4_GGFcateg_SR_110_Histogram \
    #     --bkgs $HNAME \
    #     --cardOut $CARDNAME

    # #### step 2 : generate workspace
    # echo "... making workspace for injection of $injname"
    # text2workspace.py $CARDNAME -D DATA 

    #### step 3 : generate the asimov with injected signal
    echo "... generating Asimov datasets for $injname"
    # combine \
    #     -M GenerateOnly -t -1 \
    #     --dataset DATA \
    #     # --setParameters r=${SIGSCALE} \
    #     --setParameters r=23.7 \
    #     --saveToys \
    #     -n inj_${INJVAL} \
    #     card_inj_9000.root #${WSNAME}

    #### NOTE: generating from the cards with shape for mu = 0 (inj_0) that acts as my "truth" reference for the generation
    combine \
        -M GenerateOnly -t -1 \
        --dataset DATA \
        --setParameters r=${SIGSCALE} \
        --saveToys \
        -n inj_${INJVAL} \
        card_inj_0.root #${WSNAME}


    #### step 4 : best-fit signal strength
    RMIN=`bc -l <<< "$SIGSCALE/5 -10"`
    RMAX=`bc -l <<< "$SIGSCALE*5 +10"`
    # RMIN="-10"
    # RMAX="30"
    echo "... fitting the signal strength for $injname (scale = $SIGSCALE) in $RMIN, $RMAX"
    combine \
        -M MultiDimFit -t -1 \
        --dataset DATA \
        --rMin $RMIN --rMax $RMAX \
        --algo singles \
        --toysFile higgsCombineinj_${INJVAL}.GenerateOnly.mH120.123456.root \
        -n fitresult_${INJVAL} \
        ${WSNAME} | tee "log_${INJVAL}.txt"
done    
