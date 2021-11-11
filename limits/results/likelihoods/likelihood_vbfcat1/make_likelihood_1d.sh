### MAKE LIKELIHOOD SCAN

#Inputs: Year and coupling
YEAR=$1
COUPLING=$2

echo
echo " ============================================== "
echo "... running likelihood on $COUPLING coupling"
echo " ============================================== "
echo

#Tell combine to get blind likelihood
FLDRNAME="${COUPLING}_likelihood_${YEAR}"
mkdir $FLDRNAME
cd ${FLDRNAME}
if [[ $COUPLING == 'C2V' ]]; then
   combine -M MultiDimFit  ../../../../prepareModelsNNLO/physicsmodel${YEAR}/datacard${YEAR}_VBFcateg1.root -D data_obs --redefineSignalPOIs C2V --setParameters r=1,r_gghh=1,r_qqhh=1,CV=1,kl=1,kt=1  --freezeParameters r,r_qqhh,r_gghh,CV,kl,kt  --algo grid --points=100 --setParameterRanges C2V=-5,7
elif [[ $COUPLING == 'CV' ]]; then
   combine -M MultiDimFit  ../../../../prepareModelsNNLO/physicsmodel${YEAR}/datacard${YEAR}_VBFcateg1.root -D data_obs --redefineSignalPOIs CV  --setParameters r=1,r_gghh=1,r_qqhh=1,C2V=1,kl=1,kt=1 --freezeParameters r,r_qqhh,r_gghh,C2V,kl,kt --algo grid --points=100 --setParameterRanges CV=-3,3
elif [[ $COUPLING == 'KL' ]]; then
   combine -M MultiDimFit  ../../../../prepareModelsNNLO/physicsmodel${YEAR}/datacard${YEAR}_VBFcateg1.root -D data_obs --redefineSignalPOIs kl --setParameters r=1,r_gghh=1,r_qqhh=1,CV=1,C2V=1,kt=1  --freezeParameters r,r_qqhh,r_gghh,CV,C2V,kt --algo grid --points=100 --setParameterRanges kl=-28,28
else
	"[ERROR] Coupling was correct!"
fi	
cd ..
