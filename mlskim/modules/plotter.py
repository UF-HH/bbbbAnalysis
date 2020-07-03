import matplotlib
import numpy 
import os
import root_numpy
import pandas
import glob
import math
from root_numpy import root2array
from matplotlib import pyplot as plt
from numpy.lib.recfunctions import stack_arrays
from hep_ml.metrics_utils import ks_2samp_weighted
from scipy import stats

def Draw1DHistosComparison(original, target, variables, original_weights, norm, tag):
	#Create folder for plots
	os.system("mkdir myplots")
	#Normalize or not?
	if norm is True:
	  hist_settings = {'bins': 20, 'density': True, 'alpha': 0.5}
	else:
	  hist_settings = {'bins': 20, 'density': None, 'alpha': 0.5}
	#Create figure on matplotlib
	matplotlib.rcParams.update({'font.size': 35})
	plt.figure(figsize=[100, 60], dpi=50)
	i = 0
	for id, column in enumerate(variables, 1):
		xlim = numpy.percentile(numpy.hstack([target[column]]), [0.01, 99.99])
		plt.subplot(4, 2, id)
		plt.hist(original[column],label='Bkg. Model', weights=original_weights, range=xlim, **hist_settings)
		plt.hist(target[column],  label='4 btag (Target)', range=xlim, **hist_settings)
		plt.legend(loc='best')
		plt.title(column,fontsize=40)
		i+=1
	plt.savefig("myplots/distributions_%s.png"%tag)
	plt.close('all')

def Draw1DHisto(original, target, variable, original_weights, fmt, norm, tag):
	#Create folder for plots
	os.system("mkdir myplots")
	#Normalize or not?
	if norm is True:
	  hist_settings = {'bins': fmt[0], 'density': True, 'alpha': 0.4}
	else:
	  hist_settings = {'bins': fmt[0], 'density': None, 'alpha': 0.4}
	#Get parameters and observed data
	xlim  = [fmt[1],fmt[2]]
	nbins = fmt[0]
	binwidth = abs(float(fmt[2]-fmt[1])/float(fmt[0]))
	entries_target, edges_target = numpy.histogram(target[variable],nbins,xlim)
	#Create figure
	print "[INFO] Making observable validation plot: ",variable
	matplotlib.rcParams.update({'font.size': 50})
	plt.style.use('seaborn-whitegrid')
	fig  = plt.figure(1, figsize=(40, 20))
	ax1  = plt.subplot2grid((3, 1), (0, 0), rowspan=2)
	ax2  = plt.subplot2grid((3, 1), (2, 0))
	entries_original, edges_original, _original = ax1.hist(original[variable],label='Model',color='blue', weights=original_weights, range=xlim, **hist_settings)
	bin_centers_original = 0.5 * (edges_original[:-1] + edges_original[1:])
	bin_centers_target   = 0.5 * (edges_target[:-1] + edges_target[1:])
	#Calculate error bars
	print "     . . .  calculating error bars for plot"
	entries_target_error             = numpy.sqrt(entries_target)
	entries_original_error           = CalculateSumw2Errors(original,original_weights,variable,fmt)
	entries_original_statlike_error  = numpy.sqrt(entries_original)
	entries_pull_error               = [ math.sqrt( entries_original_error[x]**2 + entries_original_statlike_error[x]**2) for x in range(0,int(nbins)  )]
	reshistx    = [ fmt[1] + float(binwidth)/2 + (x*binwidth) for x in range(0,int(nbins)  )]
	reshisty    = [ (entries_target[x]-entries_original[x]) / entries_pull_error[x] if entries_target[x]!=0 and entries_pull_error[x]!=0 else 0 for x in range(0,int(nbins)) ]
	band1sigup  = [ 1 for x in range(0,int(nbins)) ]
	band1sigdn  = [-1 for x in range(0,int(nbins)) ]
	band2sigup  = [ 2 for x in range(0,int(nbins)) ]
	band2sigdn  = [-2 for x in range(0,int(nbins)) ]
	#Draw errorbars
	ax1.errorbar(bin_centers_original, entries_original, yerr=entries_original_error,     fmt='_b',label='$\sigma_{Model}$', ecolor='blue', ms=5, capsize=10, elinewidth=5,markeredgewidth=5)
	ax1.errorbar(bin_centers_target,     entries_target, yerr=numpy.sqrt(entries_target), fmt='or',label='Data', ecolor='red', ms=10, capsize=10, elinewidth=5,markeredgewidth=5)
	ax1.legend(loc='best', numpoints=1)
	ax1.set_title('Validation of %s'%variable,fontsize=50)
	ax1.set_ylabel('Events',fontsize=50)
	ax1.minorticks_on()
	ax2.bar(reshistx, height=band2sigup, alpha=1.0, edgecolor='green', linewidth=1.0, color='green',width = binwidth)
	ax2.bar(reshistx, height=band2sigdn, alpha=1.0, edgecolor='green', linewidth=1.0, color='green',width = binwidth)
	ax2.bar(reshistx, height=band1sigup, alpha=1.0, edgecolor='yellow', linewidth=1.0, color='yellow',width = binwidth)
	ax2.bar(reshistx, height=band1sigdn, alpha=1.0, edgecolor='yellow', linewidth=1.0, color='yellow',width = binwidth)
	ax2.bar(reshistx, height=reshisty, alpha=1.0, edgecolor='black', label='Pull', linewidth=2, width = binwidth, fill=False)
	ax2.set_ylim([-5,5])
	ax2.set_ylabel('(Data-Model)/$\sigma_{Tot}$',fontsize=50)
	ax2.set_xlabel(variable,fontsize=50)
	ax2.minorticks_on()
	print "     . . . saving plot"
	fig.savefig("myplots/distribution_observable_%s_%s.png"%(tag,variable) )
	plt.close()

