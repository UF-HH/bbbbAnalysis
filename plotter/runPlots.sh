datasets=(2016 2017 2018) 
selections=(GGFcateg_CR_110 GGFcateg_CR_210 GGFcateg_SR_110 GGFcateg_SR_210 VBFcateg_CR_110 VBFcateg_CR_210 VBFcateg_SR_110 VBFcateg_SR_210)
regions=(Histogram) 
for dataset in ${datasets[@]}
do
     for selection in ${selections[@]}
     do
          for region in ${regions[@]}
          do
                mkdir plots/$dataset 
                mkdir plots/$dataset/$selection
                mkdir plots/$dataset/$selection/$region
                source do_all_plots_tmp_datadriven.sh $dataset $selection $region
                mv plots/*.png plots/$dataset/$selection/$region
          done
     done   
done
