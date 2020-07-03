YEAR=$1
mkdir bkgtraining
mkdir bkgtraining/mylogs

python DataBackgroundModel.py --config config/outputskim_${YEAR}.cfg --casename NtuplesV7 | tee "bkgtraining/mylogs/training_bkgmodel_${YEAR}v7.txt"
