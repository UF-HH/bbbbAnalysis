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
   combine -M MultiDimFit  ../../../prepareModels/physicsmodel${YEAR}/datacard${YEAR}_comb_GGFcateg1GGFcateg2VBFcateg1VBFcateg2.root -D data_obs --redefineSignalPOIs C2V --setParameters r=1,r_gghh=0,r_qqhh=1,CV=1,kl=1,kt=1  --freezeParameters r,r_qqhh,r_gghh,CV,kl,kt --algo grid --points=200
elif [[ $COUPLING == 'CV' ]]; then
   combine -M MultiDimFit  ../../../prepareModels/physicsmodel${YEAR}/datacard${YEAR}_comb_GGFcateg1GGFcateg2VBFcateg1VBFcateg2.root -D data_obs --redefineSignalPOIs CV  --setParameters r=1,r_gghh=0,r_qqhh=1,C2V=1,kl=1,kt=1 --freezeParameters r,r_qqhh,r_gghh,C2V,kl,kt --algo grid --points=200
elif [[ $COUPLING == 'C3' ]]; then
   combine -M MultiDimFit  ../../../prepareModels/physicsmodel${YEAR}/datacard${YEAR}_comb_GGFcateg1GGFcateg2VBFcateg1VBFcateg2.root -D data_obs --redefineSignalPOIs kl  --setParameters r=1,r_gghh=0,r_qqhh=1,CV=1,C2V=1,kt=1 --freezeParameters r,r_qqhh,r_gghh,CV,C2V,kt --algo grid --points=200
else
	"[ERROR] Coupling was correct!"
fi	
cd ..
