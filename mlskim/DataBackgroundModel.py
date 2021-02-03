import numpy 
import os
import root_numpy
import pandas
import glob
import argparse
import sys
import ast
from  ConfigParser import *
from hep_ml import reweight
from root_numpy import root2array
from numpy.lib.recfunctions import stack_arrays
import pickle
import multiprocessing as mp
from functools import partial
import time
from sklearn.utils import shuffle
import math
import ROOT

#My modules
import modules.datatools as data
import modules.plotter as plotter
import modules.bdtreweighter as bdtreweighter
import modules.selections as selector

def AddGGFMVA(skim,ggfmvafile):
    # open picked file and use it back again
    #print '  *Adding the ggfmva in %s'%ggfmvafile
    infile     = open(ggfmvafile,'rb')
    ggfmva     = pickle.load(infile)
    infile.close()
    variables  = ggfmva.get('vars')
    X          = skim[list(set(variables))] 
    y_pred     = ggfmva.get('model').predict_proba(X)[:,1]
    return y_pred

def SaveReweighterModel(vars_training,modelargs,bdtreweighter,transferfactor,label):
    ## save the model as pickle
    outname   = 'bkgtraining/mymodels/%s_bkgmodel.pkl'%label
    print '[INFO] Pickling bdtreweighter training as', outname
    outfile   = open(outname, 'wb')
    to_save   = {
        'name'     : label,
        'vars'     : vars_training,
        'modelargs': modelargs,
        'model'    : bdtreweighter,
        'tfactor'  : transferfactor,
    }
    pickle.dump (to_save, outfile)
    outfile.close()

def SaveTransferFactorModel(vars_training,transferfactor,label):
    ## save the model as pickle
    outname   = 'bkgtraining/mymodels/%s_bkgmodel.pkl'%label
    print '[INFO] Pickling transferfactor training as', outname
    outfile   = open(outname, 'wb')
    to_save   = {
        'name'    : label,
        'vars'    : vars_training,
        'tfactor' : transferfactor,
    }
    pickle.dump (to_save, outfile)
    outfile.close()

def SelectRegions(sample,btagregion,category,validation):
    datacr,samplerest = selector.eventselection(sample,btagregion,category,'CR',validation)
    del sample
    datasr,datarest   = selector.eventselection(samplerest,btagregion,category,'SR',validation) 
    del samplerest
    return datacr,datasr,datarest


def BuildReweightingModel(data_3b, data_4b,category,tag,bkgclassifierparams,bkgparams):
    print "[INFO] Processing predicted model for %s"%(tag)
    modelargs=bkgparams
    ############################################################################
    ##Let's slice data one more time to have the inputs for the bdt reweighting
    ############################################################################
    if category =='GGF' or category =='GGF1' or category =='GGF2':
         variablescr = ['H1_b1_ptRegressed','H1_b2_ptRegressed','H2_b1_ptRegressed','H2_b2_ptRegressed','H1_pt','H2_pt','H1_m','H2_m','HH_m','h1h2_deltaEta','H1_bb_deltaR','H2_bb_deltaR','abs_costh_H1_ggfcm','abs_costh_H1_b1_h1cm','sum_4b_pt','HH_pt','nBtagTightonMediumWP','sum_3b_bres','min_4b_deltaR','max_4b_deltaEta']  
    elif category == 'VBF' or category =='VBF1' or category =='VBF2':
         variablescr = ['H1_b1_ptRegressed','H1_b2_ptRegressed','H2_b1_ptRegressed','H2_b2_ptRegressed','H1_m','H2_m','HH_m','H1_pt','H2_pt','h1h2_deltaEta','h1h2_deltaPhi','JJ_m','j1j2_deltaEta','GGFKiller']
    else:
         variablescr = ['HH_m']         

    #######################################
    ##Prepare data to create the model
    #######################################
    originalcr,targetcr,originalcr_weights,targetcr_weights,transferfactor = data.preparedataformodel(data_3b,data_4b,variablescr)
    #plotter.Draw1DHistosComparison(originalcr, targetcr, variablescr, originalcr_weights,True,"%s_original"%tag)

    #######################################
    ##Grid search or main fit
    #######################################
    print "--------------------------Best-Hyperparameters-------------------------------------"
    print "-----------------------------------------------------------------------------------"
    print "Parameters: ",modelargs
    print "-----------------------------------------------------------------------------------"
    #######################################
    ##Folding Gradient Boosted Reweighter (3-fold BDT reweighter)
    #######################################
    foldingcr_weights,reweightermodel = data.fitreweightermodel(originalcr,targetcr,originalcr_weights,targetcr_weights,transferfactor,modelargs)  
    #plotter.Draw1DHistosComparison(originalcr, targetcr, variablescr, foldingcr_weights,True,"%s_model"%tag)
    
    ########################################
    ## KS Test (as the developers of the method do), currently used for optimization/check of the parameters
    ########################################
    ksresult_original = bdtreweighter.ks_test(originalcr, targetcr, variablescr, originalcr_weights)
    ksresult_model    = bdtreweighter.ks_test(originalcr, targetcr, variablescr, foldingcr_weights) 
    passedksresult    = bdtreweighter.ks_comparison(variablescr,ksresult_original,ksresult_model)
    ###########################################################################################
    ## GB ROC AUC Test Study (Very slow test, needs to train a classifier in cross-validation)
    ############################################################################################
    roc_auc_nowts  = bdtreweighter.discrimination_test(originalcr,targetcr,originalcr_weights,bkgclassifierparams,"%s"%tag,"original",False)
    roc_auc_wts    = bdtreweighter.discrimination_test(originalcr,targetcr,foldingcr_weights,bkgclassifierparams,"%s"%tag,"model",False)
    passedresult   = bdtreweighter.discrimination_comparison(roc_auc_nowts,roc_auc_wts)
    ##############################################################
    ## Save training information only if found a set candidate
    ##############################################################
    SaveReweighterModel(variablescr,modelargs,reweightermodel,transferfactor,tag)
    return foldingcr_weights,reweightermodel,transferfactor

