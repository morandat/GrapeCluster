#!/bin/bash

#Usage function to print how to use
usage() {
	echo "armmanager [-m=<file>] [-u[=<destination>]] [-o=<destination>]"
}

realpath() {
	[[ $1 == /* ]] && echo "$1" || echo "$PWD/${1#./}"
}

rootasked() {
	if [ "$EUID" -ne 0 ]; then
		echo ">> Command $1 is going to be runned in sudo mode."
	fi
}

command_exists() {
	type "$1" &>/dev/null
}

main_action() {
	echo -e "\e[1;33m========> $1\e[21;0m "
}

second_action() {
	echo -e "\e[1;31m>>>\e[21;0m \e[1m$1\e[21;0m"
}

simple_action() {
	echo "> $1"
}

#Global Variables
FILE=""
DESTINATION=""
MOUNTOPT=0
UMOUNTOPT=0
WORKING_PATH=""
MOUNT_ONLY=false
CHROOT=false
CHROOTOPTIONS=""
POSSIBLEOPTIONS=("chroot-only" "install-only" "upgrade-clean" "no-update" "no-daemon" "no-kernel-recompile")
FINALOPTIONS=""
RESIZE_VALUE=-1

resize_image() {
	main_action "Resizing the image"

	if [[ $FILE == *.img ]] || [[ $FILE == *.iso ]]; then

		df -h | grep $FILE &>/dev/null
		if [ $? -eq 0 ]; then
			simple_action "The image is already mounted, unmounting it ..."
			rootasked "Umount"
			sudo umount $FILE
		fi

		simple_action "Getting the partition's offset, and resizing it ..."
		part=$(fdisk -l -o Start $FILE | cut -d' ' -f1,3 | tail -n1)
		offset=$((512 * $part))

		truncate -s +1G $FILE
		rootasked "Losetup"
		sudo losetup /dev/loop0 $FILE

		if [ $? -ne 0 ]; then
			second_action "loopback device 0 is required to resize and mount on loop raspbian system"
			exit 1
		fi

		simple_action "Re-creating partition in order to resize it ..."
		rootasked "fdisk"
		echo -e "d\n2\nn\np\n2\n$part\n\nw" | sudo fdisk /dev/loop0
		sudo losetup -d /dev/loop0

		sudo losetup -o $offset /dev/loop0 $FILE
		simple_action "Verifying filesystem with e2fsck ..."
		sudo e2fsck -f -y /dev/loop0
		simple_action "Resizing filesystem with resize2fs ..."
		sudo resize2fs /dev/loop0

		sudo losetup -d /dev/loop0
		simple_action "Done !"

	else
		simple_action "Please give a valid file ... (img/iso)"
		usage
		exit 1
	fi
}

mount_image() {
	main_action "Mounting the image"
	if [[ $FILE == *.img ]] || [[ $FILE == *.iso ]]; then

		#If no destination given
		if [ "$DESTINATION" == "" ]; then
			$FINALOPTIONS
			DESTINATION="./.armmanager"
			filename=$(basename $FILE)
			WORKING_PATH="$DESTINATION/${filename%.*}"
			second_action "Setting destination folder to default $WORKING_PATH"
		else
			WORKING_PATH=$DESTINATION
			second_action "Destination folder is : $WORKING_PATH"
		fi

		df -h | grep $WORKING_PATH &>/dev/null
		if [ $? -eq 0 ]; then
			simple_action "The image you gave has already been mounted to $WORKING_PATH, please"
			simple_action "give another destination, or use another image."
			simple_action "Exitting ..."
			exit 1
		fi

		#Create directory for destination
		mkdir -p $WORKING_PATH

		#to save working path
		#we could have used readlink or realpath, but this is special to "recent" linux, so for instance not for mac
		echo -n $(realpath $WORKING_PATH) >.wpath

		#Seeking part to mount
		simple_action "Mounting Linux partition of $FILE"
		#part_root=`fdisk -l -o Start $FILE | cut -d' ' -f1,3 | tail -n1`
		#part_boot=`fdisk -l -o Start $FILE | cut -d' ' -f1,3 | tail -n2 | head -n1`
		#offset_root=$((512*$part_root))
		#offset_boot=$((512*$part_boot))
		rootasked "Losetup"
		FREE_LOOP=$(sudo losetup -f)
		if [ $? -ne 0 ]; then
			second_action "No free loop device could be found, cannot set loopback device."
			exit 1
		fi

		sudo losetup -P $FREE_LOOP $FILE

		rootasked "Mount"
		#sudo mount -o offset=$offset_root -t ext4 $FILE $WORKING_PATH
		sudo mount "$FREE_LOOP"p2 $WORKING_PATH

		if [ $? -ne 0 ]; then
			simple_action "Something went wrong when trying to mount root"
			exit 1
		else
			#sudo mkdir -p $WORKING_PATH/boot
			#sudo mount -o offset=$offset_boot -t vfat $FILE $WORKING_PATH/boot
			sudo mount "$FREE_LOOP"p1 $WORKING_PATH/boot

			if [ $? -ne 0 ]; then
				simple_action "Something went wrong when trying to mount boot"
				exit 1
			else
				simple_action "Successfully mounted $(basename $FILE) to $WORKING_PATH"
			fi
		fi

		#Then we copy qemu executable to make translation for chroot
		if [ ! MOUNT_ONLY ]; then
			copy_qemu
		fi
	else
		simple_action "Please give a valid file ... (img/iso)"
		usage
		exit 1
	fi
}

copy_qemu() {
	main_action "Preparing Qemu executable"
	if [ -e "/usr/bin/qemu-arm-static" ]; then
		second_action "Adding qemu-arm-static to /usr/bin of the image ..."
		rootasked "cp (in mounted filesystem)"
		sudo cp --remove-destination /usr/bin/qemu-arm-static $WORKING_PATH/usr/bin/
	else
		second_action "Couldn't find /usr/bin/qemu-arm-static which is required to translate from ARM to $(uname -m)"
		simple_action "Please install qemu, qemu-user and qemu-user-static (maybe search for qemu-extra depending on your linux distribution)"
		simple_action "Use 'apt-get install qemu qemu-user qemu-user-static [qemu-arm-static]'"
		exit 1
	fi
}

unmount_image() {
	main_action "Unmounting the image"
	if [ -d $WORKING_PATH ]; then
		second_action "Unmounting $(realpath $WORKING_PATH) filesystem ..."
		rootasked "Umount"
		sudo umount $WORKING_PATH/boot
		sudo umount $WORKING_PATH
		if [ $? -ne 0 ]; then
			second_action "A problem occured when trying to unmount $WORKING_PATH"
			exit 1
		else

			sudo losetup -d /dev/loop0

			if [ $? -ne 0 ]; then
				second_action "A problem occured when unload loopback device"
				exit 1
			else
				simple_action "Successfully unmounted $WORKING_PATH"
				simple_action "Removing temporary file ..."
				rm .wpath
				rm -r $WORKING_PATH
			fi
		fi
	else
		simple_action "Unmount target must be a directory"
		usage
		exit 1
	fi
}

chroot_image() {
	second_action "Verifying given path for chroot ..."
	df -h | grep $WORKING_PATH &>/dev/null
	if [ $? -eq 0 ]; then
		main_action "Preparing and executing change of root"
		if [ -e "$WORKING_PATH/usr/bin/qemu-arm-static" ]; then
			if command_exists update-binfmts; then
				simple_action "Enabling Qemu ARM translation ..."
				rootasked "binfmts"
				sudo update-binfmts --enable qemu-arm

				if [ $? -ne 0 ]; then
					second_action "There was a problem when trying to enable qemu ARM translation to your system."
					simple_action "Please refer to binfmts and try to correct this issue"
					exit 1
				else
					simple_action "Successfully enabled translation through Qemu"
					simple_action "Preparing files for chroot ..."
					mkdir -p $WORKING_PATH/home/pi/armmanager
					sudo cp --remove-destination rasparchitect.sh $WORKING_PATH/home/pi/armmanager
					if [ ! -e ../slave ]; then
						second_action "Impossible to copy sources of the slave daemon"
					else
						sudo cp --remove-destination ../slave/ $WORKING_PATH/home/pi/armmanager/slave
					fi

					simple_action "Going to chroot into mounted raspberry pi filesystem ..."
					second_action "Careful ! For now only works for Raspbian"
					rootasked "chroot"
					sudo chroot $WORKING_PATH /usr/bin/qemu-arm-static /bin/bash /home/pi/armmanager/rasparchitect.sh $FINALOPTIONS
					simple_action "Finished chroot actions"
				fi
			else
				simple_action "Please install binfmt-support in order to enable qemu ARM translation"
				simple_action "Run 'apt-get install binfmt-support'"
			fi
		else
			second_action "Coudln't find qemu-arm-static executable in image, trying to copy it ..."
			copy_qemu
			simple_action "Retrying to chroot ..."
			chroot_image
		fi
	else
		second_action "Given path is not a mounted filesystem, please mount a filesystem before trying to chroot in ..."
		simple_action "Exitting ..."
		exit 1
	fi
}

verify_chroot_options() {
	if [ "$CHROOTOPTIONS" != "" ]; then
		for element in "${CHROOTOPTIONS[@]}"; do
			added=false
			for option in "${POSSIBLEOPTIONS[@]}"; do
				if [ "$option" == "$element" ]; then
					FINALOPTIONS="$FINALOPTIONS --$option"
					added=true
				fi
			done
			if [ $added == false ]; then
				simple_action "Unknown chroot-in option : $element"
			fi
		done
	fi

	if [ $added == false ]; then
		second_action "Please verify your chroot-options"
		exit 1
	fi
}

#If no parameter given, wtf ?
if [ -z "$1" ]; then
	usage
	exit 1
fi

for i in "$@"; do
	case $i in
		-m | --mountfile)
			MOUNTOPT=1
			shift # past argument=value
			;;
		-mo | --mount-only)
			MOUNT_ONLY=true
			;;
		-u=* | --umount=* | -u | --umount)
			if [ $MOUNTOPT -ne 1 ]; then
				DIRECTORY="${i#*=}"
				if [ "$DIRECTORY" == "$i" ]; then
					if [ -f ".wpath" ]; then
						DIRECTORY=$(cat .wpath)
					else
						second_action "umount option needs a directory path or at least needs to be used after having mounted a filesystem"
						exit 1
					fi
				else
					if [ ! -e "$DIRECTORY" ]; then
						second_action "Please give an existing file"
						usage
						exit 1
					fi
				fi
				WORKING_PATH=$DIRECTORY
			fi
			UMOUNTOPT=1
			shift # past argument=value
			;;
		-o=* | --output=*)
			DESTINATION="${i#*=}"
			shift # past argument=value
			;;
		--pidora)
			FINALOPTIONS="--yum $FINALOPTIONS"
			;;
		-h | --help)
			usage
			exit 0
			shift # past argument=value
			;;
		-c=* | --chroot=* | -c | --chroot)
			if [ $MOUNTOPT -ne 1 ]; then
				FILE="${i#*=}"
				if [ "$FILE" == "$i" ]; then
					if [ -f ".wpath" ]; then
						FILE=$(cat .wpath)
					else
						second_action "chroot option needs a directory path or at least needs to be used after having mounted a filesystem"
						exit 1
					fi
				else
					if [ ! -e "$FILE" ]; then
						second_action "Please give an existing directory"
						usage
						exit 1
					fi
				fi
				WORKING_PATH=$FILE
			fi
			CHROOT=true
			shift # past argument=value
			;;
		-co=* | --chroot-options=*)
			OPTIONS="${i#*=}"
			if [ "$OPTIONS" != "$i" ]; then
				IFS=',' read -r -a CHROOTOPTIONS <<<"$OPTIONS"
				verify_chroot_options
			fi
			shift # past argument=value
			;;
		-r | -r=* | --resize-image | --resize-image=*)
			VALUE="${i#*=}"
			if [ "$VALUE" == "$i" ]; then
				RESIZE_VALUE=1G
			else
				RESIZE_VALUE=$VALUE
			fi
			shift
			;;
		-*)
			simple_action "Unkown option : $i" # unknown option
			;;
		*)
			if [ "$FILE" == "" ]; then
				FILE="${i}"
				if [ ! -e "$FILE" ]; then
					second_action "Please give an existing file"
					usage
					exit 1
				fi
			else
				simple_action "Unkown argument : $i" # unknown argument
			fi
			;;
	esac
done

#resize the image
if [ "$RESIZE_VALUE" != "-1" ]; then
	resize_image
fi

#Mount the filesystem
if [ $MOUNTOPT -eq 1 ]; then
	mount_image
fi

if [ $CHROOT == true ]; then
	chroot_image
fi

if [ $UMOUNTOPT -eq 1 ]; then
	unmount_image
fi

exit 0
