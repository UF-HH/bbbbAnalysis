### make upper limits
YEAR=$1 #run2/2016/20172018
CAT=$2  #0/1/2

for kl in `seq -f %.1f -28.0 0.5 28.0`; do
#for kl in `seq -f %.1f 1.0 1.0 1.0`; do
     klname=${kl}
     if [[ $klname == *-* ]]; then
         klname=${klname/-/m_};
     else
         klname=p_${klname}
     fi
    
    echo
    echo " ============================================== "
    echo "... running on kl $kl ($klname)"
    echo " ============================================== "
    echo

    FLDRNAME="kl_limits_${YEAR}"

     if [[ $CAT == "0" ]]; then
         FLDRNAME="kl_limits_${YEAR}"
         CATNAME="comb_GGFcateg1GGFcateg2"
     else
         FLDRNAME="kl_limits_${YEAR}_${CAT}"
         CATNAME="GGFcateg${CAT}"
     fi

    mkdir $FLDRNAME

    cd ${FLDRNAME}
    combine \
        -M AsymptoticLimits --run blind ../../../prepareModels/physicsmodel${YEAR}/datacard${YEAR}_${CATNAME}.root -D data_obs --redefineSignalPOIs r \
        --setParameters r_gghh=1,r_qqhh=0,kt=1,kl=${kl},C2V=1,CV=1 \
          -n kl_${klname} \
          --rMax 200 --freezeNuisanceGroups theory\
        #--freezeParameters allConstrainedNuisances        
    cd ..
done    
