#!/bin/bash
# usage: ./getFilesFromDatasetList.sh <dataset_list_file> > <file_list_file>

dasgoclient --query="file dataset=/BTagCSV/Run2016B-17Jul2018_ver2-v1/MINIAOD" --unique > tmpList.txt
# filename="tmpList.txt"

while IFS='' read -r line || [[ -n "$line" ]]; do
	if [ "${line:0:1}" = "#" -o "$line" = "" ]; then
		continue
	fi
    runList="$(eval "dasgoclient --query \"run file=$line\" --unique")" # | read runList
    # echo $runList
    if [[ $runList = *"273409"* ]]; then
  		echo $line
  		eval "dasgoclient --query \"lumi file=$line\" --unique"
	fi
done < "tmpList.txt"

rm tmpList.txt
