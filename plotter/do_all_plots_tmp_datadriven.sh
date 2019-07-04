#USAGE: Input parameters: $1=year $2=selection $3region

if [ "$1" != "2018" ]; then
      #Here variables from specific selections
      if [ "$2" == "VBFcateg_SR_110" ]; then
         python plotter.py --dir ../MyHistos/Histos$1 --var nJet         --sel  Btag4_$2_$3 --no-sr-namecompl --title ";Number of jets;Events"            --log --leg-coords 0.38 0.60 0.85 0.93 --ymin 1. --ymax 1e8 --quit --ratio --DataCond $1 --sigscale 100 --tab
         python plotter.py --dir ../MyHistos/Histos$1 --var HH_m         --sel  Btag4_$2_$3 --no-sr-namecompl --title ";HH invariant mass [GeV];Events"   --log --blind-range 0 5000 --leg-coords 0.38 0.60 0.85 0.93 --ymin 1. --ymax 1e8 --quit --ratio --DataCond $1 --sigscale 100      
         python plotter.py --dir ../MyHistos/Histos$1 --var BDT2         --sel  Btag4_$2_$3 --no-sr-namecompl --title "; BDT2 ;Events"                    --log --blind-range 0 1 --leg-coords 0.38 0.60 0.85 0.93 --ymin 1. --ymax 1e8 --quit --ratio --DataCond $1 --sigscale 100 
      elif [ "$2" == "GGFcateg_SR_110" ]; then
         python plotter.py --dir ../MyHistos/Histos$1 --var nJet         --sel   Btag4_$2_$3 --no-sr-namecompl --title ";Number of jets;Events"            --log --leg-coords 0.38 0.60 0.85 0.93 --ymin 1. --ymax 1e8 --quit --ratio --DataCond $1 --sigscale 100 --tab
         python plotter.py --dir ../MyHistos/Histos$1 --var HH_m         --sel  Btag4_$2_$3 --no-sr-namecompl --title ";HH invariant mass [GeV];Events"   --log --blind-range 0 5000 --leg-coords 0.38 0.60 0.85 0.93 --ymin 1. --ymax 1e8 --quit --ratio --DataCond $1 --sigscale 100      
         python plotter.py --dir ../MyHistos/Histos$1 --var BDT3         --sel  Btag4_$2_$3 --no-sr-namecompl --title "; BDT3 ;Events"                    --log --blind-range 0 1  --leg-coords 0.38 0.60 0.85 0.93 --ymin 1. --ymax 1e8 --quit --ratio --DataCond $1 --sigscale 100
      else
         python plotter.py --dir ../MyHistos/Histos$1 --var HH_m         --sel   Btag4_$2_$3 --no-sr-namecompl --title ";HH invariant mass [GeV];Events"   --log --leg-coords 0.38 0.60 0.85 0.93 --ymin 1. --ymax 1e8 --quit --ratio --DataCond $1 --sigscale 100      
#         python plotter.py --dir ../MyHistos/Histos$1 --var BDT3         --sel   Btag4_$2_$3 --no-sr-namecompl --title "; BDT3 ;Events"                          --leg-coords 0.38 0.60 0.85 0.93 --ymin 1. --quit --ratio --DataCond $1 --sigscale 100 
         python plotter.py --dir ../MyHistos/Histos$1 --var nJet         --sel   Btag4_$2_$3 --no-sr-namecompl --title ";Number of jets;Events"            --log --leg-coords 0.38 0.60 0.85 0.93 --ymin 1. --ymax 1e8 --quit --ratio --DataCond $1 --sigscale 100 --tab
         python plotter.py --dir ../MyHistos/Histos$1 --var H1_pt        --sel   Btag4_$2_$3 --no-sr-namecompl --title ";Leading Higgs P_{T} [GeV];Events" --log --leg-coords 0.38 0.60 0.85 0.93 --ymin 1. --ymax 1e8 --quit --ratio --DataCond $1 --sigscale 100
         python plotter.py --dir ../MyHistos/Histos$1 --var H2_pt        --sel   Btag4_$2_$3 --no-sr-namecompl --title ";Second Higgs P_{T} [GeV];Events"  --log --leg-coords 0.38 0.60 0.85 0.93 --ymin 1. --ymax 1e8 --quit --ratio --DataCond $1 --sigscale 100
