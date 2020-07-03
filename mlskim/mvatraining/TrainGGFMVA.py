import warnings
warnings.filterwarnings('ignore')
import numpy as np
import pandas as pd
from datetime import datetime
from sklearn.model_selection import GridSearchCV
from sklearn.metrics import roc_auc_score
from sklearn.model_selection import StratifiedKFold
from xgboost import XGBClassifier
from xgboost import plot_importance
from sklearn.model_selection import train_test_split
import uproot
from sklearn.metrics import roc_curve, auc
import matplotlib
from matplotlib import pyplot as plt
import seaborn as sns
from sklearn.metrics import mean_squared_error
import pickle
import argparse
from  ConfigParser import *
import ast
from collections import Counter
import math
import os
import ROOT

def SaveXGBoostModel(classifier,vars_training,label,cat):
    ## save the model as pickle
    xg_model  = classifier.best_estimator_
    outname   = 'mymodels/%s%s_bdt_xgboost.pkl'%(label,cat)
    print '... pickling the output as', outname
    outfile   = open(outname, 'wb')
    to_save   = {
        'vars'  : vars_training,
        'model' : xg_model,
    }
    pickle.dump (to_save, outfile)
    outfile.close()

def getefficiencies(tr,y_true,y_pred,weights):
    signorm  = 0 
    bkgnorm  = 0
    sigcount = 0
    bkgcount = 0
    wts = weights.to_numpy()
    lbs = y_true.to_numpy()
    for k in range(0,len(y_true)):
        if lbs[k]==1: #get signal info
           signorm+=wts[k]
           if y_pred[k]>=tr: sigcount+=wts[k]
        else:            #get bkg info
           bkgnorm+=wts[k]
           if y_pred[k]>=tr: bkgcount+=wts[k]           
    sigeff = float(sigcount/signorm)
    bkgeff = float(bkgcount/bkgnorm) 
    return sigeff,bkgeff

def computekspvalue(histofmt,train,test,weights_train,weights_test):
    h_train = ROOT.TH1F("histo1","histo1",histofmt[0],histofmt[1],histofmt[2])
    h_test  = ROOT.TH1F("histo2","histo2",histofmt[0],histofmt[1],histofmt[2])

    smp_train = train
    smp_test  = test
    wts_train = weights_train.to_numpy()
    wts_test  = weights_test.to_numpy()
    #Fill train histogram
    for k in range(0,len(train)): h_train.Fill(smp_train[k],wts_train[k])
    #Fill test  histogram
    for k in range(0,len(test)): h_test.Fill(smp_test[k],wts_test[k])
    h_train.Sumw2()
    h_test.Sumw2()
    #Compute the goodnes of fit test
    ks = h_train.KolmogorovTest(h_test)
    return ks


def my_roc_curve(y_true, y_pred,weights):
    thresholds = [0.1,0.15,0.2,0.25,0.3,0.35,0.4,0.45,0.5,0.55,0.6,0.65,0.7,0.75,0.8,0.85,0.9,0.95]
    fpr=[]
    tpr=[]
    #Initialize
    fpr.append(1)
    tpr.append(1)
    #Intermediate
    for tr in thresholds:
       sigeff,bkgeff = getefficiencies(tr,y_true,y_pred,weights)
       fpr.append(bkgeff)
       tpr.append(sigeff)
    #Finalize
    fpr.append(0)
    tpr.append(0)       
    return fpr, tpr, thresholds
def CalculateSumw2Errors(original,original_weights,fmt):
    #variables and format
    sumw2 = [] 
    nbins = fmt[0]
    binwidth = abs(float(fmt[2]-fmt[1])/float(fmt[0]))
    bines = [ [((x)*binwidth)+ fmt[1], ((x+1)*binwidth)+fmt[1]] for x in range(0,int(nbins)) ]
    sumw2 = [ 0 for x in range(0,int(nbins)) ]
    original_weights = original_weights.to_numpy()
    #Calculation of sqrt(sumW2)
    for k in range(0,len(original)):
        for x in range(0,int(nbins)):
            if original[k]<=bines[x][1] and original[k]>bines[x][0]: sumw2[x]+=original_weights[k]**2
    entries_original_error = np.sqrt(sumw2)
    return entries_original_error

