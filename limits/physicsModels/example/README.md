# About the physics model
The physics model ``limits/physicsModels/VBFHHModel.py`` implements the scaling of the 6 VBF components.
When using it the important things to check are:

* the sample list that defines the 6 samples used
* the xs values and the coupling values for each sample
* the label of each sample (must be the start of the name of the corresponding process in the datacard)

If you change the combination of samples, just create a new sample_list and a model with a different name from it (you can define as many models as you want).
The default one for the 6 original samples available for the Run 2 is ``VBFHHdefault``.

The POIs are called ``CV``, ``C2V``, ``kl``, and ``r``.
``r`` scales the total predicted signal yield. If fixed to 1, then the signal normalisation will correspond to the theoretical one from ``CV``, ``C2V``, ``kl``.

By default ``r`` is floating in -10, 10, while ``CV``, ``C2V``, ``kl`` are all fixed.

**IMPORTANT NOTE** : the model assumes that the six input samples are originally normalised to the theoretical cross section that is indicated in the ``sample_list``.

# Examples

An example datacard is in ``limits/physicsModels/example``, commands to be launched in that subfolder.

## Making the datacard

```
cmsenv
PYTHONPATH=$PYTHONPATH:${CMSSW_BASE}/src/bbbbAnalysis/limits/physicsModels
text2workspace.py test_card_vbfHH_comp.txt -D DATA -P VBFHHModel:VBFHHdefault
```

Setting ``PYTHONPATH`` (just once) before calling ``text2workspace.py`` is needed to link properly the model repo.

## Running the upper limits

```
combine -M AsymptoticLimits --run blind test_card_vbfHH_comp.root -D DATA --redefineSignalPOIs r --setParameters kl=1,CV=1,C2V=3
```
Add `` --freezeParameters allConstrainedNuisances`` to make stat only limits


## Make likelihood scans

**1D scan**
```
combine -M MultiDimFit -t -1  test_card_vbfHH_comp.root -D DATA --redefineSignalPOIs C2V --setParameters r=1,CV=1,kl=1 --freezeParameters r,CV,kl --algo grid
```
**2D scan**
```
combine -M MultiDimFit -t -1  test_card_vbfHH_comp.root -D DATA --redefineSignalPOIs CV,C2V --setParameters r=1,kl=1 --freezeParameters r,kl --algo grid
```

**IMPORTANT NOTE** : the parameters that are not fit (and in particular ``r``) *must* be frozen to avoid them being floated in the fit.

To make stat-only limits add `` --freezeParameters rgx{.*},r,kl`` (i.e. use a regexp to get the result)


## Quickly checking the output of the likelihood fits
From the combine output file

**1D scan**
```
limit->Draw("2*deltaNLL:C2V","2*deltaNLL<10")
```

**2D scan**
```
limit->Draw("2*deltaNLL:CV:C2V","2*deltaNLL<10","prof colz")
```
