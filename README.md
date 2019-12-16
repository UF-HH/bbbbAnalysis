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

## Machine learning for background modeling and discriminator training using pandas dataframes
After we produced the bbbb_ntuple skims of data and simulation, the next step is to use machine learning techniques to train a discriminator (e.g. BDT or DNN) or create a data-driven estimation of the HH backgrounds. These techniques are developed and executed inside mlskim directory. The two main python scripts inside mlskim folder are Outputskim.py, DataBackgroundModel.py and MCBackgroundModel.py. 

We have three groups of data and MC samples for Run 2 data analysis (2016, 2017 & 2018). Therefore, after producing the ntuples for each year, it is convenient to place them under the same directory in eos. For instance, we put my 2016 bbbb_ntuples as "New2016" in /eos/uscms/store/user/guerrero/bbbb_ntuples/FullNtuples/ directory, and the same for the other years. Moreover, for convenience, we merge (hadd!) the bbbbntuple files associated to each MC process or data in a single file. This is done by running the script in the inputskims. Note that the script should be adapted to eos location, username, etc.
```
cd mlskim/inputskims
source haddsamples.sh {eosname} ### In our case eosname is FullNtuples
````
The Outputskim.py code is able to process the inputskims files in data and MC samples using panda dataframes. It can take only the branches that we are interested in for the developments. These branches of interest are specified as 'variables' in the config files outputskim_201*.cfg in the folder config, and the inputskims of interest are specified as 'samples'. One can also create new branches if needed by editing the Outputskim.py script. To execute this process simply run the script:
```
source runOutputskim.sh

````
The DataBackgroundModel.py code creates a data-driven background model taking as input the control region information in 3-btag and 4 -btag data. This method is based on the BDT-reweighter method. The regions to be used for the training as defined in modules/selections.py. The parameters of the BDT-reweighter are defined in the respective config files. The script creates four weights: Weight_AnaGGF, Weight_AnaVBF, Weight_ValGGF and Weight_ValVBF, where Ana=AnalysisRegion and Val=ValidationRegion. These weights are stored as branches in the output file (SKIM_MODEL_BKG.root). To run the background modeling:
```
source runDataBkgModel.sh

````
## Running combine
Log on a CentOS 7 machine (lxplus, cmslpc-sl7) and install combine following the instructions [here](https://cms-analysis.github.io/HiggsAnalysis-CombinedLimit/#for-end-users-that-dont-need-to-commit-or-do-any-development)

After compiling  CMSSW, do ``git clone https://github.com/UF-HH/bbbbAnalysis`` (NB: no need to compile the code with scram and make exe since it will only run combine).

Scripts for running combine are under ``limits``.