def timer(start_time=None):
    if not start_time:
        start_time = datetime.now()
        return start_time
    elif start_time:
        thour, temp_sec = divmod((datetime.now() - start_time).total_seconds(), 3600)
        tmin, tsec = divmod(temp_sec, 60)
        print('\n Time taken: %i hours %i minutes and %s seconds.' % (thour, tmin, round(tsec, 2)))#

######## Plot ranking of training variables
def plot_ranking(classifier,variables,label,cat):
    print '     ... drawing feature importances'
    ranking     = [ ['%s'%variables[i],classifier.best_estimator_.feature_importances_[i]] for i in range(len(variables))]
    ranking.sort(key=lambda s: s[1],reverse = True)
    importances = [round(ranking[i][1],3) for i in range(len(variables))] 
    features    = ['%s'%ranking[i][0] for i in range(len(variables))] 
    matplotlib.rcParams.update({'font.size': 60})
    plt.figure(figsize=[25, 25], dpi=40)
    plt.subplots_adjust(left=0.30)
    plt.barh(np.arange(len(importances)),importances,align='center',color='red')
    plt.yticks(np.arange(len(features)), features,fontsize=40)
    plt.xlabel('importances',fontsize=80)
    plt.grid(True)
    plt.title('feature importances: %s%s'%(label,cat),fontsize=40)
    plt.savefig("myplots/%s%s/%s%s_importances.png"%(label,cat,label,cat) )
    plt.close()

######## Plot classifier output in train/test samples
def GetKSTest(all_data_train,all_data_test,all_vars,classifier):
    sig_train = all_data_train[all_data_train.truth == 1][all_vars]
    bkg_train = all_data_train[all_data_train.truth == 0][all_vars]    
    sig_test  = all_data_test[all_data_test.truth == 1][all_vars]
    bkg_test  = all_data_test[all_data_test.truth == 0][all_vars]
    ## these are already normalised to unity
    weights_sig_train = all_data_train[all_data_train.truth == 1]['train_w']
    weights_bkg_train = all_data_train[all_data_train.truth == 0]['train_w']    
    weights_sig_test  = all_data_test[all_data_test.truth == 1]['train_w']
    weights_bkg_test  = all_data_test[all_data_test.truth == 0]['train_w']
    ## but before the splitting, so re-normalise
    weights_sig_train = weights_sig_train.multiply(1./weights_sig_train.sum())
    weights_bkg_train = weights_bkg_train.multiply(1./weights_bkg_train.sum())
    weights_sig_test  = weights_sig_test.multiply(1./weights_sig_test.sum())
    weights_bkg_test  = weights_bkg_test.multiply(1./weights_bkg_test.sum())
    #if n_classses > 2 sig proba is the last one (in the way the code is written) 
    Y_pred_sig_train = classifier.predict_proba(sig_train)[:,1]
    Y_pred_bkg_train = classifier.predict_proba(bkg_train)[:,1]
    Y_pred_sig_test  = classifier.predict_proba(sig_test)[:,1]
    Y_pred_bkg_test  = classifier.predict_proba(bkg_test)[:,1]
    # Get histograms of the classifiers
    fmt = [10,0,1]
    nbins = fmt[0]
    binwidth = abs(float(fmt[2]-fmt[1])/float(fmt[0]))
    Histo_training_S = np.histogram(Y_pred_sig_train,bins=fmt[0],range=(fmt[1],fmt[2]),weights=weights_sig_train)
    Histo_training_B = np.histogram(Y_pred_bkg_train,bins=fmt[0],range=(fmt[1],fmt[2]),weights=weights_bkg_train)
    Histo_testing_S  = np.histogram(Y_pred_sig_test,bins=fmt[0],range=(fmt[1],fmt[2]),weights=weights_sig_test)
    Histo_testing_B  = np.histogram(Y_pred_bkg_test,bins=fmt[0],range=(fmt[1],fmt[2]),weights=weights_bkg_test)
    #Compute the KS p-values from ROOT
    ks_sig_p = computekspvalue(fmt, Y_pred_sig_train,Y_pred_sig_test,weights_sig_train,weights_sig_test)
    ks_bkg_p = computekspvalue(fmt, Y_pred_bkg_train,Y_pred_bkg_test,weights_bkg_train,weights_bkg_test)
    print "           *KS p-value signal (background): %.2f(%.2f) "%(ks_sig_p,ks_bkg_p)

    return ks_sig_p,ks_bkg_p 
