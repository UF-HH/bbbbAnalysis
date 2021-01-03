### make upper limits
YEAR=$1

for cv in `seq -f %.2f -3.0 0.05 3.0`; do
#for cv in `seq -f %.2f -1.52 1.0 -1.52`; do
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
        -M AsymptoticLimits --run blind ../../../prepareModels/physicsmodel${YEAR}/datacard${YEAR}_comb_VBFcateg1VBFcateg2.root -D data_obs --redefineSignalPOIs r \
        --setParameters r_gghh=0,r_qqhh=1,kt=1,kl=1,C2V=1,CV=${cv} \
        -n cv_${cvname} \
        --rMax 2000 --freezeNuisanceGroups theory \
        #--freezeParameters allConstrainedNuisances        
    cd ..
done    
