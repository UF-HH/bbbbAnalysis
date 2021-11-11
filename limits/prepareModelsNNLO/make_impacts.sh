#Define the year and the production mode

YEAR=$1
CATEG=$2
SIGNAL=$3

echo $CATEG


if  [[ $CATEG == '1' ]]; then
   WSPACE=datacard${YEAR}_comb_GGFcateg1GGFcateg2.root
   FOLDER=physicsmodel${YEAR}
   EXPECT_SIGNAL=$SIGNAL
   HERE=`pwd`
   echo "... working in", $FOLDER
   cd ${FOLDER}
   combineTool.py -M Impacts -d ${WSPACE}  --doInitialFit --robustFit 1 --setParameters r_gghh=1,r_qqhh=0,kl=1,CV=1,C2V=1,kt=1 --freezeParameters r_gghh,r_qqhh,CV,C2V,kl,kt --setParameterRanges r=-20,20 --redefineSignalPOIs r -m 125 --expectSignal ${EXPECT_SIGNAL} -t -1
   combineTool.py -M Impacts -d ${WSPACE}  --doFits       --robustFit 1 --setParameters r_gghh=1,r_qqhh=0,kl=1,CV=1,C2V=1,kt=1 --freezeParameters r_gghh,r_qqhh,CV,C2V,kl,kt --setParameterRanges r=-20,20 --redefineSignalPOIs r -m 125 --expectSignal ${EXPECT_SIGNAL} -t -1 --parallel 4
   combineTool.py -M Impacts -d ${WSPACE}  -o impacts_sig_inj${EXPECT_SIGNAL}_ggf.json --setParameters r_gghh=1,r_qqhh=0,kl=1,CV=1,C2V=1,kt=1 --freezeParameters r_gghh,r_qqhh,CV,C2V,kl,kt --setParameterRanges r=-20,20 --redefineSignalPOIs r -m 125
   plotImpacts.py -i impacts_sig_inj${EXPECT_SIGNAL}_ggf.json -o impacts_sig_inj${EXPECT_SIGNAL}_ggf
   cd $HERE
elif [[ $CATEG == '2' ]]; then
   WSPACE=datacard${YEAR}_comb_VBFcateg1VBFcateg2.root
   FOLDER=physicsmodel${YEAR}
   EXPECT_SIGNAL=$SIGNAL
   HERE=`pwd`
   echo "... working in", $FOLDER
   cd ${FOLDER}
   combineTool.py -M Impacts -d ${WSPACE}  --doInitialFit --robustFit 1 --setParameters r_gghh=0,r_qqhh=1,kl=1,CV=1,C2V=1,kt=1 --freezeParameters r_gghh,r_qqhh,CV,C2V,kl,kt --setParameterRanges r=-1000,1000 --redefineSignalPOIs r -m 125 --expectSignal ${EXPECT_SIGNAL} -t -1
   combineTool.py -M Impacts -d ${WSPACE}  --doFits       --robustFit 1 --setParameters r_gghh=0,r_qqhh=1,kl=1,CV=1,C2V=1,kt=1 --freezeParameters r_gghh,r_qqhh,CV,C2V,kl,kt --setParameterRanges r=-1000,1000 --redefineSignalPOIs r -m 125 --expectSignal ${EXPECT_SIGNAL} -t -1 --parallel 4
   combineTool.py -M Impacts -d ${WSPACE}  -o impacts_sig_inj${EXPECT_SIGNAL}_vbf.json --setParameters r_gghh=0,r_qqhh=1,kl=1,CV=1,C2V=1,kt=1 --freezeParameters r_gghh,r_qqhh,CV,C2V,kl,kt --setParameterRanges r=-1000,1000 --redefineSignalPOIs r -m 125
   plotImpacts.py -i impacts_sig_inj${EXPECT_SIGNAL}_vbf.json -o impacts_sig_inj${EXPECT_SIGNAL}_vbf
   cd $HERE
else
    	"[ERROR] Incorrect category!"
fi