def plot_classifier_output(all_data_train,all_data_test,all_vars,classifier,label,cat):
    print '     ... drawing discriminator output'
    sig_train = all_data_train[all_data_train.truth == 1][all_vars]
    bkg_train = all_data_train[all_data_train.truth == 0][all_vars]    
    sig_test  = all_data_test[all_data_test.truth == 1][all_vars]
    bkg_test  = all_data_test[all_data_test.truth == 0][all_vars]
    ## these are already normalised to unity
    weights_sig_train = all_data_train[all_data_train.truth == 1]['train_w']
    weights_bkg_train = all_data_train[all_data_train.truth == 0]['train_w']    
    weights_sig_test  = all_data_test[all_data_test.truth == 1]['train_w']
    weights_bkg_test  = all_data_test[all_data_test.truth == 0]['train_w']
    ## but before the splitting, so re-normalise
    weights_sig_train = weights_sig_train.multiply(1./weights_sig_train.sum())
    weights_bkg_train = weights_bkg_train.multiply(1./weights_bkg_train.sum())
    weights_sig_test  = weights_sig_test.multiply(1./weights_sig_test.sum())
    weights_bkg_test  = weights_bkg_test.multiply(1./weights_bkg_test.sum())
    #if n_classses > 2 sig proba is the last one (in the way the code is written) 
    Y_pred_sig_train = classifier.predict_proba(sig_train)[:,1]
    Y_pred_bkg_train = classifier.predict_proba(bkg_train)[:,1]
    Y_pred_sig_test  = classifier.predict_proba(sig_test)[:,1]
    Y_pred_bkg_test  = classifier.predict_proba(bkg_test)[:,1]
    # Get histograms of the classifiers
    fmt = [10,0,1]
    nbins = fmt[0]
    binwidth = abs(float(fmt[2]-fmt[1])/float(fmt[0]))
    Histo_training_S = np.histogram(Y_pred_sig_train,bins=fmt[0],range=(fmt[1],fmt[2]),weights=weights_sig_train)
    Histo_training_B = np.histogram(Y_pred_bkg_train,bins=fmt[0],range=(fmt[1],fmt[2]),weights=weights_bkg_train)
    Histo_testing_S  = np.histogram(Y_pred_sig_test,bins=fmt[0],range=(fmt[1],fmt[2]),weights=weights_sig_test)
    Histo_testing_B  = np.histogram(Y_pred_bkg_test,bins=fmt[0],range=(fmt[1],fmt[2]),weights=weights_bkg_test)
    #Compute the KS p-values from ROOT
    ks_sig_p = computekspvalue(fmt, Y_pred_sig_train,Y_pred_sig_test,weights_sig_train,weights_sig_test)
    ks_bkg_p = computekspvalue(fmt, Y_pred_bkg_train,Y_pred_bkg_test,weights_bkg_train,weights_bkg_test)
    print "           *KS p-value signal (background): %.2f(%.2f) "%(ks_sig_p,ks_bkg_p)
    # Lets get the min/max of the Histograms
    AllHistos = [Histo_training_S,Histo_training_B,Histo_testing_S,Histo_testing_B]
    h_max     = max([histo[0].max() for histo in AllHistos])*1.2
    h_min     = min([histo[0].min() for histo in AllHistos])
    # Get the histogram properties (binning, widths, centers)
    bin_edges = Histo_training_S[1]
    bin_centers = ( bin_edges[:-1] + bin_edges[1:]  ) /2.
    bin_widths = (bin_edges[1:] - bin_edges[:-1])
    # To make error bar plots for the data, take the Poisson uncertainty sqrt(N)
    ErrorBar_testing_S = CalculateSumw2Errors(Y_pred_sig_test,weights_sig_test,fmt)
    ErrorBar_testing_B = CalculateSumw2Errors(Y_pred_bkg_test,weights_bkg_test,fmt)
    plt.clf() 
    #ratios for signal and background
    srathisty    = [ (Histo_testing_S[0][x]/Histo_training_S[0][x])  if Histo_training_S[0][x]>0.001  else 0 for x in range(0,int(nbins)) ]
    brathisty    = [ (Histo_testing_B[0][x]/Histo_training_B[0][x])  if Histo_training_B[0][x]>0.001  else 0 for x in range(0,int(nbins)) ]
    # Draw objects
    fig  = plt.figure(1, figsize=(40, 20))
    ax1  = plt.subplot2grid((3, 1), (0, 0), rowspan=3)
    # Draw solid histograms for the training data
    ax1.bar(bin_centers,Histo_training_S[0],facecolor='blue',linewidth=0,width=bin_widths,label='S (Train)',alpha=0.5)
    ax1.bar(bin_centers,Histo_training_B[0],facecolor='red',linewidth=0,width=bin_widths,label='B (Train)',alpha=0.5)
    # Draw error-bar histograms for the testing data
    ax1.errorbar(bin_centers, Histo_testing_S[0], yerr=ErrorBar_testing_S, xerr=None, ecolor='blue',c='blue',fmt='o',label='S (Test)')
    ax1.errorbar(bin_centers, Histo_testing_B[0], yerr=ErrorBar_testing_B, xerr=None, ecolor='red',c='red',fmt='o',label='B (Test)')
    # Make a colorful backdrop to show the clasification regions in red and blue
    ax1.axvspan(0.5,1.0, color='blue',alpha=0.08)
    ax1.axvspan(0.0,0.5, color='red',alpha=0.08)#
    #KS p-value
    ax1.text(0.20, h_max*0.8,'K-S test: signal(background) = %.3f(%.3f)'%(ks_sig_p,ks_bkg_p),fontsize=10) 
    # Adjust the axis boundaries (just cosmetic)
    ax1.axis([0, 1, h_min, h_max])
    # Make labels and title
    ax1.set_title("Classification with XGBoost:%s%s"%(label,cat))
    ax1.set_xlabel("ggFMVA%s output"%(cat))
    ax1.set_ylabel("Normalized Yields")
    # Make legend with smalll font
    legend = ax1.legend(loc='upper center', shadow=True,ncol=2)
    for alabel in legend.get_texts():
        alabel.set_fontsize('small')
    # Save the result to png
    plt.savefig("myplots/%s%s/%s%s_score_output.png"%(label,cat,label,cat))
    plt.close()

