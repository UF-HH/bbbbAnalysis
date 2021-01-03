#readable variables
datasets=$1
option=$2

if [ "$option" == "1" ]; then
     #This is for control plots
     echo "Running on datadriven control plots and bias studies"
     folders=(GGFcateg1_CR_210_Histogram GGFcateg2_CR_210_Histogram VBFcateg1_CR_210_Histogram VBFcateg2_CR_210_Histogram GGFcateg1_CR_110_Histogram GGFcateg2_CR_110_Histogram VBFcateg1_CR_110_Histogram VBFcateg2_CR_110_Histogram  GGFcateg1_SR_210_Histogram GGFcateg2_SR_210_Histogram VBFcateg1_SR_210_Histogram VBFcateg2_SR_210_Histogram GGFcateg1_SR_110_Histogram GGFcateg2_SR_110_Histogram VBFcateg1_SR_110_Histogram VBFcateg2_SR_110_Histogram)
     histograms=(H1_b1_ptRegressed H1_b2_ptRegressed H2_b1_ptRegressed H2_b2_ptRegressed H1_m H2_m HH_m H1_pt H2_pt h1h2_deltaEta H1_bb_deltaR H2_bb_deltaR abs_costh_H1_ggfcm HH_btag_b3_bres abs_costh_H1_b1_h1cm sum_4b_pt HH_pt sum_3b_bres min_4b_deltaR max_4b_deltaEta nBtagTightonMediumWP h1h2_deltaPhi JJ_m j1j2_deltaEta GGFKiller HH_m_1 HH_m_2 GGFMVA1 GGFMVA2 EventCount)
     models=(MODEL) # MODEL_1p0_uinj_ggHH_kl_1_kt_1 MODEL_1p0_uinj_qqHH_CV_1_C2V_1_kl_1) 
     mkdir MyDataModelHistosBDTR_New
     for dataset in ${datasets[@]}
     do
        #Start
        cp -r ControlBkgModelHistos${dataset} BkgModelHistos${dataset}
        cd BkgModelHistos${dataset}
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
        mv ControlBkgModelHistos${dataset} MyDataModelHistosBDTR_New
        mv BkgModelHistos${dataset} MyDataModelHistosBDTR_New
     done
elif [ "$option" == "2" ]; then
   #This is for limit setting or bias studies
   echo "[INFO] Running on datadriven for combine input"   
   folders=(GGFcateg1_SR_110_Histogram GGFcateg2_SR_110_Histogram VBFcateg1_SR_110_Histogram VBFcateg2_SR_110_Histogram)
   histograms=(HH_m_1 HH_m_2 GGFMVA1 GGFMVA2) #GGFMVA1btag GGFMVA2btag) 
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
      #mkdir MyHistos
      #mv Model3b24bHistos${dataset} MyHistos
      #mv ModelHistos${dataset} MyHistos
      #mv DataSignalHistos${dataset} MyHistos
      ##Merge the root files
      #echo "[INFO] Preparing file for combine with Data/MC/datadriven histograms"
      #cd MyHistos
      #mkdir Histos${dataset}
      #hadd -f Histos${dataset}/outPlotter.root DataSignalHistos${dataset}/outPlotter.root Model3b24bHistos${dataset}/outPlotter.root
      #cd ..  
   done
else
     echo "[ERROR] Option is not correct!!"
fi
