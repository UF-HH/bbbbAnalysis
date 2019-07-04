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
			sel  = data[  data.nBtag==4 ]
	elif btagregion == '3b': 
			sel  = data[  data.nBtag==3 ]
	else:
			sel = data
			print "The b-tagging region was not specify, the data is b-tagging inclusive!" 
	return sel	
	
def eventcategory(data,category):
	if category   == 'preVBF':
		 sel = data[ (data.VBFEvent == 1)]
	elif category == 'preGGF':
		 sel = data[ (data.VBFEvent == 0)]
	elif category == 'VBF':
		 sel = data[ (data.VBFEvent == 1) & (data.BDT1 >=0) ]
	elif category == 'GGF':
		 sel = data[(data.BDT1 <0)]    	
	else:
		 sel = data 
		 print "The hh-category was not specify, the data is category inclusive!"
	return sel
	##TODO:Here code for jet cleaning, pt, etc

def eventmassregion(data,massregion,validation=False):
	# 2016 is (115,)
	if validation:
		center1 = 220
		center2 = 210
	else:
		center1 = 115
		center2 = 110    	
	radius1  = 45
	radius2  = 60

	if   massregion == 'CR':
		sel  = data[ (((data.H1_m-center1)**2+(data.H2_m-center2)**2)**(0.5) >= radius1) & (((data.H1_m-center1)**2+(data.H2_m-center2)**2)**(0.5) <= radius2) ] 
		sel  = sel[ ~(((sel.H1_m-center1)**2 +(sel.H2_m-center2)**2)**(0.5) < radius1) ]
	elif massregion == 'SR':
		sel  = data[((data.H1_m-center1)**2+(data.H2_m-center2)**2)**(0.5) < radius1 ]
	else:
		sel  = data
		print "The mass region was not specify, the data is mass region inclusive!"
	return sel

def eventselection(data,btagregion=None,category=None,massregion=None,validation=None):
	databtagregion  = eventbtagregion(data,btagregion)
	datacategory    = eventcategory(databtagregion,category) 
	datamassregion  = eventmassregion(datacategory,massregion,validation)    
	return datamassregion