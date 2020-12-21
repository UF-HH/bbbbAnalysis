# MLSKIM Instructions 

## First step: Prepare the input skims
After we produced the bbbb_ntuple skims of data and simulation, the next step is to use machine learning techniques to train a discriminator (e.g. BDT or DNN) or create a data-driven estimation of the HH backgrounds. These techniques are developed and executed inside mlskim directory. The two main python scripts inside the mlskim folder are Outputskim.py and DataBackgroundModel.py. 

We have three groups of data and MC samples for Run 2 data analysis (2016, 2017 & 2018). Therefore, after producing the ntuples for each year, it is convenient to place them under the same directory in eos, say '/eos/uscms/store/user/user/bbbb_ntuples/Ntuples/'. For instance, we put the 2016 bbbb_ntuples in a folder called "2016" under Ntuples as /eos/uscms/store/user/bbbb_ntuples/Ntuples/2016, and the same for the other years.

## Second step: Run the outputskims
The Outputskim.py code is able to process the inputskims files in data and MC samples using panda dataframes. It can take only the branches that we are interested in for the developments and final signal extraction. All list of samples, variables, weights, youreosdirectory, mvafiles are defined in the config files outputskim_201*.cfg in the folder config.

The runOutputskim.sh script runs Outputskim.py and contains the multiple options to customize the outputskims:
```
--config      : This is the config file, say config/outputskim_<YEAR>.cfg
--casename    : This is the name of the folder with the ntuples on eos, say Ntuples
--eos         : Store outputskims in eos 
--data        : Run on Data
--signal      : Run on Signals
--mcbkgs      : Run on MC background
--othersignal : Run on alternative signals
--systs       : Run systematic variations
--lepveto     : Apply lepton veto cut
--trigmat     : Apply trigger matching cut
````

After defining the options that want to use in runOutputskim.sh, to process the inputskims to outputskims one simply run the script:
```
source runOutputskim.sh $year
````
This will create the output skims either on eos or locally in a folder called 'outputskims'. A subfolder with the year name will be created as well and will contain the files.

## Third step: Run the background model
The DataBackgroundModel.py code creates a data-driven background model taking as input the control region information in 3-btag and 4 -btag data. This method is based on the BDT-reweighter method (https://arxiv.org/abs/1608.05806) and uses the python package provided by the developers called hep_ml (https://arogozhnikov.github.io/hep_ml/). 

The regions used for the training of the model are defined in modules/selections.py. The parameters of the BDT-reweighter are included in the config files. The script creates four weights (where Ana=AnalysisRegion and Val=ValidationRegion): Weight_AnaGGF@, Weight_AnaVBF@, Weight_ValGGF@ and Weight_ValVBF@, where @=1,2 categories. These weights are stored as branches in the output file (SKIM_MODEL_BKG.root). To run the background modeling:

The runDataBkgmodel.sh script runs DataBackgroundModel.py and contains the multiple options to customize the bkgmodeling:
```
--config      : This is the config file, say config/outputskim_<YEAR>.cfg
--casename    : This is the name of the folder with the ntuples on eos, say Ntuples
--eos         : Store outputskims in eos 
````

After defining the options that want to use in runDataBkgmodel.sh, to process the bkgmodel one simply run the script:
```
source runDataBkgmodel.sh $year #This will create the file outputskims/$year/SKIM_MODEL_BKG.root
````

Note: To obtain the 2017+2018 combined background model, first you need to hadd the 2017 and 2018 data. Next place the file in a new directory '20172018' under outputskims.

Then, run the modeling as following:
```
source runDataBkgmodel.sh 20172018
````