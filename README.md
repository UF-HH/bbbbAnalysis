# bbbbAnalysis

## Install instructions
```
cmsrel CMSSW_10_2_5
cd CMSSW_10_2_5/src
cmsenv
git cms-addpkg PhysicsTools/KinFitter CommonTools/Utils CondFormats/JetMETObjects CondFormats/Serialization FWCore/MessageLogger FWCore/Utilities JetMETCorrections/Modules
scram b -j 8
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
