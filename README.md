# bbbbAnalysis

## Install instructions
```
cmsrel CMSSW_9_4_1
cd CMSSW_9_4_1/src
cmsenv
git clone https://github.com/l-cadamuro/bbbbAnalysis
```

## Setup and compile
```
# from bbbbAnalysis/
cmsenv
source scripts/setup.sh # only needed once for every new shell
make exe -j # compiles and makes everything under test/ executable
````