def Draw1DHistoWithMC(original, target, mc, original_weights, mc_weights, variable, fmt, norm, tag):
	#Create folder for plots
	os.system("mkdir myplots")
	#Normalize or not?
	if norm is True:
	  hist_settings = {'bins': fmt[0], 'density': True, 'alpha': 0.4}
	else:
	  hist_settings = {'bins': fmt[0], 'density': None, 'alpha': 0.4}
	#Get parameters and observed data
	xlim  = [fmt[1],fmt[2]]
	nbins = fmt[0]
	binwidth = abs(float(fmt[2]-fmt[1])/float(fmt[0]))
	entries_target, edges_target = numpy.histogram(target[variable],nbins,xlim)
	#Create figure
	print "[INFO] Making observable validation plot: ",variable
	matplotlib.rcParams.update({'font.size': 40})
	plt.style.use('seaborn-whitegrid')
	fig  = plt.figure(1, figsize=(40, 20))
	ax1  = plt.subplot2grid((3, 1), (0, 0), rowspan=2)
	ax2  = plt.subplot2grid((3, 1), (2, 0))
	fig, (ax1, ax2, ax3) = plt.subplots(figsize=(40, 20), nrows=3)
	entries_original, edges_original, _original = ax1.hist(original[variable],label='Model',color='blue', weights=original_weights, range=xlim, **hist_settings)
	entries_mc, edges_mc, _mc                   = ax1.hist(mc[variable],      label='$t\overline{t}$',color='black', edgecolor='black',linewidth=5, histtype="step", weights=mc_weights, range=xlim, **hist_settings)
	bin_centers_original = 0.5 * (edges_original[:-1] + edges_original[1:])
	bin_centers_target   = 0.5 * (edges_target[:-1] + edges_target[1:])
	#Calculate error bars
	print "     . . .  calculating error bars for plot"
	entries_target_error       = numpy.sqrt(entries_target)
	entries_original_systerror = CalculateSumw2Errors(original,original_weights,variable,fmt)
	entries_original_staterror = numpy.sqrt(entries_original)
	entries_original_error     = [ math.sqrt( entries_original_systerror[x]**2 + entries_original_staterror[x]**2) for x in range(0,int(nbins)  )]
	reshistx    = [ fmt[1] + float(binwidth)/2 + (x*binwidth) for x in range(0,int(nbins)  )]
	reshisty    = [ (entries_target[x]-entries_original[x]) / entries_original_error[x] if entries_target[x]!=0 and entries_original_error[x]!=0 else 0 for x in range(0,int(nbins)) ]
	band1sigup  = [ 1 for x in range(0,int(nbins)) ]
	band1sigdn  = [-1 for x in range(0,int(nbins)) ]
	band2sigup  = [ 2 for x in range(0,int(nbins)) ]
	band2sigdn  = [-2 for x in range(0,int(nbins)) ]
	rathistx    = [ fmt[1] + float(binwidth)/2 + (x*binwidth) for x in range(0,int(nbins)  )]
	rathisty    = [ (entries_target[x]/entries_original[x])  if entries_original[x]!=0  else 0 for x in range(0,int(nbins)) ]
	ratband2sigup  = [ (((2*entries_original_error[x])+entries_original[x])/entries_original[x]) if entries_original[x]!=0 else 0 for x in range(0,int(nbins)) ]
	ratband1sigup  = [ ((entries_original_error[x]+entries_original[x])/entries_original[x]) if entries_original[x]!=0 else 0 for x in range(0,int(nbins)) ]
	ratband1sigdn  = [ 2-((entries_original_error[x]+entries_original[x])/entries_original[x]) if entries_original[x]!=0 else 0 for x in range(0,int(nbins)) ]
	ratband2sigdn  = [ 2-(((2*entries_original_error[x])+entries_original[x])/entries_original[x]) if entries_original[x]!=0 else 0 for x in range(0,int(nbins)) ]
	#Draw errorbars
	ax1.errorbar(bin_centers_original, entries_original, yerr=entries_original_error, fmt='_b',label='$\sigma_{Model}$', ecolor='blue', ms=5, capsize=10, elinewidth=5,markeredgewidth=5)
	ax1.errorbar(bin_centers_target, entries_target, yerr=numpy.sqrt(entries_target), fmt='or',label='Data', ecolor='red', ms=10, capsize=10, elinewidth=5,markeredgewidth=5)
	ax1.legend(loc='best', numpoints=1)
	ax1.set_title('Validation of %s'%variable,fontsize=50)
	ax1.set_ylabel('Events',fontsize=40)
	ax1.minorticks_on()
	ax2.bar(rathistx, height=ratband2sigup, alpha=1.0, edgecolor='green',  linewidth=1.0, color='green',width = binwidth)
	ax2.bar(rathistx, height=ratband1sigup, alpha=1.0, edgecolor='yellow', linewidth=1.0, color='yellow',width = binwidth)
	ax2.bar(rathistx, height=ratband1sigdn, alpha=1.0, edgecolor='green',  linewidth=1.0, color='green', width = binwidth)
	ax2.bar(rathistx, height=ratband2sigdn, alpha=1.0, edgecolor='white',  linewidth=1.0, color='white', width = binwidth)
	ax2.errorbar(rathistx,rathisty, fmt='ok',label='$\sigma_{Model}$', ecolor='black', ms=10, capsize=10, elinewidth=5,markeredgewidth=5)
	ax2.axhline(y=1.0, color='k', linestyle='-')
	ax2.set_ylim([0,2])
	ax2.set_ylabel('Data/Model',fontsize=40)
	ax2.minorticks_on()

	ax3.bar(reshistx, height=band2sigup, alpha=1.0, edgecolor='green', linewidth=1.0, color='green',width = binwidth)
	ax3.bar(reshistx, height=band2sigdn, alpha=1.0, edgecolor='green', linewidth=1.0, color='green',width = binwidth)
	ax3.bar(reshistx, height=band1sigup, alpha=1.0, edgecolor='yellow', linewidth=1.0, color='yellow',width = binwidth)
	ax3.bar(reshistx, height=band1sigdn, alpha=1.0, edgecolor='yellow', linewidth=1.0, color='yellow',width = binwidth)
	ax3.bar(reshistx, height=reshisty, alpha=1.0, edgecolor='black', label='Pull', linewidth=2, width = binwidth, fill=False)
	ax3.set_ylim([-5,5])
	ax3.set_ylabel('(Data-Model)/$\sigma_{Model}$',fontsize=40)
	ax3.set_xlabel(variable,fontsize=50)
	ax3.minorticks_on()

	print "     . . . saving plot"
	fig.savefig("myplots/distribution_observablewithMC_%s_%s.png"%(tag,variable) )
	plt.close()


