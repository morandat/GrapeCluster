#!/bin/bash

#Usage function to print how to use
usage() {
	echo "armmanager [-m=<file>] [-u[=<destination>]] [-o=<destination>]"
}

realpath() {
    [[ $1 = /* ]] && echo "$1" || echo "$PWD/${1#./}"
}

rootasked(){
	if [ "$EUID" -ne 0 ]
	then
		echo "Command $1 is going to be runned in sudo mode."
	fi
}

command_exists(){
    type "$1" &> /dev/null ;
}

#Global Variables
FILE=""
DESTINATION=""
MOUNTOPT=0
WORKING_PATH=""
MOUNT_ONLY=false
CHROOT=false
CHROOTOPTIONS=""
POSSIBLEOPTIONS=("chroot-only" "install-only")
FINALOPTIONS=""

mount_image(){
	echo "======> Mounting the image"
	if [[ $FILE == *.img ]] || [[ $FILE == *.iso ]]
	then
		#If no destination given
		if [ "$DESTINATION" == "" ]
		then $FINALOPTIONS
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

		rootasked "Mount"
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
			copy_qemu
		fi
	else
		echo "Please give a valid file ... (img/iso)"
		usage
		exit 1
	fi
}

copy_qemu(){
	echo "======> Preparing Qemu executable"
	if [ -e "/usr/bin/qemu-arm-static" ]
	then
		echo "Adding qemu-arm-static to /usr/bin of the image ..."
		cd $WORKING_PATH
		rootasked "cp (in mounted filesystem)"
		sudo cp --remove-destination /usr/bin/qemu-arm-static $WORKING_PATH/usr/bin/
	else
		echo "Couldn't find /usr/bin/qemu-arm-static which is required to translate from ARM to `uname -m`"
		echo "Please install qemu, qemu-user and qemu-user-static (maybe search for qemu-extra depending on your linux distribution)"
		echo "Use 'apt-get install qemu qemu-user qemu-user-static'"
		exit 1
	fi
}

unmount_image(){
	echo "======> Unmounting image"
	if [ -d $WORKING_PATH ]
	then
		echo "Unmounting `realpath $WORKING_PATH` filesystem ..."
		rootasked "Umount"
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
	echo "Verifying given path ..."
	df -h | grep $WORKING_PATH &> /dev/null
	if [ $? -eq 0 ]
	then
		echo "======> Preparing and executing change of root"
		if [ -e "$WORKING_PATH/usr/bin/qemu-arm-static" ]
		then
			if command_exists update-binfmts;
			then
				echo "Enabling Qemu ARM translation ..."
				rootasked "binfmts"
				sudo update-binfmts --enable qemu-arm

				if [ $? -ne 0 ]
				then
					echo "There was a problem when trying to enable qemu ARM translation to your system."
					echo "Please refer to binfmts and try to correct this issue"
					exit 1
				else
					echo "Successfully enabled translation through Qemu"
					#Not activated yet for debug, really interesting ?
					#if [ ! -e $WORKING_PATH/tmp/rasparchitect.sh ]
					#then
						echo "Preparing files for chroot ..."
						mkdir -p $WORKING_PATH/tmp/armmanager
						sudo cp --remove-destination rasparchitect.sh $WORKING_PATH/tmp/armmanager
						#Providing daemon
						sudo cp --remove-destination daemon.tgz $WORKING_PATH/tmp/armmanager
						if [ $? -ne 0 ]
						then
							echo "Couldn't copy script for manipulation inside the image."
							echo "Please retry or copy it manually"
							exit 1
						fi
					#fi
					echo "Going to chroot into mounted raspberry pi filesystem ..."
					echo "Careful ! Now only works for Raspbian"
					rootasked "chroot"
					sudo chroot $WORKING_PATH /usr/bin/qemu-arm-static /bin/bash /tmp/armmanager/rasparchitect.sh -f=daemon.tgz $FINALOPTIONS
					echo "Finished chroot actions"
				fi
			else
				echo "Please install binfmt-support in order to enable qemu ARM translation"
				echo "Run 'apt-get install binfmt-support'"
			fi
		else
			echo "Coudln't find qemu-arm-static executable in image, trying to copy it ..."
			copy_qemu
			echo "Retrying to chroot ..."
			chroot_image
		fi
	else
		echo "Given path is not a mounted filesystem, please mount a filesystem before trying to chroot in ..."
		echo "Exitting ..."
		exit 1
	fi
}

verify_chroot_options(){
	if [ "$CHROOTOPTIONS" != "" ]
	then
		for element in "${CHROOTOPTIONS[@]}"
		do
			added=false
			for option in "${POSSIBLEOPTIONS[@]}"
			do
				if [ "$option" == "$element" ]
				then
					FINALOPTIONS="$FINALOPTIONS --$option"
					added=true
				fi
			done
			if [ $added == false ]
			then
				echo "Unknown chroot-in option : $element"
			fi
		done
	fi

	if [ $added == false ]
	then
		echo "Please verify your chroot-options"
		exit 1
	fi
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
		-c=*|--chroot=*|-c|--chroot)
			if [ $MOUNTOPT -ne 1 ]
			then
				FILE="${i#*=}"
				if [ "$FILE" == "$i" ]
				then
					if [ -f ".wpath" ]
					then
						FILE=`cat .wpath`
					else
						echo "chroot option needs a directory path or at least needs to be used after having mounted a filesystem"
						exit 1
					fi
				else 
					if [ ! -e "$FILE" ]
					then
						echo "Please give an existing directory"
						usage
						exit 1
					fi
				fi
				WORKING_PATH=$FILE
			fi
			CHROOT=true
		shift # past argument=value
		;;
		-co=*|--chroot-options=*)
			OPTIONS="${i#*=}"
			if [ "$OPTIONS" != "$i" ]
			then
				IFS=',' read -r -a CHROOTOPTIONS <<< "$OPTIONS"
				verify_chroot_options
			fi
		shift # past argument=value
		;;
		-*)
			echo "Unkown option : $i" # unknown option
		;;
		*)
			echo "Unkown argument : $i" # unknown argument
		;;
	esac
done

#If no file is given

#Mount the filesystem
if [ $MOUNTOPT -eq 1 ]
then
	mount_image
fi

if [ $CHROOT == true ]
then
	chroot_image
fi

if [ $MOUNTOPT -eq 2 ]
then
	unmount_image
fi

echo "All actions seemed to end correctly, thank you ...!"
exit 0