def BuildTransferFactorModel(data_3b, data_4b,category,tag):
    print "[INFO] Processing predicted model for %s"%(tag)
    #######################################
    ##Prepare data to create the model
    #######################################
    variablescr = ['HH_m']
    originalcr,targetcr,originalcr_weights,targetcr_weights,transferfactor = data.preparedataformodel(data_3b,data_4b,variablescr)
    data.fittransferfactormodel(originalcr,targetcr,originalcr_weights,targetcr_weights,transferfactor)
    #plotter.Draw1DHistosComparison(originalcr, targetcr, variablescr, originalcr_weights,True,"%s_model"%tag)
    ########################################
    ## Save training information
    ########################################
    SaveTransferFactorModel(variablescr,transferfactor,tag)

    return originalcr_weights,transferfactor

def ApplyReweightingModel(reweightermodel,transferfactor,data_3b,data_4b,tag,bkgclassifierparams,category,valflag,srflag):
    ############################################################################
    ##Let's slice data one more time to have the inputs for the bdt reweighting
    ############################################################################
    if category =='GGF' or category =='GGF1' or category =='GGF2':
         variables = ['H1_b1_ptRegressed','H1_b2_ptRegressed','H2_b1_ptRegressed','H2_b2_ptRegressed','H1_pt','H2_pt','H1_m','H2_m','HH_m','h1h2_deltaEta','H1_bb_deltaR','H2_bb_deltaR','abs_costh_H1_ggfcm','abs_costh_H1_b1_h1cm','sum_4b_pt','HH_pt','nBtagTightonMediumWP','sum_3b_bres','min_4b_deltaR','max_4b_deltaEta']
    elif category == 'VBF' or category =='VBF1' or category =='VBF2':
         variables = ['H1_b1_ptRegressed','H1_b2_ptRegressed','H2_b1_ptRegressed','H2_b2_ptRegressed','H1_m','H2_m','HH_m','H1_pt','H2_pt','h1h2_deltaEta','h1h2_deltaPhi','JJ_m','j1j2_deltaEta','GGFKiller']
    else:
         variables = ['HH_m']          
    original,original_weights = data.preparedataforprediction(data_3b,transferfactor,variables)
    target,target_weights     = data.preparedataforprediction(data_4b,1,variables)

    ########################################3############
    ##Folding Gradient Boosted Reweighter (and DQM plots)
    #####################################################
    folding_weights= data.getmodelweights(original,original_weights,target,target_weights,reweightermodel,transferfactor,valflag,srflag)
    ########################################
    
    return folding_weights


