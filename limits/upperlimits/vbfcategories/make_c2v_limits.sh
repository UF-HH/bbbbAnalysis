### make upper limits
YEAR=$1 #2016/2017/2018/run2
CAT=$2  #0/1/2

for c2v in `seq -f %.1f -5.0 0.1 7.0`; do
#for c2v in `seq -f %.2f 0.0 1.0 2.0`; do
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
         CATNAME="comb_VBFcateg1VBFcateg2"
     else
         FLDRNAME="c2v_limits_${YEAR}_${CAT}"
         CATNAME="VBFcateg${CAT}"
     fi

    mkdir $FLDRNAME

    cd ${FLDRNAME}
    combine \
        -M AsymptoticLimits --run blind ../../../prepareModels/physicsmodel${YEAR}/datacard${YEAR}_${CATNAME}.root -D data_obs --redefineSignalPOIs r \
        --setParameters r_gghh=0,r_qqhh=1,kt=1,kl=1,CV=1,C2V=${c2v} \
         -n c2v_${c2vname} \
         --rMax 2000 --freezeNuisanceGroups theory \
         # --freezeParameters allConstrainedNuisances
    cd ..
done    
