import numpy 
import os
import matplotlib
import root_numpy
import pandas
import glob
from root_numpy import root2array
from numpy.lib.recfunctions import stack_arrays

def eventbtagregion(data,btagregion):
	#Select b-tagging region
	if btagregion   == '4b':
			selected = data[  data.nBtag==4 ]
			rejected = data[  data.nBtag!=4 ]
	elif btagregion == '3b': 
			selected = data[  data.nBtag==3 ]
			rejected = data[  data.nBtag!=3 ]
	else:
			selected = data
			rejected = pandas.DataFrame()
			print "[INFO] The b-tagging region was not specified, the data is b-tagging inclusive! Then rejected dataframe is empty!" 
	return  selected,rejected	
	
def eventcategory(data,category):
	if   category == 'VBF':
		 selected = data[ ((data.GGFKiller >=0.5) & (data.VBFEvent==1)) ]
		 rejected = data[~((data.GGFKiller >=0.5) & (data.VBFEvent==1)) ] 
	elif category == 'VBF1':
		 selected = data[ ((data.GGFKiller >=0.5) & (data.VBFEvent==1) & (data.GGFKiller < 0.975)) ]
		 rejected = data[~((data.GGFKiller >=0.5) & (data.VBFEvent==1) & (data.GGFKiller < 0.975)) ]		 
	elif category == 'VBF2':
		 selected = data[ ((data.GGFKiller >=0.5) & (data.VBFEvent==1) & (data.GGFKiller >= 0.975)) ]
		 rejected = data[~((data.GGFKiller >=0.5) & (data.VBFEvent==1) & (data.GGFKiller >= 0.975)) ]
	elif category == 'GGF':
		 selected = data[ (((data.GGFKiller <0.5) & (data.VBFEvent==1)) | (data.VBFEvent==0)) ]    	
		 rejected = data[~(((data.GGFKiller <0.5) & (data.VBFEvent==1)) | (data.VBFEvent==0)) ]
	elif category == 'GGF1':
		 selected = data[ (  ((data.GGFKiller <0.5) & (data.VBFEvent==1) & (data.HH_m<450)) | ((data.VBFEvent==0) & (data.HH_m<450))  )  ]    	
		 rejected = data[~(  ((data.GGFKiller <0.5) & (data.VBFEvent==1) & (data.HH_m<450)) | ((data.VBFEvent==0) & (data.HH_m<450))  )  ]
	elif category == 'GGF2':
		 selected = data[ (  ((data.GGFKiller <0.5) & (data.VBFEvent==1) & (data.HH_m>=450)) | ((data.VBFEvent==0) & (data.HH_m>=450))  )  ]    	
		 rejected = data[~(  ((data.GGFKiller <0.5) & (data.VBFEvent==1) & (data.HH_m>=450)) | ((data.VBFEvent==0) & (data.HH_m>=450))  )  ]
	elif category == 'preVBF':
		 selected = data[ (data.VBFEvent==1)]
		 rejected = data[~(data.VBFEvent==1)]
	else:
		 selected = data
		 rejected = pandas.DataFrame()
		 print "[INFO] The hh-category was not specified, the data is category inclusive! Then rejected dataframe is empty!"
	return selected,rejected

def eventmassregion(data,massregion,validation=False):
	#The radius of the circles
	radius1  = 25
	radius2  = 50
	# is it validation regions or analysis region?
	if validation:
		center1 = 182
		center2 = 175
	else:
		center1 = 125
		center2 = 120    	
	if   massregion == 'CR':
		selected = data[ (((data.H1_m-center1)**2+(data.H2_m-center2)**2)**(0.5) >= radius1) & (((data.H1_m-center1)**2+(data.H2_m-center2)**2)**(0.5) <= radius2) ]
		rejected = data[  ~(   (((data.H1_m-center1)**2+(data.H2_m-center2)**2)**(0.5) >= radius1) & (((data.H1_m-center1)**2+(data.H2_m-center2)**2)**(0.5) <= radius2)  )  ] 
	elif massregion == 'SR':
		selected = data[((data.H1_m-center1)**2+(data.H2_m-center2)**2)**(0.5) <  radius1 ]
		rejected = data[((data.H1_m-center1)**2+(data.H2_m-center2)**2)**(0.5) >= radius1 ]
	else:
		selected = data
		rejected = pandas.DataFrame()
		print "[INFO] The mass region was not specify, the data is mass region inclusive! Then rejected dataframe is empty!"
	return selected,rejected


def eventselection(data,btagregion=None,category=None,massregion=None,validation=None):

	#Define what the selections is:
	if btagregion==None: btagname='btag-inclusive'
	else: btagname='%s'%btagregion
	if category==None: categname='categ-inclusive'
	else: categname='%s'%category
	if massregion==None: massname='massreg-inclusive'
	else: massname='%s'%massregion
	if validation==False: valname='anareg'
	else: valname='valreg'
	selectionname="%s/%s/%s/%s"%(btagname,categname,massname,valname)
	blindname    ="%s/%s/%s"%(btagname,massname,valname)
 
	#selected data
	databtagregionselected,databtagregionrejected = eventbtagregion(data,btagregion)
	datacategoryselected,datacategoryrejected     = eventcategory(databtagregionselected,category)
	dataselected,datamassregionrejected           = eventmassregion(datacategoryselected,massregion,validation) 

	#rejected data
	datarejected    = pandas.concat((databtagregionrejected,datacategoryrejected,datamassregionrejected), ignore_index=True)
	del databtagregionrejected,datacategoryrejected,datamassregionrejected

    #printout
	print "[INFO] DATAFRAME SLICING CHECK FOR %s"%selectionname   
	print "    -Number of events in dataset (before) = ",len(data) 
	if blindname != '4b/SR/anareg': print "    -Number of selected events            = ",len(dataselected)  
	if blindname != '4b/SR/anareg': print "    -Number of rejected events            = ",len(datarejected)
	print "    -Number of events in dataset (after)  = ",int(len(dataselected)+len(datarejected))
	return dataselected,datarejected