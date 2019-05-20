## FIXME: can try to extract the test dir from the setup command variables
## to avoid having to launch from this folder
RUN_TEST_DIR=`pwd`
cd ../..

echo "... launching test skim"

# --puWeight=weights/2016/VBFHHTo4B_CV_1_C2V_1_C3_1_13TeV-madgraph_PUweights.root                   

# skim_ntuple.exe \
# skim_ntuple_nonresonant.exe \
# --cfg $RUN_TEST_DIR/config/skim_validation.cfg \
# --input $RUN_TEST_DIR/inputfiles/input_VBF_SM_HH_2016MC.txt \
# --output $RUN_TEST_DIR/output/validation_VBF_SM_HH_2016MC.root \
# --xs 0.00058 \
# --is-VBF-sig \
# --puWeight weights/test_PU/test_PUweights.root
# # --is-signal \

skim_ntuple_nonresonant.exe \
--cfg $RUN_TEST_DIR/config/skim_2016NonResonantDiHiggs4B.cfg \
--input $RUN_TEST_DIR/inputfiles/input_VBF_SM_HH_2016MC.txt \
--output $RUN_TEST_DIR/output/validation_VBF_SM_HH_2016MC.root \
--xs 0.00058 \
--is-VBF-sig \
--puWeight weights/test_PU/test_PUweights.root
# --is-signal \


echo "... done"

cd $RUN_TEST_DIR


##### problems encountered
# 1) nonres cfg does not work for resonant skimmer > problemi con il parametri VBF
# 2) non ci sono i file di PU per i vari sample > rimosso PUweights dal cfg
# 3) running with is-VBF-sig (but not is-sig) throws an error of
# skim_ntuple_nonresonant.exe: /cvmfs/sft.cern.ch/lcg/views/LCG_89/x86_64-slc6-gcc62-opt/include/boost/optional/optional.hpp:1117: boost::optional<T>::pointer_type boost::optional<T>::operator->() [with T = Jet; boost::optional<T>::pointer_type = Jet*]: Assertion `this->is_initialized()' failed.
# Aborted
# >..> this problem is given by the following lines added by Fabio: https://github.com/UF-HH/bbbbAnalysis/blob/master/src/OfflineProducerHelper.cc#L2266
# because Daniel's code selects the bbbb gen *before* the reco jets
# matching there should be added somewhere separate