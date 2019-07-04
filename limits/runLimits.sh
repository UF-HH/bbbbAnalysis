rm -r CLs
#calculate shapes
root -q -b -l 'make_shapes.C(-1,1,"GGF_HH","GGFcateg","BDT3","2016",100)'
root -q -b -l 'make_shapes.C(-1,1,"VBF_HH","VBFcateg","BDT2","2016",100)'
root -q -b -l 'make_shapes.C(-1,1,"VBF_HH_BSM","VBFcateg","BDT2","2016",100)'
root -q -b -l 'make_shapes.C(-1,1,"GGF_HH","GGFcateg","BDT3","2017",100)'
root -q -b -l 'make_shapes.C(-1,1,"VBF_HH","VBFcateg","BDT2","2017",100)'
root -q -b -l 'make_shapes.C(-1,1,"VBF_HH_BSM","VBFcateg","BDT2","2017",100)'

#Calculate limits
combine -M AsymptoticLimits --freezeParameters allConstrainedNuisances workspace_GGF_HH_GGFcateg_2016.txt --run blind &> GGF_HH_GGFcateg_2016_asymptoticCLs.out
combine -M AsymptoticLimits --freezeParameters allConstrainedNuisances workspace_GGF_HH_GGFcateg_2017.txt --run blind &> GGF_HH_GGFcateg_2017_asymptoticCLs.out
combine -M AsymptoticLimits --freezeParameters allConstrainedNuisances workspace_VBF_HH_VBFcateg_2016.txt --run blind &> VBF_HH_VBFcateg_2016_asymptoticCLs.out
combine -M AsymptoticLimits --freezeParameters allConstrainedNuisances workspace_VBF_HH_VBFcateg_2017.txt --run blind &> VBF_HH_VBFcateg_2017_asymptoticCLs.out
combine -M AsymptoticLimits --freezeParameters allConstrainedNuisances workspace_VBF_HH_BSM_VBFcateg_2016.txt --run blind &> VBF_HH_BSM_VBFcateg_2016_asymptoticCLs.out
combine -M AsymptoticLimits --freezeParameters allConstrainedNuisances workspace_VBF_HH_BSM_VBFcateg_2017.txt --run blind &> VBF_HH_BSM_VBFcateg_2017_asymptoticCLs.out

mkdir CLs
mv *.out CLs