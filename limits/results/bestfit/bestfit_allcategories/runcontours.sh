YEAR=$1 #2016/20172018/run2

#Expected contours
for CONTOUR in 68 95; 
  do 
     combine -M MultiDimFit -t -1 ../../../prepareModels/physicsmodel${YEAR}/datacard${YEAR}_comb_GGFcateg1GGFcateg2VBFcateg1VBFcateg2.root -D data_obs --redefineSignalPOIs r_gghh,r_qqhh --setParameters r=1,C2V=1,CV=1,kl=1,kt=1 --freezeParameters r,CV,C2V,kl,kt --algo contour2d --cl=0.${CONTOUR} --setParameterRanges r_gghh=-20.0,20:r_qqhh=-1000.0,1000.0 --points=20 -n EXPcontour${CONTOUR} ; 
  done

#Observed contours
for CONTOUR in 68 95;
  do
     combine -M MultiDimFit ../../../prepareModels/physicsmodel${YEAR}/datacard${YEAR}_comb_GGFcateg1GGFcateg2VBFcateg1VBFcateg2.root -D data_obs --redefineSignalPOIs r_gghh,r_qqhh --setParameters r=1,C2V=1,CV=1,kl=1,kt=1 --freezeParameters r,CV,C2V,kl,kt --algo contour2d --cl=0.${CONTOUR} --setParameterRanges r_gghh=-20.0,20:r_qqhh=-1000.0,1000.0 --points=20 -n OBScontour${CONTOUR} ;
  done