#########  Compute correlation matrix in signal and background
def plot_correlation_matrix_sigbkg(all_data,all_vars,label,cat):
    print '     ... drawing correlation matrices'
    sig_data = all_data[all_data.truth == 1][all_vars]
    bkg_data = all_data[all_data.truth == 0][all_vars]
    plot_correlation_matrix(sig_data,'%s'%label,cat,"sig")
    plot_correlation_matrix(bkg_data,'%s'%label,cat,"bkg")
def plot_correlation_matrix(data,label,cat,typ):
    corr = data.corr()
    plt.clf() 
    ax = sns.heatmap(corr, vmin=-1, vmax=1, center=0,cmap=sns.diverging_palette(20, 220, n=200),square=True)
    ax.set_xticklabels(ax.get_xticklabels(),rotation=45,horizontalalignment='right')    
    plt.subplots_adjust(left=0.20)
    plt.subplots_adjust(bottom=0.30)
    plt.title("%s correlation matrix (%s%s)"%(typ,label,cat))
    plt.savefig("myplots/%s%s/%s%s_%s_corr_matrix.png"%(label,cat,label,cat,typ))
    plt.close()
#########  Plot ROC curve and calculate ROC area
def GetROCAUC(X_test,Y_test,W_test,classifier):
    y_pred      = classifier.predict_proba(X_test)[:,1]
    fpr, tpr, _ = my_roc_curve(Y_test, y_pred, W_test)
    roc_auc     = auc(fpr, tpr)
    print "           *ROC-AUC (Test) = ",roc_auc
    return roc_auc
def plot_roc_curves(X_train,Y_train,W_train,X_test,Y_test,W_test,classifier,label,cat):
    print '     ... drawing roc curves'
    y_train  = classifier.predict_proba(X_train)
    y_test   = classifier.predict_proba(X_test)
    plot_roc_curve(Y_train, y_train[:,1], W_train,"%s"%label,"train",cat)
    plot_roc_curve(Y_test,   y_test[:,1], W_test, "%s"%label,"test",cat)