#         python plotter.py --dir ../MyHistos/Histos$1 --var HH_b1_pt     --sel   Btag4_$2_$3 --no-sr-namecompl --title ";b1 P_{T} [GeV];Events"            --log --leg-coords 0.38 0.60 0.85 0.93 --ymin 1. --ymax 1e8 --quit --ratio --DataCond $1 --sigscale 100
#         python plotter.py --dir ../MyHistos/Histos$1 --var HH_b2_pt     --sel   Btag4_$2_$3 --no-sr-namecompl --title ";b2 P_{T} [GeV];Events"            --log --leg-coords 0.38 0.60 0.85 0.93 --ymin 1. --ymax 1e8 --quit --ratio --DataCond $1 --sigscale 100
#         python plotter.py --dir ../MyHistos/Histos$1 --var HH_b3_pt     --sel   Btag4_$2_$3 --no-sr-namecompl --title ";b3 P_{T} [GeV];Events"            --log --leg-coords 0.38 0.60 0.85 0.93 --ymin 1. --ymax 1e8 --quit --ratio --DataCond $1 --sigscale 100
#         python plotter.py --dir ../MyHistos/Histos$1 --var HH_b4_pt     --sel   Btag4_$2_$3 --no-sr-namecompl --title ";b4 P_{T} [GeV];Events"            --log --leg-coords 0.38 0.60 0.85 0.93 --ymin 1. --ymax 1e8 --quit --ratio --DataCond $1 --sigscale 100
#         python plotter.py --dir ../MyHistos/Histos$1 --var HH_m         --sel   Btag4_$2_$3 --no-sr-namecompl --title ";HH invariant mass [GeV];Events"   --log --leg-coords 0.38 0.60 0.85 0.93 --ymin 1. --ymax 1e8 --quit --ratio --DataCond $1 --sigscale 100      
#         python plotter.py --dir ../MyHistos/Histos$1 --var BDT2         --sel   Btag4_$2_$3 --no-sr-namecompl --title "; BDT2 ;Events"                          --leg-coords 0.38 0.60 0.85 0.93 --ymin 1. --quit --ratio --DataCond $1 --sigscale 100 
#         python plotter.py --dir ../MyHistos/Histos$1 --var JJ_j1_pt     --sel   Btag4_$2_$3 --no-sr-namecompl --title ";j1 P_{T} [GeV];Events"                 --log --leg-coords 0.38 0.60 0.85 0.93 --ymin 1. --ymax 1e8 --quit --ratio --DataCond $1 --sigscale 100
#         python plotter.py --dir ../MyHistos/Histos$1 --var JJ_j2_pt     --sel   Btag4_$2_$3 --no-sr-namecompl --title ";j2 P_{T} [GeV];Events"                 --log --leg-coords 0.38 0.60 0.85 0.93 --ymin 1. --ymax 1e8 --quit --ratio --DataCond $1 --sigscale 100
#         python plotter.py --dir ../MyHistos/Histos$1 --var JJ_j1_eta    --sel   Btag4_$2_$3 --no-sr-namecompl --title "; j1 #eta;Events"                       --log --leg-coords 0.38 0.60 0.85 0.93 --ymin 1. --ymax 1e8 --quit --ratio --DataCond $1 --sigscale 100
#         python plotter.py --dir ../MyHistos/Histos$1 --var JJ_j2_eta    --sel   Btag4_$2_$3 --no-sr-namecompl --title "; j2 #eta;Events"                       --log --leg-coords 0.38 0.60 0.85 0.93 --ymin 1. --ymax 1e8 --quit --ratio --DataCond $1 --sigscale 100
      fi
fi

if [ "$1" == "2018" ]; then
      #Here variables from specific selections
      if [ "$2" == "VBFcateg_SR_110" ]; then
         python plotter.py --dir ../MyHistos/Histos$1 --var HH_m         --sel  Btag4_$2_$3 --no-sr-namecompl --title ";HH invariant mass [GeV];Events"   --log --blind-range 0 5000 --leg-coords 0.38 0.60 0.85 0.93 --ymin 1. --ymax 1e8 --quit --ratio --DataCond $1 --no-sig      
         python plotter.py --dir ../MyHistos/Histos$1 --var BDT2         --sel  Btag4_$2_$3 --no-sr-namecompl --title "; BDT2 ;Events"                    --log --blind-range 0 1 --leg-coords 0.38 0.60 0.85 0.93 --ymin 1. --ymax 1e8 --quit --ratio --DataCond $1 --no-sig
      elif [ "$2" == "GGFcateg_SR_110" ]; then
         python plotter.py --dir ../MyHistos/Histos$1 --var HH_m         --sel  Btag4_$2_$3 --no-sr-namecompl --title ";HH invariant mass [GeV];Events"   --log --blind-range 0 5000 --leg-coords 0.38 0.60 0.85 0.93 --ymin 1. --ymax 1e8 --quit --ratio --DataCond $1 --no-sig      
         python plotter.py --dir ../MyHistos/Histos$1 --var BDT3         --sel  Btag4_$2_$3 --no-sr-namecompl --title "; BDT3 ;Events"                    --log --blind-range 0 1  --leg-coords 0.38 0.60 0.85 0.93 --ymin 1. --ymax 1e8 --quit --ratio --DataCond $1 --no-sig
      else
         python plotter.py --dir ../MyHistos/Histos$1 --var HH_m         --sel   Btag4_$2_$3 --no-sr-namecompl --title ";HH invariant mass [GeV];Events"   --log --leg-coords 0.38 0.60 0.85 0.93 --ymin 1. --ymax 1e8 --quit --ratio --DataCond $1 --no-sig      
