#!/bin/bash
# usage: ./getFilesFromDatasetList.sh <dataset_list_file> > <file_list_file>

while IFS='' read -r line || [[ -n "$line" ]]; do
	if [ "${line:0:1}" = "#" -o "$line" = "" ]; then
		continue
	fi
    eval "dasgoclient --query \"file dataset=$line\" --unique"
done < "$1"
