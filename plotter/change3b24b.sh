#Define folders and histograms
datasets=$1
folders=(GGFcateg1_CR_110_Histogram GGFcateg1_CR_210_Histogram GGFcateg1_SR_110_Histogram GGFcateg1_SR_210_Histogram GGFcateg2_CR_110_Histogram GGFcateg2_CR_210_Histogram GGFcateg2_SR_110_Histogram GGFcateg2_SR_210_Histogram GGFcateg_CR_110_Histogram GGFcateg_CR_210_Histogram GGFcateg_SR_110_Histogram GGFcateg_SR_210_Histogram VBFcateg_CR_110_Histogram VBFcateg_CR_210_Histogram VBFcateg_SR_210_Histogram VBFcateg_SR_110_Histogram VBFcateg1_SR_210_Histogram VBFcateg2_SR_210_Histogram VBFcateg1_SR_110_Histogram VBFcateg2_SR_110_Histogram VBFcateg1_CR_210_Histogram VBFcateg2_CR_210_Histogram VBFcateg1_CR_110_Histogram VBFcateg2_CR_110_Histogram)
#histograms=(BDT3 HH_m EventCount) 
histograms=(HH_m_1 HH_m_2 JJ_j1_eta JJ_j2_eta BDT3 nJet_ec H1_pt H2_pt h1h2_deltaEta h1h2_deltaPhi H1_bb_deltaR H2_bb_deltaR HH_m JJ_m j1j2_deltaEta H1_m H2_m costh_HH_b1_ggfcm HH_btag_b3_bscore HH_btag_b3_bres EventCount)
#folders=(GGFcateg_SR_110_Histogram VBFcateg1_SR_110_Histogram VBFcateg2_SR_110_Histogram)
#histograms=(HH_m HH_m_1 HH_m_2 BDT3) 
models=(MODEL) 

for dataset in ${datasets[@]}
do
   #Start
   cp -r Histos${dataset}skim++ Histos${dataset}
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
               echo "Done with the folder $folder in $dataset"
          done
   done  
   cd ..
done
