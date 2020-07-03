import os
import glob
import ast
import argparse
import sys
import copy
import math
import pickle

def GetTrainingModel(file):
	# open picked file and use it back again
	infile     = open(file,'rb')
	rwmodel    = pickle.load(infile)
	infile.close()
	variables           = rwmodel.get('vars')
	model               = rwmodel.get('model') 
	print "\n"*2
	print "The file: ",file
	print "The variables: ",variables
	print "The model", model
	print "\n"*2

#############COMMAND CODE IS BELOW ######################

###########OPTIONS
parser = argparse.ArgumentParser(description='Command line parser of skim options')
parser.add_argument('--dataset',  dest='dataset',  help='Name of config file',   required = True)
parser.add_argument('--classifier',dest='classifier', help='Case name',   required = True)
args = parser.parse_args()
dataset    = args.dataset
classifier = args.classifier

file = '%s%s_bdt_xgboost.pkl'%(dataset,classifier)

#Make printing
GetTrainingModel(file)