def plot_roc_curve(y_true, y_pred, weights, label, typ, cat):
    #fpr, tpr, _ = roc_curve(y_true, y_pred,sample_weight=weights)
    fpr, tpr, _ = my_roc_curve(y_true, y_pred,weights)
    roc_auc     = auc(fpr, tpr)
    print '        ... ROC AUC (%s%s %s): '%(label,cat,typ), roc_auc
    print '        ... drawing ROC curve'
    plt.figure()
    lw = 2
    plt.plot(fpr, tpr, color='darkorange',
             lw=lw, label='ROC curve (area = %0.2f)' % roc_auc)
    plt.plot([0, 1], [0, 1], color='navy', lw=lw, linestyle='--')
    plt.xlim([-0.02, 1.0])
    plt.ylim([0.0, 1.05])
    plt.xlabel('False Positive Rate')
    plt.ylabel('True Positive Rate')
    plt.title('ROC curve')
    plt.grid(True)
    plt.legend(loc="lower right")
    plt.savefig("myplots/%s%s/%s%s_%s_bdt_roc.png"%(label,cat,label,cat,typ))
    plt.close()

def PrintGridResults(grid_search):
    #print('\n GridCV results:')
    #print(grid_search.cv_results_)
    print('\n Best mean roc-auc score from cross-validation:')
    print(grid_search.best_score_ )
    print('\n Best hyperparameters:')
    print(grid_search.best_params_)
    print('\n Best estimator:')
    print(grid_search.best_estimator_)
    #print('\n Refiting time:')
    #print(grid_search.refit_time_)

def RunGridTraining(params,X_train,Y_train,W_train,imbalancefactor,parameters):
    #Fit the GridSearchCV
    xgbforgrid  = XGBClassifier(scale_pos_weight=imbalancefactor, objective='binary:logistic',silent=True, nthread=1, random_state = parameters[5])
    skf = StratifiedKFold(n_splits=2, shuffle = True, random_state = parameters[5])
    grid_search = GridSearchCV(xgbforgrid, refit=True, param_grid=params, scoring='roc_auc', n_jobs=10, cv=skf.split(X_train,Y_train,W_train), fit_params={'sample_weight':W_train}, verbose=1)
    print '\n      ... fitting GridSearchCV to training set using 2-fold cross validation'
    start_time = timer(None) 
    grid_search.fit(X_train,Y_train)
    timer(start_time) 

    return grid_search