#         python plotter.py --dir ../MyHistos/Histos$1 --var BDT3         --sel   Btag4_$2_$3 --no-sr-namecompl --title "; BDT3 ;Events"                          --leg-coords 0.38 0.60 0.85 0.93 --ymin 1. --quit --ratio --DataCond $1 --no-sig 
         python plotter.py --dir ../MyHistos/Histos$1 --var nJet         --sel   Btag4_$2_$3 --no-sr-namecompl --title ";Number of jets;Events"            --log --leg-coords 0.38 0.60 0.85 0.93 --ymin 1. --ymax 1e8 --quit --ratio --DataCond $1 --no-sig --tab
         python plotter.py --dir ../MyHistos/Histos$1 --var H1_pt        --sel   Btag4_$2_$3 --no-sr-namecompl --title ";Leading Higgs P_{T} [GeV];Events" --log --leg-coords 0.38 0.60 0.85 0.93 --ymin 1. --ymax 1e8 --quit --ratio --DataCond $1 --no-sig
         python plotter.py --dir ../MyHistos/Histos$1 --var H2_pt        --sel   Btag4_$2_$3 --no-sr-namecompl --title ";Second Higgs P_{T} [GeV];Events"  --log --leg-coords 0.38 0.60 0.85 0.93 --ymin 1. --ymax 1e8 --quit --ratio --DataCond $1 --no-sig
#         python plotter.py --dir ../MyHistos/Histos$1 --var HH_b1_pt     --sel   Btag4_$2_$3 --no-sr-namecompl --title ";b1 P_{T} [GeV];Events"            --log --leg-coords 0.38 0.60 0.85 0.93 --ymin 1. --ymax 1e8 --quit --ratio --DataCond $1 --no-sig
#         python plotter.py --dir ../MyHistos/Histos$1 --var HH_b2_pt     --sel   Btag4_$2_$3 --no-sr-namecompl --title ";b2 P_{T} [GeV];Events"            --log --leg-coords 0.38 0.60 0.85 0.93 --ymin 1. --ymax 1e8 --quit --ratio --DataCond $1 --no-sig
#         python plotter.py --dir ../MyHistos/Histos$1 --var HH_b3_pt     --sel   Btag4_$2_$3 --no-sr-namecompl --title ";b3 P_{T} [GeV];Events"            --log --leg-coords 0.38 0.60 0.85 0.93 --ymin 1. --ymax 1e8 --quit --ratio --DataCond $1 --no-sig
#         python plotter.py --dir ../MyHistos/Histos$1 --var HH_b4_pt     --sel   Btag4_$2_$3 --no-sr-namecompl --title ";b4 P_{T} [GeV];Events"            --log --leg-coords 0.38 0.60 0.85 0.93 --ymin 1. --ymax 1e8 --quit --ratio --DataCond $1 --no-sig
#         python plotter.py --dir ../MyHistos/Histos$1 --var HH_m         --sel   Btag4_$2_$3 --no-sr-namecompl --title ";HH invariant mass [GeV];Events"   --log --leg-coords 0.38 0.60 0.85 0.93 --ymin 1. --ymax 1e8 --quit --ratio --DataCond $1 --no-sig      
#         python plotter.py --dir ../MyHistos/Histos$1 --var BDT2         --sel   Btag4_$2_$3 --no-sr-namecompl --title "; BDT2 ;Events"                          --leg-coords 0.38 0.60 0.85 0.93 --ymin 1. --quit --ratio --DataCond $1 --no-sig 
#         python plotter.py --dir ../MyHistos/Histos$1 --var JJ_j1_pt     --sel   Btag4_$2_$3 --no-sr-namecompl --title ";j1 P_{T} [GeV];Events"                 --log --leg-coords 0.38 0.60 0.85 0.93 --ymin 1. --ymax 1e8 --quit --ratio --DataCond $1 --no-sig
#         python plotter.py --dir ../MyHistos/Histos$1 --var JJ_j2_pt     --sel   Btag4_$2_$3 --no-sr-namecompl --title ";j2 P_{T} [GeV];Events"                 --log --leg-coords 0.38 0.60 0.85 0.93 --ymin 1. --ymax 1e8 --quit --ratio --DataCond $1 ---no-sig
#         python plotter.py --dir ../MyHistos/Histos$1 --var JJ_j1_eta    --sel   Btag4_$2_$3 --no-sr-namecompl --title "; j1 #eta;Events"                       --log --leg-coords 0.38 0.60 0.85 0.93 --ymin 1. --ymax 1e8 --quit --ratio --DataCond $1 --no-sig
#         python plotter.py --dir ../MyHistos/Histos$1 --var JJ_j2_eta    --sel   Btag4_$2_$3 --no-sr-namecompl --title "; j2 #eta;Events"                       --log --leg-coords 0.38 0.60 0.85 0.93 --ymin 1. --ymax 1e8 --quit --ratio --DataCond $1 --no-sig
      fi
fi

