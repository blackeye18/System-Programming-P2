#!/bin/bash
#Test file creator for 2nd assigment of SysPro
#sdi1800115 - Mavrommatis Panagiotis
SOURCE="${BASH_SOURCE[0]}"
#apothikefsh arxikhs tehshs
#elegxos an yparxei hdh h xwra ston pinaka
function not_duplicate_country(){
	flag1=1
	for i in ${countries_array_onetime[@]}
	do
		
		if [ $temp == $i ] 
		then
			flag1=0
			break
		fi
	done
	echo $flag1
}
#arguments check
if [ $# -ne 3 ]
then
	echo "Wrong number of arguents!"
	exit 1
fi

inputFile=$1
input_dir=$2
numFilesPerDirectory=$3
#an yparxei hdh to fakelos me idio onoma
if [ -d $input_dir ]
then
	echo "input_dir Already exists! Exiting!"
	exit 1
fi
#ama yparxei to inputfile to diavazei se pinaka kai apothikevei to plithos twn gramwn
if [ -f $inputFile ]
then
	readarray input_array < $inputFile
	input_counter=${#input_array[@]}
	echo "Inputs in inputFile: $input_counter"
else
	echo "Error! File $inputFile does not exist"
	exit 1
fi	
#elegxos an to numFilesPerDirectory einai arithmos megalyteros tou 0
 if [[ $numFilesPerDirectory =~ ^[0-9]+$ ]] && (( $numFilesPerDirectory > 0))
 then
	:
else
	echo "numFilesPerDirectory is NOT a >0 number! Exiting!"
	exit 1
fi
mkdir $input_dir
#dimiourgeia fakelou 
declare -A matrix
#gia kathe grammh
for ((i=0; i<$input_counter; i++))
do
	#spame thn grammh se kommatia analoga me ta kena
	read -r -a array <<< "${input_array[$i]}"
	for ((j=0; j<8; j++))
	do
		matrix[$i,$j]=${array[$j]}
	done
done
declare -A countries_array_onetime
count1=0
#apothikevoume tis xwres se enan pinaka, apo mia fora h kathe mia mono. 
for ((i=0; i<$input_counter; i++))
do
	temp=${matrix[$i,3]}
	#elegxoume an yparxei hdh, an den yparxei thn prosthetoume ston pinaka
	if (( $(not_duplicate_country) ))
	then
		countries_array_onetime[$count1]=${matrix[$i,3]}
		count1=$((count1+1))
	fi
done
echo "There are $count1 different Countries"

#mpainoume ston fakelo pou ftiaksame
cd $input_dir
#gia kathe xwra ftiaxnoume enan fakelo gia afth
for ((i=0; i<$count1; i++))
do
	mkdir ${countries_array_onetime[$i]}
done
#gia kathe xwra mpainoume ston fakelo ths
for ((i=0; i<$count1; i++))
do
	cd ${countries_array_onetime[$i]}
	for ((j=0; j<numFilesPerDirectory; j++))
	do
	#ftiaxnoume to onoma tou arxeiou
		tempstring=${countries_array_onetime[$i]}
		tempstring=$tempstring"-"
		tempstring=$tempstring$((j+1))
		tempstring=$tempstring".txt"
		touch $tempstring
		#kai to dimioyrgoume
		tempstring=""
	done
	cd - >  /dev/null #stelnoume thn eksodo ths edolhs gia na mhn emfanizetai sto terminal/stdout
done
#arxikopoioume ton pinaka
declare -A recs_per_country
for ((i=0; i<$count1; i++))
do
	recs_per_country[$i]=0
done
for ((i=0; i<$input_counter; i++))
do
	for((j=0; j<$count1; j++))
	do
		#gia kathe grammh tou arxeiou,kai gia kathe xwra 
		if [ ${matrix[$i,3]} == ${countries_array_onetime[$j]} ]
		then 
		#kanoume round robin, me thn metavlithth recpercountry
			(( recs_per_country[$j]++ ))
			if [ ${recs_per_country[$j]} \> $numFilesPerDirectory ]
			then
				recs_per_country[$j]=1
			fi
			#mpainoume sto adistoixo txt
			tempstring=${countries_array_onetime[$j]}
			tempstring=$tempstring"-"
			tempstring=$tempstring${recs_per_country[$j]}
			tempstring=$tempstring".txt"
			cd ${countries_array_onetime[$j]}
			for((k=0; k<8;k++))
			do
				#an eimaste sto telos ths grammhs eite me no eite me yes kanei break alliws vazei keno
				echo -n "${matrix[$i,$k]}" >> $tempstring
				if [ $k == 6 ] && [ ${matrix[$i,$k]} == "NO" ]
				then 
					break
				elif [ $k == 7 ]
				then
					break
				else
					echo -n " " >> $tempstring
				fi
			done
			#allagh grammhs
			echo "" >> $tempstring
			tempstring=""
			cd - >  /dev/null #stelnoume thn eksodo ths edolhs gia na mhn emfanizetai sto terminal/stdout
			break
		fi
	done
done

#elegxos oti exoun ftiaxtei ola swsta, toulaxiston san arxeia
for((j=0; j<$count1; j++))
do
	tempstring=${countries_array_onetime[$j]}
	tempstring=$tempstring"-"
	tempstring=$tempstring${recs_per_country[$j]}
	tempstring=$tempstring".txt"
	cd ${countries_array_onetime[$j]}
	for((k=1; k<=1;k++))
	do
		:
	done
	tempstring=""
	cd - >  /dev/null #stelnoume thn eksodo ths edolhs gia na mhn emfanizetai sto terminal/stdout
done
#rm -r -- $input_dir