def RunGGFMVATraining(grids,parameters):

    ###Category and tag
    categ = parameters[7]
    tag   = '%s%s'%(parameters[6],parameters[7])        

    ## open sig and bkg datasets
    print "\n... opening input files"
    input_sig  = '%s/%s/SKIM_%s_BR.root'%(parameters[0],parameters[1],parameters[2])
    input_bkg  = '%s/%s/SKIM_%s_BR.root'%(parameters[0],parameters[1],parameters[3])
    arrs_bkg  = uproot.open(input_bkg)['bbbbTree']
    arrs_sig  = uproot.open(input_sig)['bbbbTree']

    ## convert to dataframes
    print "\n... converting to pandas"
    vars_training = parameters[4]
    all_vars = vars_training+['Weight_AnaGGF%s_MVA'%categ]
    all_vars = list(set(all_vars))
    data_bkg = arrs_bkg.pandas.df(all_vars+['Weight_AnaGGF%s'%categ])
    data_sig = arrs_sig.pandas.df(all_vars+['HH_reweight']+['genWeight']+['bTagScaleFactor_central']    )

    #Preselection (half the background and all signal)
    data_bkg = data_bkg[ (data_bkg['Weight_AnaGGF%s_MVA'%categ]==0) | (data_bkg['Weight_AnaGGF%s_MVA'%categ]==1) ]
    data_sig = data_sig[ (data_sig['Weight_AnaGGF%s_MVA'%categ]==0) | (data_sig['Weight_AnaGGF%s_MVA'%categ]==1) ]

    #Remove Weight_MVA ID variable
    data_bkg.drop('Weight_AnaGGF%s_MVA'%categ, axis=1, inplace=True)
    data_sig.drop('Weight_AnaGGF%s_MVA'%categ, axis=1, inplace=True)
    all_vars = vars_training
    all_vars = list(set(all_vars))
    print "\n... list of input variables"
    for var in all_vars: print "   *",var 

    #Preselection (low mas or high mass) into the two categories
    if categ=='1':
        data_bkg = data_bkg[ (data_bkg['HH_m']<450) ]
        data_sig = data_sig[ (data_sig['HH_m']<450) ]
    else:
        data_bkg = data_bkg[ (data_bkg['HH_m']>=450) ]
        data_sig = data_sig[ (data_sig['HH_m']>=450) ]        
    ## prepare train weights for signal and background
    data_bkg['train_w'] = data_bkg['Weight_AnaGGF%s'%categ]
    data_bkg.drop('Weight_AnaGGF%s'%categ, axis=1, inplace=True)
    data_sig['train_w'] = data_sig['HH_reweight']*data_sig['genWeight']*data_sig['bTagScaleFactor_central']
    data_sig.drop("HH_reweight", axis=1, inplace=True)
    data_sig.drop("genWeight",   axis=1, inplace=True)
    data_sig.drop("bTagScaleFactor_central",   axis=1, inplace=True)
    ## label the datasets as bkg (0) and signal (1)
    data_bkg['truth'] = 0
    data_sig['truth'] = 1

    all_data = pd.concat([data_bkg, data_sig], axis=0, sort=False)
    print '\n... number of input events'
    print '      *signal: ',len(data_sig)
    print '      *background: ',len(data_bkg)

    #prepare train/test datasets
    print '\n... preparing inputs for xgboost'
    all_data_train, all_data_test = train_test_split(all_data, test_size=0.25, random_state=parameters[5])
    X_train,Y_train,W_train       = all_data_train[all_vars],all_data_train['truth'],all_data_train['train_w']
    X_test,Y_test,W_test          = all_data_test[all_vars],all_data_test['truth'],all_data_test['train_w']
 
    ##Address the fact that is an imbalancede dataset by calculating a imbalance factor and later feed into the gridtraining 
    print '\n... preparing imbalance factor for train samples'
    imbalancefactor = float((all_data_train[all_data_train.truth == 0]['train_w']).sum()) / float((all_data_train[all_data_train.truth == 1]['train_w'] ).sum())
    print '      *the signal weight sum (train):',(all_data_train[all_data_train.truth == 1]['train_w']).sum()
    print '      *the background weight sum (train):',(all_data_train[all_data_train.truth == 0]['train_w']).sum()
    print '      *the imbalance factor:',imbalancefactor
    print '\n... preparing BDT hyperparameter grid'

    ##Loop over defined grids and only keep the best
    bestgrids = []
    ngrid=1
    for grid in grids:
        print '-----------------------------------------------GRID %i-----------------------------------------------'%ngrid
        print grid
        print '-----------------------------------------------------------------------------------------------------'
        classifier    = RunGridTraining(grid,X_train,Y_train,W_train,imbalancefactor,parameters)
        print 'The best result of this grid: ',classifier.best_params_
        ks_sig,ks_bkg = GetKSTest(all_data_train,all_data_test,all_vars,classifier)
        test_rocauc   = GetROCAUC(X_test,Y_test,W_test,classifier)   
        if ks_sig>=0.05 and ks_bkg>=0.05: bestgrids.append([classifier,test_rocauc])
        ngrid+=1
    print '-----------------------------------------------------------------------------------------------------'
 
    #Plot and save the best grid 
    if len(bestgrids)>=1:
        bestgrids.sort(key=lambda x:x[1],reverse=True)
        bestgrid = bestgrids[0][0]
        #Print results and make plots only for the best grid 
        print '\n... the best hyperparameters and its cross-validation results'
        PrintGridResults(bestgrid)
        print '\n... making plots related to the training/testing'    
        os.system("mkdir myplots")
        os.system("mkdir myplots/%s%s"%(parameters[6],parameters[7])  )
        plot_classifier_output(all_data_train,all_data_test,all_vars,bestgrid,parameters[6],parameters[7])
        plot_roc_curves(X_train,Y_train,W_train,X_test,Y_test,W_test,bestgrid,parameters[6],parameters[7])
        plot_correlation_matrix_sigbkg(all_data,all_vars,parameters[6],parameters[7])
        plot_ranking(bestgrid,all_vars,parameters[6],parameters[7])
        print '\n... saving model'  
        SaveXGBoostModel(bestgrid,vars_training,parameters[6],parameters[7]) 
    else:
        print '\n... none of the defined grids are good for us!!!! :('      

