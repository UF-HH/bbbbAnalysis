### for the single categ GoF
# for y in 2016 20172018; do for c in GGFcateg1 GGFcateg2 VBFcateg1 VBFcateg2; do source make_GoF.sh $y $c ; done ; done

### for the fully combined GoF
# source make_GoF.sh run2 comb_GGFcateg1GGFcateg2VBFcateg1VBFcateg2

year=$1
categ=$2

# nametag=""
nametag="freqToys"

# the full comb does approx 20 toys per minute
nblocks=10
nperblock=100

fol="../physicsmodel${year}"
wsname="datacard${year}_${categ}.root"
THISDIR=`pwd`

echo "... running GoF for $year, $categ"
echo "... will run into $fol"
echo "... will run onto $wsname"
echo "... will run $nblocks x $nperblock toys"

cd $fol

combine -M GoodnessOfFit $wsname --algo=saturated --redefineSignalPOIs r --freezeParameters r_gghh,r_qqhh,CV,C2V,kl,kt -n ${year}_${categ}${nametag}

for i in `seq 1 ${nblocks}`; do
    combine -M GoodnessOfFit $wsname --algo=saturated -t $nperblock -s $i --redefineSignalPOIs r --freezeParameters r_gghh,r_qqhh,CV,C2V,kl,kt -n ${year}_${categ}${nametag} --toysFreq &
done

echo ".... finished, hadding"

### FIXME: here add waiting for PIDs : https://stackoverflow.com/questions/356100/how-to-wait-in-bash-for-several-subprocesses-to-finish-and-return-exit-code-0
# hadd higgsCombine${year}_${categ}${nametag}.GoodnessOfFit.mH120.AllToyRuns.root higgsCombine${year}_${categ}${nametag}.GoodnessOfFit.mH120.*.root

cd $THISDIR
