YEAR=$1

CR=$2 

mkdir bkgtraining
mkdir bkgtraining/mylogs
mkdir bkgtraining/mymodels

python DataBackgroundModel.py --config config/outputskim_${YEAR}.cfg --eos --casename NtuplesPA --crdefinition ${CR} | tee "bkgtraining/mylogs/training_v7_bkgmodel_${YEAR}_forshapes.txt"

#OPTIONS: --config --casename --eos