#############COMMAND CODE IS BELOW ######################

###########OPTIONS
parser = argparse.ArgumentParser(description='Command line parser of skim options')
parser.add_argument('--config',   dest='cfgfile',   help='Name of config file',   required = True)
parser.add_argument('--case',     dest='case',      help='Case name',   required = True)
parser.add_argument('--categ',    dest='categ',     help='(1) Low Mass or High Mass',   required = True)
args = parser.parse_args()
configfilename = args.cfgfile
case           = args.case
categ          = args.categ

###########Read Config file
print "[INFO] Reading skim configuration file . . ."
cfgparser = ConfigParser()
cfgparser.read('%s'%configfilename)

##########Get skim variables
print "[INFO] Getting training configuration . . ."
print "    -The ntuples directory:"
print "      *",case
directory   = ast.literal_eval(cfgparser.get("configuration","directory"))
print "    -The ntuples subdirectory:"
print "      *",directory
sigsamples  = ast.literal_eval(cfgparser.get("configuration","GGFMVASignal"))
bkgsamples  = ast.literal_eval(cfgparser.get("configuration","GGFMVABackground"))
print "    -The list of training samples:"
print "      *Signal 1:",sigsamples[0]
print "      *Signal 2:",sigsamples[1]
print "      *Background:",bkgsamples[0]  
variables   = ast.literal_eval(cfgparser.get("configuration","GGFMVAVariables"))
print "    -The list of training variables:"
for var  in variables:
    print "      *",var
print "    -The category (1. Low-mass and 2.High mass):"
print "      *",categ
randseed    = ast.literal_eval(cfgparser.get("configuration","GGFMVAseed"))
print "    -The random seed:"
print "      *",randseed 
tag         = ast.literal_eval(cfgparser.get("configuration","GGFMVAtag"))
print "    -The tag:"
print "      *",tag  

print "[INFO] Starting XGBoost training . . ."
##define your grid parameters
grid1 = {
        'n_estimators':    [100,150],
        'learning_rate':   [0.05,0.1],
        'max_depth':       [2],
        }
grid2={
        'n_estimators':    [100,150],
        'learning_rate':   [0.05,0.1],
        'max_depth':       [3],    
}
grid3={
        'n_estimators':    [100,150],
        'learning_rate':   [0.05,0.1],
        'max_depth':       [4],    
}
grid4 = {
        'n_estimators':    [200,250],
        'learning_rate':   [0.05,0.1],
        'max_depth':       [2],
        }
grid5={
        'n_estimators':    [200,250],
        'learning_rate':   [0.05,0.1],
        'max_depth':       [3],    
}
grid6={
        'n_estimators':    [200,250],
        'learning_rate':   [0.05,0.1],
        'max_depth':       [4],    
}
grid7 = {
        'n_estimators':    [300,350],
        'learning_rate':   [0.05,0.1],
        'max_depth':       [2],
        }
grid8={
        'n_estimators':    [300,350],
        'learning_rate':   [0.05,0.1],
        'max_depth':       [3],    
}
grid9={
        'n_estimators':    [300,350],
        'learning_rate':   [0.05,0.1],
        'max_depth':       [4],    
}
grids = [grid1,grid2,grid3,grid4,grid5,grid6,grid7,grid8,grid9]

#Run the training
if categ != '1' and categ != '2':
    print "[ERROR] No training is performed, no category was specified"
else:
    if categ =='1' :
        trainingarguments=[case,directory,sigsamples[0],bkgsamples[0],variables,randseed[0],tag,categ]
        RunGGFMVATraining(grids,trainingarguments)
    else:
        trainingarguments=[case,directory,sigsamples[1],bkgsamples[0],variables,randseed[1],tag,categ]
        RunGGFMVATraining(grids,trainingarguments)        
    print "[INFO] XGBoost training is finished . . ."