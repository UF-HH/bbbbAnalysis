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

## Make plots
Use the ``plotter/plotter.py`` script. Styles (line colors, etc.) for the processes are defined in ``plotter/styles/plotStyles.py``
Inside the script, the subset of processes to run on is defined through ``bkgToPlot`` and  ``sigToPlot``.
Several cmd line options available to configure the plot, it's practical to make a script that produces all the plots.
Example:
```
source do_all_plots.sh
````

## Machine learning skims using pandas dataframes
All developments (background modeling and bdt/dnn discriminator training) take place inside mlskim directory. For Run 2 we have three groups of data and MC samples (2016, 2017 & 2018). Therefore, after producing the ntuples for each year, it is convenient to put them under the same directory in eos. For instance, I put my 2016 bbbb_ntuples as "New2016" in /eos/uscms/store/user/guerrero/bbbb_ntuples/FullNtuples/ directory. Similarly, this is done for the other years. Next, we merge (hadd!) the bbbbntuple files associated to each MC process or data in a single file. This is done by running the script in the inputskims folder:
```
cd mlskim/inputskims
source haddsamples.sh {eosname} ### In my case eosname is FullNtuples

````
Next, one finds two main python scripts inside mlskim folder: Outputskim.py and DataBackgroundModel.py. The Outputskim.py code is able to skim over the inputskims files in data and MC samples using panda dataframes. It takes only the branches that we are interested in for the developments. The branches are specified in the config files in the folder config. It can also create new branches if needed. To execute the outputskims one runs the script:
```
source runOutputskim.sh

````
Then, the DataBackgroundModel.py code creates a data-driven background model taking as input the control region information in 3-btag and 4 -btag data. The parameters of the BDT-reweighter are defined in the config files. The script creates four weights (Weight_210_GGF,Weight_210_VBF,Weight_110_GGF,Weight_110_VBF) as branches in the output file (SKIM_MODEL_BKG.root). To run the background modeling:
```
source runDataBkgModel.sh

````
## Running combine
Log on a CentOS 7 machine (lxplus, cmslpc-sl7) and install combine following the instructions [here](https://cms-analysis.github.io/HiggsAnalysis-CombinedLimit/#for-end-users-that-dont-need-to-commit-or-do-any-development)

After compiling  CMSSW, do ``git clone https://github.com/UF-HH/bbbbAnalysis`` (NB: no need to compile the code with scram and make exe since it will only run combine).

Scripts for running combine are under ``limits``.

