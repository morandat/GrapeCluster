#!/bin/bash

#Usage function to print how to use
usage() {
	echo "armmanager [-m] [-u <destination>] <file> [-o <destination>]"
}

#Global Variables
FILE=""
DESTINATION=""
MOUNTOPT=0
DIRECTORY=""

#If no parameter given, wtf ?
if [ -z "$1" ]; then
	usage
	exit 1
fi

##Options handling
while [[ $# -gt 1 ]]; do
	key="$1"

	case $key in
		#A file is given
		-f | --file)
			FILE="$2"
			shift # past argument
			;;
		#Mount
		-m | --mount)
			MOUNTOPT=1
			;;
		#unmount
		-u | --unmount)
			MOUNTOPT=2
			DIRECTORY="$2"
			shift
			;;
		#where to output
		-o | --output)
			DESTINATION="$2"
			shift # past argument
			;;
		#need to know usage
		-h | --help)
			usage
			exit 0
			;;
		#default : input file
		*)
			FILE="$1"
			;;
	esac
	shift # past argument or value
done


#If no file is given
if [ "$FILE" == "" ]
then    
    usage 
    exit 1
fi

#Mount the filesystem
if [ $MOUNTOPT -le 1 ]
then
	if [[ $FILE == *.img ]] || [[ $FILE == *.iso ]]
	then
		#If no destination given
		if [ "$DESTINATION" == "" ]
		then
			DESTINATION="./.armanager"
		fi
		#Create directory for destination
		mkdir -p $DESTINATION
		guestmount -a $FILE -m /dev/sda2 $DESTINATION
		if [ $? -ne 0 ]
		then
			echo "Something went wrong when trying to mount"
			exit 1
		else
			echo "Successfully mounted $FILE to $DESTINATION"
		fi
	else
		echo "Please give a valid file ... (img/iso)"
		usage
		exit 1
	fi
fi

if [ $MOUNTOPT -eq 2 ]
then
	guestunmount $DESTINATION
fi