#readable variables
datasets=$1
option=$2

if [ "$option" == "1" ]; then
     #This is for control plots
     echo "Running on datadriven control plots"
     folders=(GGFcateg1_CR_210_Histogram GGFcateg2_CR_210_Histogram VBFcateg1_CR_210_Histogram VBFcateg2_CR_210_Histogram GGFcateg1_CR_110_Histogram GGFcateg2_CR_110_Histogram VBFcateg1_CR_110_Histogram VBFcateg2_CR_110_Histogram  GGFcateg1_SR_210_Histogram GGFcateg2_SR_210_Histogram VBFcateg1_SR_210_Histogram VBFcateg2_SR_210_Histogram GGFcateg1_SR_110_Histogram GGFcateg2_SR_110_Histogram VBFcateg1_SR_110_Histogram VBFcateg2_SR_110_Histogram)
     histograms=(H1_m_rot H2_m_rot H1_pt H2_pt HH_m h1h2_deltaEta h1h2_deltaPhi JJ_m j1j2_deltaEta abs_costh_H1_ggfcm H1_bb_deltaR H2_bb_deltaR HH_btag_b3_bscore HH_btag_b2_bscore HH_btag_b3_bres HH_m_1 HH_m_2 GGFMVA1 GGFMVA2 EventCount)
     models=(MODEL MODEL_1p0_uinj_ggHH_kl_1_kt_1 MODEL_1p0_uinj_qqHH_CV_1_C2V_1_kl_1 ) 
#     folders=(GGFcateg1_SR_110_Histogram GGFcateg2_SR_110_Histogram)
#     histograms=(HH_m GGFMVA1 GGFMVA2 BDT3cat1 BDT3cat2 EventCount) 
     mkdir MyBkgModelHistos
     for dataset in ${datasets[@]}
     do
        #Start
        cp -r BkgModelHistos${dataset} Histos${dataset}
        cd Histos${dataset}
        #Change the folder tag from Btag3 to Btag4
        for folder in ${folders[@]}
        do
               for model in ${models[@]}
               do
                    rootrm -r outPlotter.root:${model}/Btag4_${folder}
                    rootmv outPlotter.root:${model}/Btag3_${folder}/ outPlotter.root:${model}/Btag4_${folder}
                    for histogram in ${histograms[@]}
                    do
                        rootmv outPlotter.root:${model}/Btag4_${folder}/${model}_Btag3_${folder}_${histogram}  outPlotter.root:${model}/Btag4_${folder}/${model}_Btag4_${folder}_${histogram} 
                    done 
                    echo "Done with the folder $folder in $dataset for $model"
               done
        done  
        cd ..
        mv BkgModelHistos${dataset} MyBkgModelHistos
        mv Histos${dataset} MyBkgModelHistos
     done
elif [ "$option" == "2" ]; then
   #This is for limit setting
   echo "[INFO] Running on datadriven for combine"   
   folders=(GGFcateg1_SR_110_Histogram GGFcateg2_SR_110_Histogram VBFcateg1_SR_110_Histogram VBFcateg2_SR_110_Histogram)
   histograms=(HH_m_1 HH_m_2 GGFMVA1 GGFMVA2) 
   models=(MODEL)   
   for dataset in ${datasets[@]}
   do
      #Start
      cp -r ModelHistos${dataset} Model3b24bHistos${dataset}
      cd Model3b24bHistos${dataset}
      #Change the folder tag from Btag3 to Btag4
      for folder in ${folders[@]}
      do
             for model in ${models[@]}
             do
                  rootrm -r outPlotter.root:${model}/Btag4_${folder}
                  rootmv outPlotter.root:${model}/Btag3_${folder}/ outPlotter.root:${model}/Btag4_${folder}
                  for histogram in ${histograms[@]}
                  do
                      rootmv outPlotter.root:${model}/Btag4_${folder}/${model}_Btag3_${folder}_${histogram}  outPlotter.root:${model}/Btag4_${folder}/${model}_Btag4_${folder}_${histogram} 
                  done 
                  echo "Done with the folder $folder in $dataset"
             done
      done  
      cd ..
      mkdir MyHistos
      mv Model3b24bHistos${dataset} MyHistos
      mv ModelHistos${dataset} MyHistos
      mv DataMCHistos${dataset} MyHistos
      #Merge the root files
      echo "[INFO] Preparing file for combine with Data/MC/datadriven histograms"
      cd MyHistos
      mkdir Histos${dataset}
      hadd -f Histos${dataset}/outPlotter.root DataMCHistos${dataset}/outPlotter.root Model3b24bHistos${dataset}/outPlotter.root
      cd ..  
   done
else
     echo "[ERROR] Option is not correct!!"
fi
