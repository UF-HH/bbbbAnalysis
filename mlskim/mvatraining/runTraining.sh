
YEAR=$1
TRAININGID=$2

mkdir mylogs

if [ "$TRAININGID" == "0" ] ; then
  ##GGFKiller
  python TrainGGFKiller.py --config ../config/outputskim_${YEAR}.cfg --case NtuplesV7 | tee "mylogs/training_GGFKiller_${YEAR}.txt"
elif [ "$TRAININGID" == "1" ]; then
  ##MVA Cat1
  python TrainGGFMVANLO.py --config ../config/outputskim_${YEAR}.cfg --case NtuplesV7 --categ 1 | tee "mylogs/training_GGFMVA1_${YEAR}.txt"
elif [ "$TRAININGID" == "2" ]; then
  ##MVA Cat2
  python TrainGGFMVANLO.py --config ../config/outputskim_${YEAR}.cfg --case NtuplesV7 --categ 2 | tee "mylogs/training_GGFMVA2_${YEAR}.txt"
else
  echo "[ERROR] Wrong training id!"
fi
