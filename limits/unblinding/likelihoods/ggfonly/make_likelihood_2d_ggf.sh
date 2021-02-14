### MAKE LIKELIHOOD SCAN

#Inputs: Year and coupling
YEAR=$1
COUPLING_COMB=$2

echo
echo " ============================================================ "
echo "... running likelihood on coupling combination $COUPLING_COMB "
echo " ============================================================ "
echo

if [[ $COUPLING_COMB == '1' ]]; then
   echo "   --- The coupling combination is KL and KT"	
   FLDRNAME="KL_KT_likelihood_${YEAR}"
   mkdir $FLDRNAME
   cd ${FLDRNAME}	
   combine -M MultiDimFit ../../../prepareModels/physicsmodel${YEAR}/datacard${YEAR}_comb_GGFcateg1GGFcateg2VBFcateg1VBFcateg2.root -D data_obs --redefineSignalPOIs kt,kl --setParameters r=1,r_gghh=1,r_qqhh=0,CV=1,C2V=1 --freezeParameters r,r_qqhh,r_gghh,C2V,CV --algo grid --points=5000
else
	"[ERROR] Coupling combination was not correct!"
fi	
cd ..
