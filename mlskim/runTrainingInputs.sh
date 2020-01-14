###########First prepare just the signal
python MCBackgroundModel.py --config config/outputskim_2016.cfg --sample 0 --casename TrainingSamples
python MCBackgroundModel.py --config config/outputskim_2017.cfg --sample 0 --casename TrainingSamples
python MCBackgroundModel.py --config config/outputskim_2018.cfg --sample 0 --casename TrainingSamples
###########Second prepare just the datadriven background
python MCBackgroundModel.py --config config/outputskim_2016.cfg --sample 2 --casename TrainingSamples
python MCBackgroundModel.py --config config/outputskim_2017.cfg --sample 2 --casename TrainingSamples
python MCBackgroundModel.py --config config/outputskim_2018.cfg --sample 2 --casename TrainingSamples