mkdir CLs_$1
#calculate shapes
root -q -b -l 'make_shapes.C(-1,1,"GGHH4B_rew_kl_p_1","GGFcateg","BDT3","2016",100)'
root -q -b -l 'make_shapes.C(-1,1,"VBF_HH","VBFcateg","BDT2","2016",100)'
root -q -b -l 'make_shapes.C(-1,1,"GGHH4B_rew_kl_p_1","GGFcateg","BDT3","2017",100)'
root -q -b -l 'make_shapes.C(-1,1,"VBF_HH","VBFcateg","BDT2","2017",100)'
root -q -b -l 'make_shapes.C(-1,1,"GGHH4B_rew_kl_p_1","GGFcateg","BDT3","2018",100)'
root -q -b -l 'make_shapes.C(-1,1,"VBF_HH","VBFcateg","BDT2","2018",100)'
#Calculate limits
combine -M AsymptoticLimits --freezeParameters allConstrainedNuisances datacard_GGF_HH_GGFcateg_2016.txt     --run blind &> CLs_$1/GGF_HH_GGFcateg_2016_asymptoticCLs.out
combine -M AsymptoticLimits --freezeParameters allConstrainedNuisances datacard_VBF_HH_VBFcateg_2016.txt     --run blind &> CLs_$1/VBF_HH_VBFcateg_2016_asymptoticCLs.out
combine -M AsymptoticLimits --freezeParameters allConstrainedNuisances datacard_GGF_HH_GGFcateg_2017.txt     --run blind &> CLs_$1/GGF_HH_GGFcateg_2017_asymptoticCLs.out
combine -M AsymptoticLimits --freezeParameters allConstrainedNuisances datacard_VBF_HH_VBFcateg_2017.txt     --run blind &> CLs_$1/VBF_HH_VBFcateg_2017_asymptoticCLs.out
combine -M AsymptoticLimits --freezeParameters allConstrainedNuisances datacard_GGF_HH_GGFcateg_2018.txt     --run blind &> CLs_$1/GGF_HH_GGFcateg_2018_asymptoticCLs.out
combine -M AsymptoticLimits --freezeParameters allConstrainedNuisances datacard_VBF_HH_VBFcateg_2018.txt     --run blind &> CLs_$1/VBF_HH_VBFcateg_2018_asymptoticCLs.out
#Now the combination
combineCards.py card2016=datacard_GGF_HH_GGFcateg_2016.txt card2017=datacard_GGF_HH_GGFcateg_2017.txt card2018=datacard_GGF_HH_GGFcateg_2018.txt >     datacard_Run2Combination_GGF_HH_GGFcateg.txt
combineCards.py card2016=datacard_VBF_HH_VBFcateg_2016.txt card2017=datacard_VBF_HH_VBFcateg_2017.txt card2018=datacard_VBF_HH_VBFcateg_2018.txt >     datacard_Run2Combination_VBF_HH_VBFcateg.txt
text2workspace.py datacard_Run2Combination_GGF_HH_GGFcateg.txt
text2workspace.py datacard_Run2Combination_VBF_HH_VBFcateg.txt

combine -M AsymptoticLimits --freezeParameters allConstrainedNuisances datacard_Run2Combination_GGF_HH_GGFcateg.root  --run blind &>     CLs_$1/Run2Combination_GGF_HH_GGFcateg_asymptoticCLs.out
combine -M AsymptoticLimits --freezeParameters allConstrainedNuisances datacard_Run2Combination_VBF_HH_VBFcateg.root  --run blind &>     CLs_$1/Run2Combination_VBF_HH_VBFcateg_asymptoticCLs.out
