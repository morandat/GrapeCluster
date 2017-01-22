#!/bin/bash

#Usage function to print how to use
usage() {
	echo "armmanager [-m] [-u] <file> [-o <destination>]"
}

#Global Variables
FILE=""
DESTINATION=""
MOUNTOPT=0

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
			;;
		#where to output
		-o | --output)
			DESTINATION="$2"
			shift # past argument
			;;
		#need to know usage
		-h | --help)
			usage
			;;
		#default : input file
		*)
			FILE="$1"
			;;
	esac
	shift # past argument or value
done

#If no file is given
if [ FILE == "" ]
then    
    usage 
    exit 1
fi