def MultiDraw1DHistoWithMC(datainfo, datawts, mcinfo, mcwts, variable, fmt, norm, tag):
	#Create folder for plots
	os.system("mkdir myplots")
	#Normalize or not?
	if norm==True:
	  hist_settings = {'bins': fmt[0], 'density': True, 'alpha': 0.4}
	else:
	  hist_settings = {'bins': fmt[0], 'density': None, 'alpha': 0.4}
	#Get parameters and observed data
	xlim  = [fmt[1],fmt[2]]
	nbins = fmt[0]
	binwidth = abs(float(fmt[2]-fmt[1])/float(fmt[0]))
	#Get Data arrays
	entries_4b_sr, edges_4b_sr = numpy.histogram(datainfo[0][variable],nbins,xlim)
	entries_3b_sr, edges_3b_sr = numpy.histogram(datainfo[1][variable],nbins,xlim)
	entries_4b_cr, edges_4b_cr = numpy.histogram(datainfo[2][variable],nbins,xlim)
	entries_3b_cr, edges_3b_cr = numpy.histogram(datainfo[3][variable],nbins,xlim)
	bin_centers_4b_sr = 0.5 * (edges_4b_sr[:-1] + edges_4b_sr[1:])
	bin_centers_3b_sr = 0.5 * (edges_3b_sr[:-1] + edges_3b_sr[1:])
	bin_centers_4b_cr = 0.5 * (edges_4b_cr[:-1] + edges_4b_cr[1:])
	bin_centers_3b_cr = 0.5 * (edges_3b_cr[:-1] + edges_3b_cr[1:])
	#Create figure
	print "[INFO] Making observable validation plot: ",variable
	matplotlib.rcParams.update({'font.size': 50})
	plt.style.use('seaborn-whitegrid')
	fig, (ax1, ax2) = plt.subplots(figsize=(40, 20), nrows=2)
	entries_mc_4b_sr, edges_mc_4b_sr, _mc_4b_sr = ax1.hist(mcinfo[0][variable],label='$t\overline{t}$ (4b,SR)',color='red', edgecolor='red',linewidth=5, histtype="step",   weights=mcwts[0], range=xlim, **hist_settings)
	entries_mc_3b_sr, edges_mc_3b_sr, _mc_3b_sr = ax2.hist(mcinfo[1][variable],label='$t\overline{t}$ (3b,SR)',color='black', edgecolor='black',linewidth=5, histtype="step", weights=mcwts[1], range=xlim, **hist_settings)
	entries_mc_4b_cr, edges_mc_4b_cr, _mc_4b_cr = ax1.hist(mcinfo[2][variable],label='$t\overline{t}$ (4b,CR)',color='blue', edgecolor='blue',linewidth=5, histtype="step",  weights=mcwts[2], range=xlim, **hist_settings)
	entries_mc_3b_cr, edges_mc_3b_cr, _mc_3b_cr = ax2.hist(mcinfo[3][variable],label='$t\overline{t}$ (3b,CR)',color='green', edgecolor='green',linewidth=5, histtype="step", weights=mcwts[3], range=xlim, **hist_settings)
	#Draw up 4b sr/cr 
	ax1.errorbar(bin_centers_4b_sr, entries_4b_sr, yerr=numpy.sqrt(entries_4b_sr), fmt='or',label='Data(4b,SR)', ecolor='red', ms=10, capsize=10, elinewidth=5,markeredgewidth=5)
	ax1.errorbar(bin_centers_4b_cr, entries_4b_cr, yerr=numpy.sqrt(entries_4b_cr), fmt='ob',label='Data(4b,CR)', ecolor='blue', ms=10, capsize=10, elinewidth=5,markeredgewidth=5)
	ax1.legend(loc='best', numpoints=1)
	ax1.set_title('Validation Region Data vs $t\overline{t}$: %s'%variable,fontsize=50)
	ax1.set_ylabel('Events',fontsize=50)
	ax1.minorticks_on()
	#Draw bottom 3b sr/cr
	ax2.errorbar(bin_centers_3b_sr, entries_3b_sr, yerr=numpy.sqrt(entries_3b_sr), fmt='ok',    label='Data(3b,SR)', ecolor='black', ms=10, capsize=10, elinewidth=5,markeredgewidth=5)
	ax2.errorbar(bin_centers_3b_cr, entries_3b_cr, yerr=numpy.sqrt(entries_3b_cr), fmt='og',    label='Data(3b,CR)', ecolor='green', ms=10, capsize=10, elinewidth=5,markeredgewidth=5)
	ax2.set_ylabel('Events',fontsize=50)
	ax2.legend(loc='best', numpoints=1)
	ax2.set_xlabel(variable,fontsize=50)
	ax2.minorticks_on()
	print "     . . . saving plot"
	fig.savefig("myplots/distribution_multiregion_observablewithMC_%s_%s.png"%(tag,variable))
	plt.close()

