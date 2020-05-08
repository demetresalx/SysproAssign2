#!/bin/bash

#Ana8esh hmeromhniwn. Sto bash oi metavlhtes einai by default global opote ginetai eukola modify apo dw.
give_date () {
	#apo 2000 mexri kai 2020
	year=$((2000 + RANDOM % 21))
	#apo 1 mexri 12
	month=$((1 + RANDOM % 12))
	#apo 1 mexri 31 me eksaireseis gia 30 kai 28 h 29
	if (( $month == 1 || $month == 3 || $month == 5 || $month == 7 || $month == 8 || $month == 10 || $month == 12 ))
	then
		day=$((1 + RANDOM % 31))
	elif (( $month == 4 || $month == 6 || $month == 9 || $month == 11 ))
	then
		day=$((1 + RANDOM % 30))
	else #febrouarios, 28 h 29 depending on year % 4
		if (( $year % 4 == 0 )) #disekto, mexri 29
		then
			day=$((1 + RANDOM % 29))
		else
			day=$((1 + RANDOM % 28))
		fi
	fi
	
}

#./create_infiles.sh diseasesFile countriesFile input_dir numFilesPerDirectory numRecordsPerFile
#An den exoume akribws ta katallhla orismata, akuro
if [[ "$#" -ne 5 ]]
then
	echo "Wrong number of arguments. Try again..."
	exit 1
fi

#apo8hkeush timwn
diseasesFile=$1
countriesFile=$2
input_dir=$3
numFilesPerDirectory=$4
numRecordsPerFile=$5
#elegxos thetikothtas parametrwn
if (( numFilesPerDirectory <= 0 || numRecordsPerFile <= 0 ))
then
	echo "Invalid Parameters. Use positive ones..."
	exit 1
fi
#elegxos oti ta arxeia gia ious kai xwres yparxoyn
if [[ ! -e "$diseasesFile" || ! -e "$countriesFile" ]]
then
	echo "diseases or countries file does not exist."
	exit 1
fi
#Diabazw ta arxeia iwn kai xwrwn kai krataw ta periexomena tous. Apaloifh duplicates wste na mhn exoume 2 fores to idio subdir
declare -A dis_array
declare -A cou_array
num_dis=0
num_cou=0
while IFS= read -r line
do
	if [[ "${dis_array[@]}" =~ "$line" ]] #an periexetai hdh stous ious auto poy diabasame, agnohse to
	then
		continue
	fi
	dis_array[$num_dis]=$line
	num_dis=$(( num_dis + 1 ))
done < $diseasesFile
#echo ${dis_array[@]}
#to idio gia countries
while IFS= read -r line
do
	if [[ "${cou_array[@]}" =~ "$line" ]] #an periexetai hdh stis xwres ayto poy diabasame, agnohse to
	then
		continue
	fi
	cou_array[$num_cou]=$line
	num_cou=$(( num_cou + 1 ))
done < $countriesFile
#echo ${cou_array[@]}
#elegxw an ta 2 arxeia exoun toulaxiston 1 grammh
if (( num_dis <= 0 || num_cou <= 0 ))
then
	echo "Disease or Country file is empty..."
	exit 1
fi
#ftiaxnw input directory
if [[ ! -d $input_dir ]]
then
	mkdir $input_dir
fi
#ftiaxnw ena subdirectory gia kathe xwra
year=0
month=0
day=0
for i in $(seq 0 $((num_cou-1)) )
do #an den yparxei to subdirectory, ftiaksto
	if [[ ! -d "$input_dir/${cou_array[$i]}" ]]
	then
		mkdir "$input_dir/${cou_array[$i]}"
	fi
	#ftiaxnw mesa ekei twra ta numFilesPerDirectory arxeia
	for j in $(seq 1 $numFilesPerDirectory )
	do
		give_date #kalw th sunarthsh poy dinei times sta year month day
		date="$day-$month-$year"
		if [[ ! -f "$input_dir/${cou_array[$i]}/$date" ]]
		then
			> "$input_dir/${cou_array[$i]}/$date"
		else
			j=$((j-1)) #as paei ena pisw se periptwsh poy prokypsei idio onoma arxeiou wste na prokyptei panta o swstos arithmos arxeiwn
		fi
		#echo haya
	done
done







