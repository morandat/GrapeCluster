#!/bin/bash


command_exists(){
    type "$1" &> /dev/null ;
}

launch_bash(){
    echo "Launching bash emulator inside image"
    echo "Just use exit command to quit bash and chroot"
    cd /
    bash
}

check_and_install_package(){
    echo -n "Checking $1 is installed ..."
    if ! command_exists $2;
    then
        echo " Not installed ! Installing ..."
        `$PACKAGEMANAGER $3`
    fi
    echo ""
}

#we just modify the PATH ...
echo "======> Raspbian Preparator"
echo "Modifying path ..."
PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:$PATH
FILES=""
EXEC_NAME=mockdaemon
CHROOT_ONLY=false
INSTALL_ONLY=false
PACKAGEMANAGER="apt-get install -qq -o=Dpkg::Use-Pty=0"
UPGRADE_CLEAN=false

echo "Moving to /tmp/armmanager"
cd /tmp/armmanager

for i in "$@"
do
	case $i in
		--chroot-only)
			CHROOT_ONLY=true
		;;
        --install-only)
            INSTALL_ONLY=true
        ;;
        -uc|--upgrade-clean)
            UPGRADE_CLEAN=true
        ;;
        -f=*|--files=*)
			FILES="${i#*=}"
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

if [ $CHROOT_ONLY == true ] && [ $INSTALL_ONLY == true ]
then
    echo "So you want to chroot only, but to install only ... What shall I do ?"
    echo "It's too complicated for me."
    echo "My answer is 42."
    exit 42
fi

if [ $CHROOT_ONLY == false ]
then
    echo "======> Installing dependencies ..."
    echo "==> Updating package list"
    #apt-get update

    check_and_install_package gcc gcc gcc
    check_and_install_package make make build-essential


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

        #Not sure if python really needs to be installed
        #on slaves
        echo "Checking python is installed ... "
        if ! command_exists python;
        then
            echo -n "Installing python "
            `$PACKAGEMANAGER python python-pip python-dev`
        fi

        echo "Checking pip is installed ... "
        if ! command_exists pip;
        then
            echo -n "Installing pip ..."
            `$PACKAGEMANAGER python-pip`
            echo "Upgrading pip ..."
            pip install --upgrade pip
        fi
        ##########################


        if [ $UPGRADE_CLEAN ]
        then
            echo "Upgrading system ..."
            apt-get upgrade
            echo "Cleaning eventual installation dependencies ..."
            apt-get autoremove
        fi

        if [ $INSTALL_ONLY == false ]
        then
            launch_bash
        fi

    else
        echo "No files provided, nothing to do ..."
        echo "Exitting ..."
        exit 1
    fi
else
    launch_bash
fi