def ApplyTransferFactorModel(transferfactor,data_3b,data_4b,tag,category,valflag,srflag):
    ############################################################################
    ##Let's slice data one more time to have the inputs for the bdt reweighting
    ############################################################################
    variables = ['HH_m']          
    original,original_weights = data.preparedataforprediction(data_3b,transferfactor,variables)
    target,target_weights     = data.preparedataforprediction(data_4b,1,variables)
    ########################################
    ## Check the single bin, done just for the validation signal region as we are currently blinded
    ########################################
    data.getmodeltransferfactorweights(original,original_weights,target,target_weights,transferfactor,valflag,srflag)   
    return original_weights


def BuildNormModel(dataset3b, dataset4b, datasetsr, x, valflag):
    #Get information 
    variable = ['%s'%x]
    data_3b  = dataset3b[variable].to_numpy() 
    data_4b  = dataset4b[variable].to_numpy()
    data_sr  = datasetsr[variable].to_numpy()

    #Book histograms depending on the variables and validation region
    center=0
    if valflag==True: center=math.sqrt(179*179 + 172*172) 
    else: center=math.sqrt(125*125 + 120*120)   
    h_3b = ROOT.TH1F("h_3b","h_3b",8,center-50,center+50)
    h_4b = ROOT.TH1F("h_4b","h_4b",8,center-50,center+50)
    h_sr = ROOT.TH1F("h_sr","h_sr",8,center-50,center+50)

    #Loop over 3b events
    for k in range(0,len(data_3b)): h_3b.Fill(data_3b[k])
    #Loop over 4b events
    for k in range(0,len(data_4b)): h_4b.Fill(data_4b[k])
    #Loop over sr events
    for k in range(0,len(data_sr)): h_sr.Fill(data_sr[k])

    #Compute the transfer factors in each bin
    tfs        = [float(h_4b.GetBinContent(k)) / float(h_3b.GetBinContent(k)) for k in range(1,9)]
    #Compute the transferfactor uncertainties in each bin
    tfs_uncs   = [data.GetTotalError(float(h_4b.GetBinContent(k)),float(h_3b.GetBinContent(k)))  for k in range(1,9)]
    #Compute the normalizations in each bin
    norms      = [float(h_sr.GetBinContent(k)) * tfs[k-1] for k in range(1,9) ]
    #Compute the normalization uncertainties in each bin
    norms_uncs = [ norms[k-1] * (tfs_uncs[k-1]/tfs[k-1]) for k in range(1,9)  ]

    #Compute the output norm and the associated uncertainty
    totalnorm = sum(norms)
    totalnormuncertainty = math.sqrt(sum(i*i for i in norms_uncs))

    return totalnorm,totalnormuncertainty

