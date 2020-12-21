YEAR=$1
mkdir bkgtraining
mkdir bkgtraining/mylogs
mkdir bkgtraining/mymodels

python DataBackgroundModel.py --config config/outputskim_${YEAR}.cfg --casename NtuplesPA | tee "bkgtraining/mylogs/training_v7_bkgmodel_${YEAR}_ntuplespa_tthstudies.txt"

#OPTIONS: --config --casename --eos