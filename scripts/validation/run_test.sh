## FIXME: can try to extract the test dir from the setup command variables
## to avoid having to launch from this folder
RUN_TEST_DIR=`pwd`
cd ../..

echo "... launching test skim"

skim_ntuple.exe \
--cfg config/skim_2016ResonantDiHiggs4B.cfg \
--input $RUN_TEST_DIR/VBF_input.txt \
--output test_bbbb_tree.root \
--xs 10 \
--is-VBF-sig

echo "... done"

cd $RUN_TEST_DIR
