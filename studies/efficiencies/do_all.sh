#### make all the eff plots for the AN

## ggF signal
python eff_vs_klambda.py
for p in mHHcateg GGFvsVBF; do python frac_categ_GGF_vs_klambda.py $p ; done

## VBF signal
for c in kl CV C2V; do python eff_VBF_vs_coupling.py $c ; done
for c in kl CV C2V; do for p in VBFcateg GGFvsVBF GGFvsVBFonVBFevts; do python frac_categ_VBF_vs_coupling.py $c $p; done ; done