#!/bin/bash


command_exists(){
    type "$1" &> /dev/null ;
}

#we just modify the PATH ...
echo "==> Raspbian Preparator"
echo "Modifying path ..."
PATH=/bin:$PATH
ACTIVATED=false
FILES=""
EXEC_NAME=mockdaemon

echo "Moving to /tmp/armmanager"
cd /tmp/armmanager

for i in "$@"
do
	case $i in
		-a|--activated)
			ACTIVATED=true
		;;
        -f=*|--files=*)
			FILES="${i#*=}"
            echo $FILES
            ls
			if [ ! -e "./$FILES" ]
			then
				echo "Please give an existing targz archive"
				exit 1
			fi
		shift # past argument=value
		;;
		*)
			echo "Unkown option : $i" # unknown option
		;;
	esac
done

echo "Installing dependencies ..."
echo "==> Updating package list"
#apt-get update

echo "Checking gcc exists ..."
if ! command_exists gcc;
then
    echo "Installing gcc ..."
    #options, to limit output
    apt-get install -qq -o=Dpkg::Use-Pty=0 gcc
fi

if ! command_exists make;
then
    echo "Installing build-essentials (make)"
    apt-get install -qq -o=Dpkg::Use-Pty=0 build-essentials
fi

if [ -e "$FILES" ]
then
    echo "==> Installing daemon"
    echo "Decompressing sources ..."
    ffiles=`basename $FILES`
    decomp_files=${ffiles%.*}
    if [ -e $decomp_files ]; then rm -rf $decomp_files; fi
    tar -xzf $FILES
    if [ $? -eq 0 ]
    then
        echo "Compiling sources ..."
        make -C ./$decomp_files all
        if [ $? -eq 0 ]
        then
            echo "Copying executable and launching script ..."
            cp ./$decomp_files/$EXEC_NAME /usr/local/bin/$EXEC_NAME
            chmod a+x ./$decomp_files/$EXEC_NAME.sh
            cp ./$decomp_files/$EXEC_NAME.sh /etc/init.d/$EXEC_NAME
            echo "Adding program to init.d services ..."
            sudo update-rc.d $EXEC_NAME defaults > /home/pi/armmanager.log
            echo "Enabling daemon ..."
            sudo update-rc.d $EXEC_NAME enable >> /home/pi/armmanager.log
        else
            echo "A problem occured when trying to compile files"
        fi
    else
        echo "Impossible to extract files from targz file"
        exit 1
    fi
fi

if [ ACTIVATED ]
then
    echo "Launching bash emulator inside image"
    echo "Just use exit command to quit bash and chroot"
    bash
fi