#!/bin/bash

#function for assigning value to dates. Defined here to save lines of code
give_date () {
	echo janna
}

#./create_infiles.sh diseasesFile countriesFile input_dir numFilesPerDirectory numRecordsPerFile
#if we have more or less arguments than we SHOULD have. first is 0 so we need to check for 5
if [[ "$#" -ne 5 ]]
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
#read diseases & countries file and save diseases/countries for use. Ommit duplicates
declare -A dis_array
declare -A cou_array
num_dis=0
num_cou=0
while IFS= read -r line
do
	if [[ "${dis_array[@]}" =~ "$line" ]]
	then
		continue
	fi
	dis_array[$num_dis]=$line
	num_dis=$(( num_dis + 1 ))
done < $diseasesFile
#echo ${dis_array[@]}
#do same for countries file
while IFS= read -r line
do
	if [[ "${cou_array[@]}" =~ "$line" ]]
	then
		continue
	fi
	cou_array[$num_cou]=$line
	num_cou=$(( num_cou + 1 ))
done < $countriesFile
#echo ${cou_array[@]}
#checking if disease & country files contain at least one line
if (( num_dis <= 0 || num_cou <= 0 ))
then
	echo "Disease or Country file is empty..."
	exit 1
fi
#make input directory as assigned
if [[ ! -d $input_dir ]]
then
	mkdir $input_dir
fi
#make a subdirectory for every country (named after it)
for i in $(seq 0 $num_cou)
do #if the subdirectory doesnt exist, make it (for each corresponding country)
	if [[ ! -d "$input_dir/${cou_array[$i]}" ]]
	then
		mkdir "$input_dir/${cou_array[$i]}"
	fi
done
give_date






