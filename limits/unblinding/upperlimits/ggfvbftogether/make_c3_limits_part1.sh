### make upper limits
YEAR=$1

for c3 in `seq -f %.1f -12.5 0.5 1.0`; do
#for c3 in `seq -f %.1f 1.0 1.0 1.0`; do
     c3name=${c3}
     if [[ $c3name == *-* ]]; then
         c3name=${c3name/-/m_};
     else
         c3name=p_${c3name}
     fi
    
    echo
    echo " ============================================== "
    echo "... running on c3 $c3 ($c3name)"
    echo " ============================================== "
    echo

    FLDRNAME="c3limits_${YEAR}"

    mkdir $FLDRNAME

    cd ${FLDRNAME}
    combine \
        -M AsymptoticLimits ../../../prepareModels/physicsmodel${YEAR}/datacard${YEAR}_comb_GGFcateg1GGFcateg2VBFcateg1VBFcateg2.root -D data_obs --redefineSignalPOIs r \
        --setParameters r_gghh=1,r_qqhh=1,kt=1,kl=${c3},C2V=1,CV=1 \
        -n kl_${c3name} \
        --freezeParameters THU_HH,pdf_Higgs_gg,mtop_ggHH,QCDscale_qqHH,pdf_Higgs_qq,BR_decay_hbbhbb  #This is for including systematics  
        # --freezeParameters rgx{.*},r  #This is for stat only     
    cd ..
done    
