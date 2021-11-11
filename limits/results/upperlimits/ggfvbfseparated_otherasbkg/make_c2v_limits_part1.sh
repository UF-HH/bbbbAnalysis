### make upper limits
YEAR=$1 #2016/20172018/run2
CAT=$2  #0=all categories, 1=VBFcateg1, 2=VBFcateg2

for c2v in `seq -f %.1f -5.0 0.1 -1.0`; do
#for c2v in `seq -f %.1f -5.0 0.1 1.0`; do
#for c2v in `seq -f %.1f 1.0 1.0 1.0`; do
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


    FLDRNAME="c2v_limits_${YEAR}"

     if [[ $CAT == "0" ]]; then
         FLDRNAME="c2v_limits_${YEAR}"
         CATNAME="comb_GGFcateg1GGFcateg2VBFcateg1VBFcateg2"
     else
         FLDRNAME="c2v_limits_${YEAR}_${CAT}"
         CATNAME="VBFcateg${CAT}"
     fi

    mkdir $FLDRNAME

    cd ${FLDRNAME}
    combine \
        -M AsymptoticLimits ../../../../prepareModels/physicsmodel${YEAR}/datacard${YEAR}_${CATNAME}.root -D data_obs --redefineSignalPOIs r_qqhh \
        --setParameters r_gghh=1,kt=1,kl=1,CV=1,C2V=${c2v} \
        -n c2v_${c2vname} \
        --freezeParameters THU_HH,pdf_Higgs_gg,mtop_ggHH,QCDscale_qqHH,pdf_Higgs_qq,r #This is for including syst on xs limit #Include BR_decay_hbbhbb if quoting xs*B limit   
        #--freezeParameters rgx{.*},r  #This is for stat only    
    cd ..
done    
