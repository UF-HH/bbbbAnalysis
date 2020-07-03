YEAR=$1

###########First prepare just the signals
python PrepareMVATrainingInputs.py --config config/outputskim_${YEAR}.cfg  --sample 0 --casename NtuplesV7

################Second prepare just the datadriven background

python PrepareMVATrainingInputs.py --config config/outputskim_${YEAR}.cfg --sample 1 --casename NtuplesV7
