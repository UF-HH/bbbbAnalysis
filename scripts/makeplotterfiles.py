import numpy 
import os
import glob
import ast
import argparse
import sys
import copy
from  ConfigParser import *

def MakeFile(directory,sample,tagname):
	with open('plotterListFiles/%s/SKIM_%s.txt'%(tagname,sample), 'a') as the_file:
		the_file.write('mlskim/outputskims/%s/SKIM_%s.root\n'%(directory,sample) )

def MakePlotterFiles(directory,samples,tagname):
	os.system('mkdir plotterListFiles')
	os.system('mkdir plotterListFiles/%s/'%tagname)
	for sample in samples:
		sample = str(sample)
		MakeFile(directory,sample,tagname)
	MakeFile(directory,'BKG_MODEL',tagname)

#############COMMAND CODE IS BELOW ######################

###########OPTIONS
parser = argparse.ArgumentParser(description='Command line parser of skim options')
parser.add_argument('--config', dest='cfgfile',  help='Name of config file',   required = True)
args = parser.parse_args()
configfilename = args.cfgfile
###########Read Config file
print "[INFO] Reading skim configuration file . . ."
cfgparser = ConfigParser()
cfgparser.read('%s'%configfilename)
##########Get skim variables
print "[INFO] Getting configuration parameters . . ."
directory   = ast.literal_eval(cfgparser.get("configuration","directory"))
print "    -The directory:"
print "      *",directory
samples     = ast.literal_eval(cfgparser.get("configuration","samples"))
print "    -The list of samples:"
for x in range(len(samples)):
  print "      *",samples[x]
tagname     = ast.literal_eval(cfgparser.get("configuration","tag"))
print "    -The tagname:"
print "      *", tagname
##########Make microskims
print "[INFO] Making files for plotter . . . "
MakePlotterFiles(directory,samples,tagname)
