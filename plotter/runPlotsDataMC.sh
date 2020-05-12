datasets=(2016) 
selections=(GGFcateg1_SR_110 GGFcateg2_SR_110 VBFcateg1_SR_110 VBFcateg2_SR_110) 
regions=(Histogram)

#rm -r plotsdatamc
mkdir plotsdatamc
 
for dataset in ${datasets[@]}
do
     for selection in ${selections[@]}
     do
          for region in ${regions[@]}
          do
                mkdir plotsdatamc/$dataset 
                mkdir plotsdatamc/$dataset/$selection
                mkdir plotsdatamc/$dataset/$selection/$region
                source do_all_plots_datamc.sh $dataset $selection $region
                mv plotsdatamc/*.pdf plotsdatamc/$dataset/$selection/$region
          done
     done   
done
