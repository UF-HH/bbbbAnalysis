YEAR=$1

python Outputskim.py --config config/outputskim_${YEAR}.cfg --casename NtuplesV7 --lepveto  --signal --data  --systs # --mcbkgs 
