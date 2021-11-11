### make upper limits
YEAR=$1
CAT=$2  #0=Allcategories, 1=ggFcateg1, 2=ggFcateg2

signals=(ggHH_bench_5 ggHH_bench_6 ggHH_bench_7 ggHH_bench_8)


for signal in ${signals[@]}
do

    eftname=${signal}
    
    echo
    echo " ============================================== "
    echo "... running on EFT $signal ($eftname)"
    echo " ============================================== "
    echo

    FLDRNAME="eftlimits_${YEAR}"

     if [[ $CAT == "0" ]]; then
         FLDRNAME="eft_limits_${YEAR}"
         CATNAME="comb_GGFcateg1GGFcateg2VBFcateg1VBFcateg2"
     else
         FLDRNAME="eft_limits_${YEAR}_${CAT}"
         CATNAME="ggFcateg${CAT}"
     fi


    mkdir $FLDRNAME

    cd ${FLDRNAME}
    combine \
        -M AsymptoticLimits ../../../../prepareModelsEFTNLO/physicsmodel${YEAR}/datacard${YEAR}_${CATNAME}_${eftname}.root -D data_obs  \
        -n _eft_${eftname} \
        --freezeParameters QCDscale_ggHH,pdf_Higgs_gg,mtop_ggHH
        #--freezeNuisanceGroups theory  #This is for including systematics but no theory  
        # --freezeParameters rgx{.*},r  #This is for stat only    
    cd ..
done    
