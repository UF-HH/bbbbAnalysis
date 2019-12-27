TAG="ntuples_26Dic2019_v4"
OUTDIR="root://cmseos.fnal.gov//store/user/{0}/bbbb_ntuples_CMSDAS_trigger/"

python scripts/submitSkimOnTier3.py --executable bin/skim_trigger_for_das.exe --outputDir ${OUTDIR} --tag=$TAG --input inputFiles/CMSDAS/Samples2016/SingleMuon_Data_forTrigger.txt            --cfg config/skim_CMSDAS_2016Trigger.cfg --njobs 200 --is-data
python scripts/submitSkimOnTier3.py --executable bin/skim_trigger_for_das.exe --outputDir ${OUTDIR} --tag=$TAG --input inputFiles/CMSDAS/Samples2016/MC_TT_TuneCUETP8M2T4_13TeV_forTrigger.txt --cfg config/skim_CMSDAS_2016Trigger.cfg --njobs 10  --puWeight weights/2016/TT_TuneCUETP8M2T4_13TeV-powheg-pythia8_PUweights.root --no-tar --no-xrdcp-tar
