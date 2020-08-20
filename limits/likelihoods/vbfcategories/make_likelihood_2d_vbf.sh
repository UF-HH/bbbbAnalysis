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
   echo "   --- The coupling combination is C2V and CV"	
   FLDRNAME="C2V_CV_likelihood_${YEAR}"
   mkdir $FLDRNAME
   cd ${FLDRNAME}	
   combine -M MultiDimFit -t -1  ../../../prepareModels/physicsmodel${YEAR}/datacard${YEAR}_comb_VBFcateg1VBFcateg2.root -D data_obs --redefineSignalPOIs CV,C2V --setParameters r=1,r_gghh=0,r_qqhh=1,kl=1,kt=1 --freezeParameters r,r_qqhh,r_gghh,kl,kt --algo grid --points=10000
elif [[ $COUPLING_COMB == '2' ]]; then
   echo "   --- The coupling combination is C2V and C3=kl"	
   FLDRNAME="C2V_C3_likelihood_${YEAR}"
   mkdir $FLDRNAME
   cd ${FLDRNAME}	
   combine -M MultiDimFit -t -1  ../../../prepareModels/physicsmodel${YEAR}/datacard${YEAR}_comb_VBFcateg1VBFcateg2.root -D data_obs --redefineSignalPOIs kl,C2V --setParameters r=1,r_gghh=0,r_qqhh=1,CV=1,kt=1 --freezeParameters r,r_qqhh,r_gghh,CV,kt --algo grid --points=1000
elif [[ $COUPLING_COMB == '3' ]]; then
   echo "   --- The coupling combination is CV and C3=kl"	
   FLDRNAME="CV_C3_likelihood_${YEAR}"
   mkdir $FLDRNAME
   cd ${FLDRNAME}	
   combine -M MultiDimFit -t -1  ../../../prepareModels/physicsmodel${YEAR}/datacard${YEAR}_comb_VBFcateg1VBFcateg2.root -D data_obs --redefineSignalPOIs kl,CV --setParameters r=1,r_gghh=0,r_qqhh=1,C2V=1,kt=1 --freezeParameters r,r_qqhh,r_gghh,C2V,kt --algo grid --points=1000
else
	"[ERROR] Coupling combination was not correct!"
fi	
cd ..