### make upper limits
YEAR=$1

for cv in `seq -f %.2f -3.0 0.05 3.0`; do
#for cv in `seq -f %.2f 1.00 1.0 1.00`; do
     cvname=${cv}
     if [[ $cvname == *-* ]]; then
         cvname=${cvname/-/m_};
     else
         cvname=p_${cvname}
     fi
    
    echo
    echo " ============================================== "
    echo "... running on cv $cv ($cvname)"
    echo " ============================================== "
    echo

    FLDRNAME="cv_limits_${YEAR}"

    mkdir $FLDRNAME

    cd ${FLDRNAME}
    combine \
        -M AsymptoticLimits ../../../prepareModels/physicsmodel${YEAR}/datacard${YEAR}_comb_GGFcateg1GGFcateg2VBFcateg1VBFcateg2.root -D data_obs --redefineSignalPOIs r_qqhh \
        --setParameters r_gghh=1,kt=1,kl=1,C2V=1,CV=${cv} \
        -n cv_${cvname} \
         --freezeParameters THU_HH,pdf_Higgs_gg,mtop_ggHH,QCDscale_qqHH,pdf_Higgs_qq,BR_decay_hbbhbb,r  #This is for including systematics  
         # --freezeParameters rgx{.*},r  #This is for stat only        
    cd ..
done    
