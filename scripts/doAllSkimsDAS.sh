### HH xs : 31.05, H->bb BR = 0.5824 --> xs * BR(bbbb) = 10.53
skim_for_das.exe --input inputFiles/CMSDAS/Samples2016/gg_HH_bbbb_SM.txt --output gg_HH_bbbb.root --xs 10.53 --is-signal --cfg config/skim_CMSDAS_2016DatMC.cfg

skim_for_das.exe --input inputFiles/CMSDAS/Samples2016/data_BTagCSV_Run2016B_ver1.txt --output data_BTagCSV_Run2016B_ver1.root --is-data --cfg config/skim_CMSDAS_2016DatMC.cfg
skim_for_das.exe --input inputFiles/CMSDAS/Samples2016/data_BTagCSV_Run2016B_ver2.txt --output data_BTagCSV_Run2016B_ver2.root --is-data --cfg config/skim_CMSDAS_2016DatMC.cfg
skim_for_das.exe --input inputFiles/CMSDAS/Samples2016/data_BTagCSV_Run2016C.txt      --output data_BTagCSV_Run2016C.root      --is-data --cfg config/skim_CMSDAS_2016DatMC.cfg
skim_for_das.exe --input inputFiles/CMSDAS/Samples2016/data_BTagCSV_Run2016D.txt      --output data_BTagCSV_Run2016D.root      --is-data --cfg config/skim_CMSDAS_2016DatMC.cfg
skim_for_das.exe --input inputFiles/CMSDAS/Samples2016/data_BTagCSV_Run2016E.txt      --output data_BTagCSV_Run2016E.root      --is-data --cfg config/skim_CMSDAS_2016DatMC.cfg
skim_for_das.exe --input inputFiles/CMSDAS/Samples2016/data_BTagCSV_Run2016F.txt      --output data_BTagCSV_Run2016F.root      --is-data --cfg config/skim_CMSDAS_2016DatMC.cfg
skim_for_das.exe --input inputFiles/CMSDAS/Samples2016/data_BTagCSV_Run2016G.txt      --output data_BTagCSV_Run2016G.root      --is-data --cfg config/skim_CMSDAS_2016DatMC.cfg
skim_for_das.exe --input inputFiles/CMSDAS/Samples2016/data_BTagCSV_Run2016H.txt      --output data_BTagCSV_Run2016H.root      --is-data --cfg config/skim_CMSDAS_2016DatMC.cfg
