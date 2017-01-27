#!/bin/bash

#Usage function to print how to use
usage() {
	echo "armmanager [-m=<file>] [-u[=<destination>]] [-o=<destination>]"
}

realpath() {
    [[ $1 = /* ]] && echo "$1" || echo "$PWD/${1#./}"
}

#Global Variables
FILE=""
DESTINATION=""
MOUNTOPT=0
WORKING_PATH=""
MOUNT_ONLY=false

mount_image(){
	if [[ $FILE == *.img ]] || [[ $FILE == *.iso ]]
	then
		#If no destination given
		if [ "$DESTINATION" == "" ]
		then
			DESTINATION="./.armmanager"
			filename=`basename $FILE`
			WORKING_PATH="$DESTINATION/${filename%.*}"
			echo "Setting destination folder to default $WORKING_PATH"
		else
			WORKING_PATH=$DESTINATION
			echo "Destination folder is : $WORKING_PATH"
		fi

		#Create directory for destination
		mkdir -p $WORKING_PATH

		#to save working path
		#we could have used readlink or realpath, but this is special to "recent" linux, so for instance not for mac
		echo -n `realpath $WORKING_PATH` > .wpath

		#Seeking part to mount
		echo "Mounting Linux partition of $FILE"
		part=`fdisk -l -o Start $FILE | cut -d' ' -f1,3 | tail -n1`
		offset=$((512*$part))

		echo "Mount command needs root permission, you will be asked to enter root password."
		sudo mount -o offset=$offset -t ext4 $FILE $WORKING_PATH
		if [ $? -ne 0 ]
		then
			echo "Something went wrong when trying to mount"
			exit 1
		else
			echo "Successfully mounted `basename $FILE` to $WORKING_PATH"
		fi

		#Then we copy qemu executable to make translation for chroot
		if [ ! MOUNT_ONLY ]
		then
			if [ -e "/usr/bin/qemu-arm-static" ]
			then
				echo "Adding qemu-arm-static to /usr/bin of the image ..."
				cd $WORKING_PATH
				cp --remove-destination /usr/bin/qemu-arm-static ./usr/bin/
			else
				echo "Couldn't find /usr/bin/qemu-arm-static which is required to translate from ARM to `uname -m`"
				echo "Please install qemu-user and qemu-user-static (maybe search for qemu-extra depending on your linux distribution)"
				echo "Use 'apt-get install qemu-user qemu-user-static'"
				exit 1
			fi
		fi

		
	else
		echo "Please give a valid file ... (img/iso)"
		usage
		exit 1
	fi
}

unmount_image(){
	if [ -d $WORKING_PATH ]
	then
		echo "Unmounting `realpath $WORKING_PATH` filesystem ..."
		echo "Umount command needs root permission, you will be asked to enter root password."
		sudo umount $WORKING_PATH
		if [ $? -ne 0 ]
		then
			echo "A problem occured when trying to unmount $WORKING_PATH"
			exit 1
		else
			echo "Successfully unmounted $WORKING_PATH"
			echo "Removing temporary file ..."
			rm .wpath
			rm -r $WORKING_PATH
		fi
	else
		echo "Unmount target must be a directory"
		usage
		exit 1
	fi
}

chroot_image(){
	cd $WORKING_PATH
}

#If no parameter given, wtf ?
if [ -z "$1" ]; then
	usage
	exit 1
fi

for i in "$@"
do
	case $i in
		-m=*|--mountfile=*)
			FILE="${i#*=}"
			if [ ! -e "$FILE" ]
			then
				echo "Please give an existing file"
				usage
				exit 1
			fi
			MOUNTOPT=1
		shift # past argument=value
		;;
		-mo|--mount-only)
			MOUNT_ONLY=true
		;;
		-u=*|--umount=*|-u|--umount)
			FILE="${i#*=}"
			if [ "$FILE" == "$i" ]
			then
				if [ -f ".wpath" ]
				then
					FILE=`cat .wpath`
				else
					echo "umount option needs a directory path or at least needs to be used after having mounted a filesystem"
					exit 1
				fi
			else 
				if [ ! -e "$FILE" ]
				then
					echo "Please give an existing file"
					usage
					exit 1
				fi
			fi
			WORKING_PATH=$FILE
			MOUNTOPT=2
		shift # past argument=value
		;;
		-o=*|--output=*)
			DESTINATION="${i#*=}"
		shift # past argument=value
		;;
		-h|--help)
			usage
			exit 0
		shift # past argument=value
		;;
		*)
			echo "Unkown option : $i" # unknown option
		;;
	esac
done

#If no file is given

#Mount the filesystem
if [ $MOUNTOPT -le 1 ]
then
	mount_image
fi

if [ $MOUNTOPT -eq 2 ]
then
	unmount_image
fi