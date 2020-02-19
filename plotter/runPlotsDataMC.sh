datasets=(2016 2017 2018) 
selections=(GGFcateg VBFcateg)
regions=(Histogram)

rm -r plotsdatamc
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
                mv plotsdatamc/*.png plotsdatamc/$dataset/$selection/$region
          done
     done   
done
