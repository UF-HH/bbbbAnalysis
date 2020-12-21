YEAR=$1

python Outputskim.py --config config/outputskim_${YEAR}.cfg --casename NtuplesPA --lepveto --trigmat --signal --data 

#OPTIONS: --config --casename --eos --data --signal --systs --mcbkgs --othersignal --lepveto --trigmat