def AddModelWeights(dataset,bkgparams,bkgclassifierparams,weightname,categ,valflag,tag,ggfmva1file1,ggfmva2file1,ggfmva1file2,ggfmva2file2,ggfmva1file1btag,ggfmva2file1btag,ggfmva1file2btag,ggfmva2file2btag):
    print "\n"*2
    #Slice the data sample to take only events with three/four b-tags among the two categories
    print "[INFO] Preparing data and building BDT-reweighting model for %s using the 3b/4b CR data"%weightname
    data_cr_3b_categ,data_sr_3b_categ,data_rest_3b_categ        = SelectRegions(dataset,'3b',categ,valflag)
    del dataset
    data_cr_4b_categ,data_sr_4b_categ,data_rest_4b_categ        = SelectRegions(data_rest_3b_categ,'4b',categ,valflag)

    #Get weights for 3b SR data as a function of H1_m_rot
    print "[INFO] Building BDT-reweighting model prediction %s in the 3b SR data as function of transverse mass (if GGF1 or GGF2)"%weightname
    if 'GGF'in weightname: norm_categ,norm_unc_categ  = BuildNormModel(data_cr_3b_categ,data_cr_4b_categ,data_sr_3b_categ,'H1_m_rot',valflag) 
    print "[INFO] Building BDT-reweighting or Transfer Factor model for %s using the 3b/4b CR data"%weightname
    #Get weights ,model, transferfactor for 3b CR data
    if 'VBF2' in weightname:
        weights_cr_categ,transferfactor_categ                       = BuildTransferFactorModel(data_cr_3b_categ,data_cr_4b_categ,categ,'%s_%s'%(tag,weightname))
    else:
        weights_cr_categ,reweightermodel_categ,transferfactor_categ = BuildReweightingModel(data_cr_3b_categ,data_cr_4b_categ,categ,'%s_%s'%(tag,weightname),bkgclassifierparams,bkgparams)
    del data_cr_4b_categ        

    #Get weights for 3b SR data
    print "[INFO] Calculating BDT-reweighting model prediction %s in the 3b SR data"%weightname
    if 'VBF2' in weightname:
        weights_sr_categ   = ApplyTransferFactorModel(transferfactor_categ,data_sr_3b_categ,data_sr_4b_categ,tag,categ,valflag,True)        
    else:
        weights_sr_categ   = ApplyReweightingModel(reweightermodel_categ,transferfactor_categ,data_sr_3b_categ,data_sr_4b_categ,tag,bkgclassifierparams,categ,valflag,True) 
    #Get weights for the rest of the data
    print "[INFO] Calculating BDT-reweighting model prediction %s in the rest of the data"%weightname
    if 'VBF2' in weightname:
        weights_rest_categ = ApplyTransferFactorModel(transferfactor_categ,data_rest_3b_categ,data_rest_4b_categ,tag,categ,valflag,False)
    else:
        weights_rest_categ = ApplyReweightingModel(reweightermodel_categ,transferfactor_categ,data_rest_3b_categ,data_rest_4b_categ,tag,bkgclassifierparams,categ,valflag,False)
    del data_sr_4b_categ,data_rest_4b_categ
    #Get Weights for 3b SR data as a function of the variable
    print "[INFO] Calculating BDT-reweighting model prediction %s in the 3b SR data as a function of a variable (if GGF1 or GGF2)"%weightname
    if 'GGF' in weightname: 
        weights_sr_categ_new = weights_sr_categ*(1/weights_sr_categ.sum())*norm_categ
        print "   -The new normalization will be ",norm_categ,"+/-",norm_unc_categ  

    #Add the weights to the dataframes
    print "[INFO] Adding weight %s_New (if GGF1 or GGF2)"%weightname
    if 'GGF' in weightname: data_sr_3b_categ[weightname+'_New'] = weights_sr_categ_new
    print "[INFO] Adding weight %s"%weightname
    data_cr_3b_categ[weightname]       = weights_cr_categ 
    data_sr_3b_categ[weightname]       = weights_sr_categ
    data_rest_3b_categ[weightname]     = weights_rest_categ
    print "[INFO] Adding training id for MVA training"
    if weightname=='Weight_AnaGGF' or weightname=='Weight_AnaGGF1' or weightname=='Weight_AnaGGF2':
        #Create fraction labels for mva training
        l = int(len(data_sr_3b_categ.index) / 2)
        data_sr_3b_categ = data_sr_3b_categ.reset_index(drop=True)
        data_sr_3b_categ.loc[l:,"%s_Frac"%weightname] = 0.0 #frac1 sample will create weights1
        data_sr_3b_categ.loc[:l,"%s_Frac"%weightname] = 2.0 #frac2 sample will create weights2
        #Re-evaluate the mva score swapping the mva training weights accordingly
        if weightname=='Weight_AnaGGF1':
              data_sr_3b_categ.drop('GGFMVA1', axis=1, inplace=True)
              data_sr_3b_categ_1 = data_sr_3b_categ[(data_sr_3b_categ.Weight_AnaGGF1_Frac==0)]
              data_sr_3b_categ_2 = data_sr_3b_categ[(data_sr_3b_categ.Weight_AnaGGF1_Frac==2)]
              data_sr_3b_categ_1['GGFMVA1'] = AddGGFMVA(data_sr_3b_categ_1,ggfmva1file2) #frac1 evaluated with weights2
              data_sr_3b_categ_2['GGFMVA1'] = AddGGFMVA(data_sr_3b_categ_2,ggfmva1file1) #frac2 evaluated with weights1
              data_sr_3b_categ_1['GGFMVA1btag'] = AddGGFMVA(data_sr_3b_categ_1,ggfmva1file2btag) #frac1 evaluated with weights2
              data_sr_3b_categ_2['GGFMVA1btag'] = AddGGFMVA(data_sr_3b_categ_2,ggfmva1file1btag) #frac2 evaluated with weights1
              #merge them again
              data_sr_3b_categ = pandas.concat( (data_sr_3b_categ_1,data_sr_3b_categ_2))
        if weightname=='Weight_AnaGGF2':
              data_sr_3b_categ_1 =  data_sr_3b_categ[(data_sr_3b_categ.Weight_AnaGGF2_Frac==0)]
              data_sr_3b_categ_2 =  data_sr_3b_categ[(data_sr_3b_categ.Weight_AnaGGF2_Frac==2)]
              data_sr_3b_categ_1['GGFMVA2'] = AddGGFMVA(data_sr_3b_categ_1,ggfmva2file2) #frac1 evaluated with weights2
              data_sr_3b_categ_2['GGFMVA2'] = AddGGFMVA(data_sr_3b_categ_2,ggfmva2file1) #frac2 evaluated with weights1
              data_sr_3b_categ_1['GGFMVA2btag'] = AddGGFMVA(data_sr_3b_categ_1,ggfmva2file2btag) #frac1 evaluated with weights2
              data_sr_3b_categ_2['GGFMVA2btag'] = AddGGFMVA(data_sr_3b_categ_2,ggfmva2file1btag) #frac2 evaluated with weights1
              #merge them again
              data_sr_3b_categ = pandas.concat( (data_sr_3b_categ_1,data_sr_3b_categ_2))
 
    #Merge (concatenate) them and return the dataframe
    print "[INFO] Concatenating all events in the dataframe"
    datasetwithweight = pandas.concat( (data_cr_3b_categ,data_sr_3b_categ,data_rest_3b_categ), ignore_index=True   )
    del data_cr_3b_categ,data_sr_3b_categ,data_rest_3b_categ
    value  = numpy.ones(dtype='float64',shape=len(datasetwithweight))
    datasetwithweight[weightname+"_tfactor"] = numpy.multiply(value,transferfactor_categ)
    datasetwithweight                        = datasetwithweight.reset_index(drop=True)

    return datasetwithweight

