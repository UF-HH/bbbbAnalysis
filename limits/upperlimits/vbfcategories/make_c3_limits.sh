### make upper limits
YEAR=$1

for c3 in `seq -f %.1f -28.0 0.5 28.0`; do
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

    FLDRNAME="c3_limits_${YEAR}"

    mkdir $FLDRNAME

    cd ${FLDRNAME}
    combine \
        -M AsymptoticLimits --run blind ../../../prepareModels/physicsmodel${YEAR}/datacard${YEAR}_comb_VBFcateg1VBFcateg2.root -D data_obs --redefineSignalPOIs r \
        --setParameters r_gghh=0,r_qqhh=1,kt=1,kl=${c3},C2V=1,CV=1 \
         -n c3_${c3name} \
         --rMax 2000 --freezeNuisanceGroups theory \
         #--freezeParameters allConstrainedNuisances
    cd ..
done    
