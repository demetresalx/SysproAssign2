#!/bin/bash

#./create_infiles.sh diseasesFile countriesFile input_dir numFilesPerDirectory numRecordsPerFile
#if we have more or less arguments than we SHOULD have. first is 0 so we need to check for 5
if [ "$#" -ne 5 ]
then
	echo "Wrong number of arguments. Try again..."
	exit 1
fi

#extracting from arguments to variables
diseasesFile=$1
countriesFile=$2
input_dir=$3
numFilesPerDirectory=$4
numRecordsPerFile=$5
#checking for positive numerical values
if (( numFilesPerDirectory <= 0 || numRecordsPerFile <= 0 ))
then
	echo "Invalid Parameters. Use positive ones..."
	exit 1
fi