def RunReweightingModel(dataset,bkgparams,classifierparams,case,directory,tag,valflag,vbfflag,subcatflag,ggfmva1file1,ggfmva2file1,ggfmva1file2,ggfmva2file2,ggfmva1file1btag,ggfmva2file1btag,ggfmva1file2btag,ggfmva2file2btag):
    #Add weights for analysis
    if valflag==True  and vbfflag==False: #Validation GGF
        if subcatflag==1:
            dataset=AddModelWeights(dataset,bkgparams,classifierparams,'Weight_ValGGF1','GGF1',valflag,'%s'%(tag),ggfmva1file1,ggfmva2file1,ggfmva1file2,ggfmva2file2,ggfmva1file1btag,ggfmva2file1btag,ggfmva1file2btag,ggfmva2file2btag)
        elif subcatflag==2:
            dataset=AddModelWeights(dataset,bkgparams,classifierparams,'Weight_ValGGF2','GGF2',valflag,'%s'%(tag),ggfmva1file1,ggfmva2file1,ggfmva1file2,ggfmva2file2,ggfmva1file1btag,ggfmva2file1btag,ggfmva1file2btag,ggfmva2file2btag)
        else:
            dataset=AddModelWeights(dataset,bkgparams,classifierparams,'Weight_ValGGF','GGF',valflag,'%s'%(tag),ggfmva1file1,ggfmva2file1,ggfmva1file2,ggfmva2file2,ggfmva1file1btag,ggfmva2file1btag,ggfmva1file2btag,ggfmva2file2btag)
    elif valflag==True and vbfflag==True: #Validation VBF
        if subcatflag==1:
            dataset=AddModelWeights(dataset,bkgparams,classifierparams,'Weight_ValVBF1','VBF1',valflag,'%s'%(tag),ggfmva1file1,ggfmva2file1,ggfmva1file2,ggfmva2file2,ggfmva1file1btag,ggfmva2file1btag,ggfmva1file2btag,ggfmva2file2btag)
        elif subcatflag==2:
            dataset=AddModelWeights(dataset,bkgparams,classifierparams,'Weight_ValVBF2','VBF2',valflag,'%s'%(tag),ggfmva1file1,ggfmva2file1,ggfmva1file2,ggfmva2file2,ggfmva1file1btag,ggfmva2file1btag,ggfmva1file2btag,ggfmva2file2btag)
        else:
            dataset=AddModelWeights(dataset,bkgparams,classifierparams,'Weight_ValVBF','VBF',valflag,'%s'%(tag),ggfmva1file1,ggfmva2file1,ggfmva1file2,ggfmva2file2,ggfmva1file1btag,ggfmva2file1btag,ggfmva1file2btag,ggfmva2file2btag)
    elif valflag==False and vbfflag==False: #Analysis GGF
        if subcatflag==1: 
            dataset=AddModelWeights(dataset,bkgparams,classifierparams,'Weight_AnaGGF1','GGF1',valflag,'%s'%(tag),ggfmva1file1,ggfmva2file1,ggfmva1file2,ggfmva2file2,ggfmva1file1btag,ggfmva2file1btag,ggfmva1file2btag,ggfmva2file2btag)
        elif subcatflag==2:
            dataset=AddModelWeights(dataset,bkgparams,classifierparams,'Weight_AnaGGF2','GGF2',valflag,'%s'%(tag),ggfmva1file1,ggfmva2file1,ggfmva1file2,ggfmva2file2,ggfmva1file1btag,ggfmva2file1btag,ggfmva1file2btag,ggfmva2file2btag)
        else:
            dataset=AddModelWeights(dataset,bkgparams,classifierparams,'Weight_AnaGGF','GGF',valflag,'%s'%(tag),ggfmva1file1,ggfmva2file1,ggfmva1file2,ggfmva2file2,ggfmva1file1btag,ggfmva2file1btag,ggfmva1file2btag,ggfmva2file2btag)        
    elif valflag==False and vbfflag==True: #Analysis VBF
        if subcatflag==1: 
            dataset=AddModelWeights(dataset,bkgparams,classifierparams,'Weight_AnaVBF1','VBF1',valflag,'%s'%(tag),ggfmva1file1,ggfmva2file1,ggfmva1file2,ggfmva2file2,ggfmva1file1btag,ggfmva2file1btag,ggfmva1file2btag,ggfmva2file2btag)
        elif subcatflag==2:
            dataset=AddModelWeights(dataset,bkgparams,classifierparams,'Weight_AnaVBF2','VBF2',valflag,'%s'%(tag),ggfmva1file1,ggfmva2file1,ggfmva1file2,ggfmva2file2,ggfmva1file1btag,ggfmva2file1btag,ggfmva1file2btag,ggfmva2file2btag)
        else:
            dataset=AddModelWeights(dataset,bkgparams,classifierparams,'Weight_AnaVBF','VBF',valflag,'%s'%(tag),ggfmva1file1,ggfmva2file1,ggfmva1file2,ggfmva2file2,ggfmva1file1btag,ggfmva2file1btag,ggfmva1file2btag,ggfmva2file2btag)
    else:
        "[ERROR] No weights are calculated, incorrect selection!"       
    return dataset

