YEAR=$1

#### Step0. Define pythonpath 
export PYTHONPATH=/uscms/home/guerrero/nobackup/Run2/HH2020/Spring/CMSSW_10_2_13/src/bbbbAnalysis/limits/physicsModels

#### Step1. Prepare histograms (no bkg shape)
python prepareHistos.py --config config/outputhistos${YEAR}.cfg

#### Step2. Prepare histograms (bkg shapes)
source runshapes.sh $YEAR

#### Step3. Prepare datacards per category and their combinations then their associated workspace
python makeDatacardsAndWorkspaces.py --config config/outputhistos${YEAR}.cfg