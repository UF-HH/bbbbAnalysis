YEAR=$1
mkdir bkgtraining
mkdir bkgtraining/mylogs

python GridDataBackgroundModel.py --config config/outputskim_${YEAR}.cfg --casename NtuplesV7 | tee "bkgtraining/mylogs/gridtraining_bkgmodel_${YEAR}.txt"