def DrawDNNScoreComparison(yhat,y, norm, tag):
	#Normalize or not?
	if norm is True:
	  hist_settings = {'bins': 20, 'density': True, 'alpha': 0.5}
	else:
	  hist_settings = {'bins': 20, 'density': None, 'alpha': 0.5}
	#Create dataframes
	datayhat= numpy.array(yhat)
	datay= numpy.array(y)
	datatrain= pandas.DataFrame({'DNN':datayhat[:,0]})
	datatrain['Class'] = pandas.DataFrame(datay)  
	datatrain_signal=datatrain[datatrain.Class==1]
	datatrain_background=datatrain[datatrain.Class==0]
	#Create figure on matplotlib
	fig, (ax1, ax2) = plt.subplots(nrows=2)

	matplotlib.rcParams.update({'font.size': 20})
	plt.figure(figsize=(7,7))
	xlim = numpy.percentile(numpy.hstack(datatrain_signal['DNN']), [0.01, 99.99])
	plt.hist(datatrain_signal['DNN'],label='Signal',  range=xlim, **hist_settings)
	plt.hist(datatrain_background['DNN'],  label='Background', range=xlim, **hist_settings)
	plt.legend(loc='best')
	plt.title('DNN output (%s)'%tag,fontsize=20) 
	plt.savefig("myplots/nndistibutions_%s.png"%tag)    

