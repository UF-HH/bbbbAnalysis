# About the physics model
The physics model ``limits/physicsModels/HHModel.py`` implements the scaling of the 6 VBF components and of the 3 GGF components.
When using it the important things to check are:

* the sample list that defines the 6 samples used for VBF and the 3 for GGF
* the xs values and the coupling values for each sample, to be taken from the generator (note that the model is not sensitive to overall scalings, so you can e.g. not apply the final state BR)
* the label of each sample (must be the start of the name of the corresponding process in the datacard)

If you change the combination of samples, just create a new ``ggf_sample_list`` and/or ``vbf_sample_list`` and a model with a different name from it (you can define as many models as you want).
The default one for Run 2 is ``HHdefault``.

The 7 POIs are:
* 4 couplings : ``CV``, ``C2V``, ``kl``, ``kt``
* 3 signal strengths : ``r``, ``r_gghh``, ``r_qqhh``
and the model is built as:

```
r * { r_gghh * sum [f_ggf^i (kl, kt) s_ggf^i ] + r_qqhh * sum [f_vbf^i (CV, C2V, kl) s_vbf^i ] }
```

Default setup:
* all paramters in default range [-10, 10] with the exception of ``kl`` in [-30, 30]
* all parameters are set constants with the exception of ``r`` that is floating


``r`` scales the total predicted signal yield, while ``r_gghh`` and ``r_qqhh`` scale independently the ggf and vbf signals.
 If all are fixed to 1, then the signal normalisation will correspond to the theoretical one from the coupling set
 (``kl``, ``kt`` for GGF, ``CV``, ``C2V``, ``kl`` for VBF)


By default ``r`` is floating in -10, 10, while ``r_gghh``, ``r_qqhh``, ``CV``, ``C2V``, ``kl``, ``kt`` are all fixed.

**IMPORTANT NOTE** : the model assumes that the input samples are originally normalised to the theoretical cross section that is indicated in the ``sample_list``
(apart from from overall scalings, i.e. the relative normalisations of the chosen samples must match the relative cross sections).
In short : just normalise your samples to their generator xs times the final state BR, possibly scaled up by a k-factor, and all will be fine.


# Examples

An example datacard is in ``limits/physicsModels/example``, commands to be launched in that subfolder.


## Making the datacard

```
cmsenv
PYTHONPATH=$PYTHONPATH:${CMSSW_BASE}/src/bbbbAnalysis/limits/physicsModels
text2workspace.py test_card_vbfHH_comp_4HHModel.txt -D DATA -P HHModel:HHdefault
```

Setting ``PYTHONPATH`` (just once) before calling ``text2workspace.py`` is needed to link properly the model repo.


## Running the upper limits

```
combine -M AsymptoticLimits --run blind test_card_vbfHH_comp_4HHModel.root -D DATA --redefineSignalPOIs r --setParameters r_gghh=1,r_qqhh=1,kt=1,kl=1,CV=1,C2V=3
```
Add `` --freezeParameters allConstrainedNuisances`` to make stat only limits

*NOTE* : by default all parameters are set to 1, so for limits you can just ``--setParameters`` for those that have a different default value.
However it's still better to have full control of your settings.


## Make likelihood scans

**1D scan**
```
combine -M MultiDimFit -t -1  test_card_vbfHH_comp_4HHModel.root -D DATA --redefineSignalPOIs C2V --setParameters r=1,r_gghh=1,r_qqhh=1,CV=1,kl=1,kt=1 --freezeParameters r,r_qqhh,r_gghh,CV,kl,kt --algo grid
```
**2D scan**
```
combine -M MultiDimFit -t -1  test_card_vbfHH_comp_4HHModel.root -D DATA --redefineSignalPOIs CV,C2V --setParameters r=1,kl=1 --freezeParameters r,kl --algo grid
```

*NOTE* : as in the limit case, you can skip the ``--setParameters`` and the ``--freezeParameters`` that do not change the default values / constant status described above. Still it is better to have full control of the setup. However...

**IMPORTANT NOTE** : ...  ``r`` *must* be frozen to avoid it being floated in the fit (it's not constant by default)

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

------------------


<details><summary>About the VBFHHModel (outdated)</summary>
<p>


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

</p>
</details>
