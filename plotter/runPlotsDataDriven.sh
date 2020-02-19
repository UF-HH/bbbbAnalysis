datasets=$1
selections=(GGFcateg1_CR_110 GGFcateg1_CR_210 GGFcateg1_SR_110 GGFcateg1_SR_210  GGFcateg2_CR_110 GGFcateg2_CR_210 GGFcateg2_SR_110 GGFcateg2_SR_210 GGFcateg_CR_110 GGFcateg_CR_210 GGFcateg_SR_110 GGFcateg_SR_210 VBFcateg_CR_110 VBFcateg1_SR_110 VBFcateg2_SR_110 VBFcateg_CR_210 VBFcateg_SR_210 VBFcateg1_SR_210 VBFcateg2_SR_210 VBFcateg1_CR_110 VBFcateg2_CR_110)
#selections=(VBFcateg_SR_110 VBFcateg1_SR_110 VBFcateg2_SR_110 )
regions=(Histogram)

mkdir plotsDatadriven
 
for dataset in ${datasets[@]}
do
     for selection in ${selections[@]}
     do
          for region in ${regions[@]}
          do
                mkdir plotsDatadriven/$dataset 
                mkdir plotsDatadriven/$dataset/$selection
                mkdir plotsDatadriven/$dataset/$selection/$region
                source do_all_plots_datadriven.sh $dataset $selection $region
                mv plotsDatadriven/*.png plotsDatadriven/$dataset/$selection/$region
          done
     done   
done