def DrawDNNScore(yhat,y, norm, tag):
	#Normalize or not?
	if norm is True:
	  hist_settings = {'bins': 10, 'density': True, 'alpha': 0.5}
	else:
	  hist_settings = {'bins': 10, 'density': None, 'alpha': 0.5}
	#Create dataframes
	datayhat= numpy.array(yhat)
	datay= numpy.array(y)
	datatrain= pandas.DataFrame({'DNN':datayhat[:,0]})
	datatrain['Class'] = pandas.DataFrame(datay)  
	datatrain_signal=datatrain[datatrain.Class==1]
	datatrain_background=datatrain[datatrain.Class==0]
	#Create figure on matplotlib
	matplotlib.rcParams.update({'font.size': 20})
	plt.figure(figsize=(7,7))
	xlim = numpy.percentile(numpy.hstack(datatrain_signal['DNN']), [0.01, 99.99])
	plt.hist(datatrain_signal['DNN'],label='Signal',  range=xlim, **hist_settings)
	plt.hist(datatrain_background['DNN'],  label='Background', range=xlim, **hist_settings)
	plt.legend(loc='best')
	plt.title('DNN output (%s)'%tag,fontsize=20) 
	plt.savefig("myplots/nndistibution_%s.png"%tag) 

def CreateDataFrameForPlot(scores,labels):
	datayhat= numpy.array(scores)
	datay= numpy.array(labels)
	data= pandas.DataFrame({'DNN':datayhat[:,0]})
	data['Class'] = pandas.DataFrame(datay)  
	data_signal=data[data.Class==1]
	data_background=data[data.Class==0]
	return data_signal,data_background

