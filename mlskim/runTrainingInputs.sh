###########First prepare just the signal
python MCBackgroundModel.py --config config/outputskim_2016.cfg --sample 0 --casename BregforBDT3BSM
python MCBackgroundModel.py --config config/outputskim_2017.cfg --sample 0 --casename BregforBDT3BSM
python MCBackgroundModel.py --config config/outputskim_2018.cfg --sample 0 --casename BregforBDT3BSM
###########Second prepare just the background
python MCBackgroundModel.py --config config/outputskim_2016.cfg --sample 1 --casename BregforBDT3BSM 
python MCBackgroundModel.py --config config/outputskim_2017.cfg --sample 1 --casename BregforBDT3BSM 
python MCBackgroundModel.py --config config/outputskim_2018.cfg --sample 1 --casename BregforBDT3BSM