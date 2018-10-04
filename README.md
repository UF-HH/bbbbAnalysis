# bbbbAnalysis

INFO: the code in this branch (YR2018-proj) features small modifications to run on the Delphes-based ntuples for the 2018 CERN YR HH bbbb projection.
Only the histogram filler code is used.
This branch is *not* meant to be merged with the master code for the Run 2 analysis.

## Install instructions
```
cmsrel CMSSW_9_4_1
cd CMSSW_9_4_1/src
cmsenv
git clone https://github.com/UF-HH/bbbbAnalysis
```

## Setup and compile
```
# from bbbbAnalysis/
cmsenv
source scripts/setup.sh # only needed once for every new shell
make exe -j # compiles and makes everything under test/ executable
````

## Make a skim of NanoAOD
```
## please, fill me with some examples of usage
````

## Fill histograms from skims
```
fill_histograms.exe config/<config_name>.cfg
````