def DrawDNNScoreComparison(yhat_train,y_train,yhat_test,y_test, norm, tag):
	#Create dataframes for train and test 
	datatrain_signal,datatrain_background = CreateDataFrameForPlot(yhat_train,y_train)
	datatest_signal,datatest_background   = CreateDataFrameForPlot(yhat_test,y_test)
	#Format of the histograms
	nbins = 40.0
	binwidth = float(1/nbins)
	dnnbin   = [                     (x*binwidth) for x in range(0,int(nbins+1))] 
	ratiobin = [ float(binwidth)/2 + (x*binwidth) for x in range(0,int(nbins)  )]
	print dnnbin
	print ratiobin 
	#KS TEST
	sig_d,sig_p = stats.ks_2samp(datatrain_signal['DNN'], datatest_signal['DNN'])
	bkg_d,bkg_p = stats.ks_2samp(datatrain_background['DNN'], datatest_background['DNN'])
	#Create figure on matplotlib
	matplotlib.rcParams.update({'font.size': 15})
	fig = plt.figure(1, figsize=(15, 15))
	ax1 = plt.subplot2grid((3, 1), (0, 0), rowspan=2)
	ax2 = plt.subplot2grid((3, 1), (2, 0))
	plot1 = ax1.hist(datatrain_signal['DNN'],label='Sig(train+val)', range=[0,1], color='blue',  bins=dnnbin, density=True, alpha=0.5)
	plot2 = ax1.hist(datatest_signal['DNN'] ,label='Sig(test)' , range=[0,1], color='darkblue', linewidth=2, histtype="step", bins=dnnbin, density=True, alpha=0.4)
	plot3 = ax1.hist(datatrain_background['DNN'],label='Bkg(train+val)', range=[0,1], color='red',  bins=dnnbin, density=True, alpha=0.5)
	plot4 = ax1.hist(datatest_background['DNN'] ,label='Bkg(test)' , range=[0,1], color='darkred', linewidth=2, histtype="step", bins=dnnbin, density=True, alpha=0.4)
	plot5 = ax1.text(0.35, 0.01,'P(KS-test) sig(bkg) = %.2f(%.2f)'%(sig_p,bkg_p),fontsize=10)
	ax1.set_ylabel("A. U.",fontsize=15)
	ax1.set_title('DNN output (%s)'%tag,fontsize=15) 
	ax1.legend(loc='upper center')
	plots5 = ax2.bar(ratiobin, height=( (plot2[0]-plot1[0])/plot1[0]), alpha=0.4, edgecolor='darkblue', linewidth=1.5, color='blue',width = binwidth)
	plots6 = ax2.bar(ratiobin, height=( (plot4[0]-plot3[0])/plot3[0]), alpha=0.4, edgecolor='darkred', linewidth=1.5, color='red',width = binwidth)
	ax2.set_ylim([-0.5, 0.5])
	ax2.axhline(y=0,ls='--',c='k',zorder=11,lw=1) 
	ax2.axhline(y=0.2,ls='--',c='k',zorder=11,lw=1) 
	ax2.axhline(y=-0.2,ls='--',c='k',zorder=11,lw=1) 
	ax2.set_ylabel('(Test-Model)/Model',fontsize=15)
	ax2.set_xlabel("DNN",fontsize=20)
	plt.savefig("myplots/nndistibutioncomparison_%s.png"%tag) 


def Draw2DHisto(data,var1,var2,x1,x2,y1,y2,tag):
	plt.figure(figsize=(10,10)) # square canvas
	_ = plt.hist2d(data['%s'%var1], data['%s'%var2], range=[[x1, x2], [y1, y2]], bins=60, cmap='viridis', norm=matplotlib.colors.LogNorm())
	plt.xlabel("%s"%var1, fontsize=20) 
	plt.ylabel("%s"%var2, fontsize=20)
	plt.title("%s"%tag,fontsize=20)
	_ = plt.colorbar() 
	plt.savefig("myplots/mh1mh2plane_%s.png"%tag) 
	plt.close()


def CalculateSumw2Errors(original,original_weights,variable,fmt):
	sumw2 = [] 
	nbins = fmt[0]
	binwidth = abs(float(fmt[2]-fmt[1])/float(fmt[0]))
	bines = [ [((x)*binwidth)+ fmt[1], ((x+1)*binwidth)+fmt[1]] for x in range(0,int(nbins)) ]
	sumw2 = [ 0 for x in range(0,int(nbins)) ]
	for k in range(0,len(original[variable])):
		for x in range(0,int(nbins)):
			if original[variable][k]<=bines[x][1] and original[variable][k]>bines[x][0]: sumw2[x]+=original_weights[k]**2
	entries_original_error = numpy.sqrt(sumw2)
	return entries_original_error