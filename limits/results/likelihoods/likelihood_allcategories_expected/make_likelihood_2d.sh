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
   combine -M MultiDimFit -t -1  ../../../../prepareModelsNNLO/physicsmodel${YEAR}/datacard${YEAR}_comb_GGFcateg1GGFcateg2VBFcateg1VBFcateg2.root -D data_obs --redefineSignalPOIs CV,C2V --setParameters r=1,r_gghh=1,r_qqhh=1,kl=1,kt=1 --freezeParameters r,r_qqhh,r_gghh,kl,kt --algo grid --points=10000 --setParameterRanges C2V=-4.0,23.0:CV=-5.0,5.0
elif [[ $COUPLING_COMB == '2' ]]; then
   echo "   --- The coupling combination is kl and C2V"	
   FLDRNAME="C2V_KL_likelihood_${YEAR}"
   mkdir $FLDRNAME
   cd ${FLDRNAME}	
   combine -M MultiDimFit -t -1  ../../../../prepareModelsNNLO/physicsmodel${YEAR}/datacard${YEAR}_comb_GGFcateg1GGFcateg2VBFcateg1VBFcateg2.root -D data_obs --redefineSignalPOIs C2V,kl --setParameters r=1,r_gghh=1,r_qqhh=1,CV=1,kt=1 --freezeParameters r,r_qqhh,r_gghh,CV,kt --algo grid --points=1000 --setParameterRanges C2V=-1.5,3.0:kl=-7,14
elif [[ $COUPLING_COMB == '3' ]]; then
   echo "   --- The coupling combination is CV and kl"	
   FLDRNAME="CV_KL_likelihood_${YEAR}"
   mkdir $FLDRNAME
   cd ${FLDRNAME}	
   combine -M MultiDimFit -t -1  ../../../../prepareModelsNNLO/physicsmodel${YEAR}/datacard${YEAR}_comb_GGFcateg1GGFcateg2VBFcateg1VBFcateg2.root -D data_obs --redefineSignalPOIs kl,CV --setParameters r=1,r_gghh=1,r_qqhh=1,C2V=1,kt=1 --freezeParameters r,r_qqhh,r_gghh,C2V,kt --algo grid --points=5000 --setParameterRanges CV=-3,3:kl=-28,28
elif [[ $COUPLING_COMB == '4' ]]; then
   echo "   --- The coupling combination is KL and KT"
   FLDRNAME="KL_KT_likelihood_${YEAR}"
   mkdir $FLDRNAME
   cd ${FLDRNAME}
   combine -M MultiDimFit -t -1  ../../../../prepareModelsNNLO/physicsmodel${YEAR}/datacard${YEAR}_comb_GGFcateg1GGFcateg2VBFcateg1VBFcateg2.root -D data_obs --redefineSignalPOIs kt,kl --setParameters r=1,r_gghh=1,r_qqhh=1,CV=1,C2V=1 --freezeParameters r,r_qqhh,r_gghh,C2V,CV --algo grid --points=5000 --setParameterRanges kt=-10,10:kl=-28,28
else
	"[ERROR] Coupling combination was not correct!"
fi	
cd ..
