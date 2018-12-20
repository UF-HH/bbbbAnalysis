import os
import sys
import argparse

parser = argparse.ArgumentParser(description='Command line parser of skim options')
parser.add_argument('--dataFolder',  dest = 'dataFolder',  help = 'folder containing the skims',  required = True )

args = parser.parse_args()

for dataSet in os.listdir(args.dataFolder):
	if dataSet[:5] != "SKIM_": continue
	fileListName =  "plotterListFiles/" + dataSet[5:] + ".txt"
	fileList = open(fileListName, 'w')
	fileFolder = args.dataFolder+"/"+dataSet+"/output/"
	for file in os.listdir(fileFolder):
		# print file[:11] , file[-5:]
		if  file[:11] != "bbbbNtuple_" or file[-5:] != ".root": continue
		# print fileFolder + "/" + file
		fileList.write(fileFolder + file + "\n")
	