#############COMMAND CODE IS BELOW ######################

###########OPTIONS
parser = argparse.ArgumentParser(description='Command line parser of skim options')
parser.add_argument('--config',  dest='cfgfile',  help='Name of config file',   required = True)
parser.add_argument('--casename',dest='casename', help='Case name',   required = True)
parser.add_argument('--eos',    dest='eos', action='store_true', help='Save skims on eos')
parser.add_argument('--no-eos', dest='eos', action='store_false',help='Do not save skims on eos')
parser.set_defaults(eos=False)
args = parser.parse_args()
configfilename = args.cfgfile
case           = args.casename
eos            = args.eos
###########Read Config file
print "[INFO] Reading skim configuration file . . ."
cfgparser = ConfigParser()
cfgparser.read('%s'%configfilename)
##########Get skim variables
print "[INFO] Getting configuration parameters . . ."
eosdirectory   = ast.literal_eval(cfgparser.get("configuration","eosdirectory"))
print "    -The eos directory:"
print "      *",eosdirectory
directory   = ast.literal_eval(cfgparser.get("configuration","directory"))
print "    -The directory:"
print "      *",directory
seed        = ast.literal_eval(cfgparser.get("configuration","seed"))
print "    -The seed:"
print "      *",seed
tag         = ast.literal_eval(cfgparser.get("configuration","tag"))
print "    -The tag name:"
print "      *",tag 
ggf1anabkgparams = ast.literal_eval(cfgparser.get("configuration","ggf1anabkgparams"))
print "    -The ggf-hh1 ana background parameters (trees, learning rate, max depth, min sample leaf, subsample, randomseed):"
print "      *",ggf1anabkgparams
ggf2anabkgparams = ast.literal_eval(cfgparser.get("configuration","ggf2anabkgparams"))
print "    -The ggf-hh2 ana background parameters (trees, learning rate, max depth, min sample leaf, subsample, randomseed):"
print "      *",ggf2anabkgparams
vbfanabkgparams  = ast.literal_eval(cfgparser.get("configuration","vbfanabkgparams"))
print "    -The vbf-hh ana background parameters (trees, learning rate, max depth, min sample leaf, subsample, randomseed):"
print "      *",vbfanabkgparams
ggf1valbkgparams = ast.literal_eval(cfgparser.get("configuration","ggf1valbkgparams"))
print "    -The ggf-hh1 ana background parameters (trees, learning rate, max depth, min sample leaf, subsample, randomseed):"
print "      *",ggf1valbkgparams
ggf2valbkgparams = ast.literal_eval(cfgparser.get("configuration","ggf2valbkgparams"))
print "    -The ggf-hh2 ana background parameters (trees, learning rate, max depth, min sample leaf, subsample, randomseed):"
print "      *",ggf2valbkgparams
vbfvalbkgparams  = ast.literal_eval(cfgparser.get("configuration","vbfvalbkgparams"))
print "    -The vbf-hh ana background parameters (trees, learning rate, max depth, min sample leaf, subsample, randomseed):"
print "      *",vbfvalbkgparams
ggfmva1file1        = ast.literal_eval(cfgparser.get("configuration","GGFMVA1file1"))
print "    -The ggfmva1 file1:   ",ggfmva1file1
ggfmva2file1        = ast.literal_eval(cfgparser.get("configuration","GGFMVA2file1"))
print "    -The ggfmva2 file1:   ",ggfmva2file1
ggfmva1file2        = ast.literal_eval(cfgparser.get("configuration","GGFMVA1file2"))
print "    -The ggfmva1 file1:   ",ggfmva1file2
ggfmva2file2        = ast.literal_eval(cfgparser.get("configuration","GGFMVA2file2"))
print "    -The ggfmva2 file1:   ",ggfmva2file2
ggfmva1file1btag        = ast.literal_eval(cfgparser.get("configuration","GGFMVA1file1btag"))
print "    -The ggfmva1 file1:   ",ggfmva1file1btag
ggfmva2file1btag        = ast.literal_eval(cfgparser.get("configuration","GGFMVA2file1btag"))
print "    -The ggfmva2 file1:   ",ggfmva2file1btag
ggfmva1file2btag        = ast.literal_eval(cfgparser.get("configuration","GGFMVA1file2btag"))
print "    -The ggfmva1 file1:   ",ggfmva1file2btag
ggfmva2file2btag        = ast.literal_eval(cfgparser.get("configuration","GGFMVA2file2btag"))
print "    -The ggfmva2 file1:   ",ggfmva2file2btag

##########Make background miodel
print "[INFO] Making background model . . . "
print "[INFO] The bkgmodel will be saved in eos: ",eos

#Get data and create panda dataframes with specific variables, a.k.a. slicing the data. Then, create background model based on control region data
datalist = []
if eos==True:
   datalist.append('%s/outputskims/%s/%s/SKIM_Data.root'%(eosdirectory,case,directory))
else:
   datalist.append('outputskims/%s/%s/SKIM_Data.root'%(case,directory)) 
dataset  = data.root2pandas(datalist,'bbbbTree')

if directory=='20172018': 
    dataset = shuffle(dataset,random_state=seed)
    dataset = dataset.reset_index(drop=True) 

if directory=='2016':
        dataset = shuffle(dataset,random_state=seed)
        dataset = dataset.reset_index(drop=True)

#classifierparameters
classifierparams=[100,0.6,2020]

#Clock
starttime = time.time()

#Add for analysis region
dataset = RunReweightingModel(dataset,ggf1anabkgparams,classifierparams,case,directory,tag,False,False,1,ggfmva1file1,ggfmva2file1,ggfmva1file2,ggfmva2file2,ggfmva1file1btag,ggfmva2file1btag,ggfmva1file2btag,ggfmva2file2btag)
dataset = RunReweightingModel(dataset,ggf2anabkgparams,classifierparams,case,directory,tag,False,False,2,ggfmva1file1,ggfmva2file1,ggfmva1file2,ggfmva2file2,ggfmva1file1btag,ggfmva2file1btag,ggfmva1file2btag,ggfmva2file2btag)
dataset = RunReweightingModel(dataset,vbfanabkgparams,classifierparams,case,directory,tag,False,True,1  ,ggfmva1file1,ggfmva2file1,ggfmva1file2,ggfmva2file2,ggfmva1file1btag,ggfmva2file1btag,ggfmva1file2btag,ggfmva2file2btag)
dataset = RunReweightingModel(dataset,vbfanabkgparams,classifierparams,case,directory,tag,False,True,2  ,ggfmva1file1,ggfmva2file1,ggfmva1file2,ggfmva2file2,ggfmva1file1btag,ggfmva2file1btag,ggfmva1file2btag,ggfmva2file2btag)

######Add for validation region
dataset = RunReweightingModel(dataset,ggf1valbkgparams,classifierparams,case,directory,tag,True,False,1,ggfmva1file1,ggfmva2file1,ggfmva1file2,ggfmva2file2,ggfmva1file1btag,ggfmva2file1btag,ggfmva1file2btag,ggfmva2file2btag)
dataset = RunReweightingModel(dataset,ggf2valbkgparams,classifierparams,case,directory,tag,True,False,2,ggfmva1file1,ggfmva2file1,ggfmva1file2,ggfmva2file2,ggfmva1file1btag,ggfmva2file1btag,ggfmva1file2btag,ggfmva2file2btag)
dataset = RunReweightingModel(dataset,vbfvalbkgparams,classifierparams,case,directory,tag,True,True,1  ,ggfmva1file1,ggfmva2file1,ggfmva1file2,ggfmva2file2,ggfmva1file1btag,ggfmva2file1btag,ggfmva1file2btag,ggfmva2file2btag)
dataset = RunReweightingModel(dataset,vbfvalbkgparams,classifierparams,case,directory,tag,True,True,2  ,ggfmva1file1,ggfmva2file1,ggfmva1file2,ggfmva2file2,ggfmva1file1btag,ggfmva2file1btag,ggfmva1file2btag,ggfmva2file2btag)

###Save everything in a root file
if eos==True:
   print "[INFO] Saving background model weight in file . . . "
   data.pandas2root(dataset,'bbbbTree',   '%s/outputskims/%s/%s/SKIM_BKG_MODEL_tree.root'%(eosdirectory,case,directory)  )
   data.roothist2root(datalist,'eff_histo','%s/outputskims/%s/%s/SKIM_BKG_MODEL_hist.root'%(eosdirectory,case,directory)  )
   os.system("hadd -f %s/outputskims/%s/%s/SKIM_BKG_MODEL.root      %s/outputskims/%s/%s/SKIM_BKG_MODEL_tree.root %s/outputskims/%s/%s/SKIM_BKG_MODEL_hist.root"%(eosdirectory,case,directory,eosdirectory,case,directory,eosdirectory,case,directory))
   os.system("rm      %s/outputskims/%s/%s/SKIM_BKG_MODEL_tree.root %s/outputskims/%s/%s/SKIM_BKG_MODEL_hist.root"%(eosdirectory,case,directory,eosdirectory,case,directory) )         
else:
   print "[INFO] Saving background model weight in file . . . "
   data.pandas2root(dataset,'bbbbTree',   'outputskims/%s/%s/SKIM_BKG_MODEL_tree.root'%(case,directory)  )
   data.roothist2root(datalist,'eff_histo','outputskims/%s/%s/SKIM_BKG_MODEL_hist.root'%(case,directory)  )
   os.system("hadd -f outputskims/%s/%s/SKIM_BKG_MODEL.root outputskims/%s/%s/SKIM_BKG_MODEL_tree.root outputskims/%s/%s/SKIM_BKG_MODEL_hist.root"%(case,directory,case,directory,case,directory))
   os.system("rm outputskims/%s/%s/SKIM_BKG_MODEL_tree.root outputskims/%s/%s/SKIM_BKG_MODEL_hist.root"%(case,directory,case,directory) )              
print('Running the background model took {} seconds'.format(time.time() - starttime))
