#!/bin/bash


command_exists(){
    type "$1" &> /dev/null ;
}

main_action(){
	echo -e "\e[1;33m========> $1\e[21;0m "
}

second_action(){
	echo -e "\e[1;31m>>>\e[21;0m \e[1m$1\e[21;0m"
}

simple_action(){
	echo "> $1"
}

launch_bash(){
    second_action "Launching bash emulator inside image"
    simple_action "Just use exit command to quit bash and chroot"
    cd /
    bash
}

check_and_install_package(){
    echo -ne "Checking \e[1m$1\e[0m is installed ..."
    if ! command_exists $2;
    then
        echo " Not installed ! Installing ..."
        `$PACKAGEMANAGER $3`
    fi
    echo ""
}

#we just modify the PATH ...
main_action "In-chroot script ISO conjurer"
second_action "Modifying path ..."
PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:$PATH
FILES=""
EXEC_NAME=mockdaemon
CHROOT_ONLY=false
INSTALL_ONLY=false
PACKAGEMANAGER="apt-get install -qq -o=Dpkg::Use-Pty=0"
UPGRADE_CLEAN=false
NO_UPDATE=false

second_action "Updating locale ..."
export LANGUAGE="en"
export LANG=en_US.utf8

simple_action "Moving to /tmp/armmanager"
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
        -nu|--no-update)
            NO_UPDATE=true
        ;;
        -y|--yum)
            PACKAGEMANAGER="yum install"
        ;;
        -f=*|--files=*)
			FILES="${i#*=}"
			if [ ! -e "./$FILES" ]
			then
				second_action "Please give an existing targz archive"
				exit 1
			fi
		shift # past argument=value
		;;
		*)
			simple_action "Unkown option : $i" # unknown option
		;;
	esac
done

if [ $CHROOT_ONLY == true ] && [ $INSTALL_ONLY == true ]
then
    simple_action "So you want to chroot only, but to install only ... What shall I do ?"
    simple_action "It's too complicated for me."
    simple_action "My answer is 42."
    exit 42
fi

if [ $CHROOT_ONLY == false ]
then
    main_action "Verifying and Installing dependencies ..."
    if [ ! $NO_UPDATE ]
    then
        second_action "Updating package list"
        apt-get update
    fi


    check_and_install_package gcc gcc gcc
    check_and_install_package make make build-essential
    check_and_install_package git git git
    check_and_install_package bc bc bc


    #if [ -e "$FILES" ]
    #then
        second_action "Installing daemon"
        simple_action "Cloning from https://github.com/raspberrypi/linux ..."
        git clone --depth=1 https://github.com/raspberrypi/linux        
        
        if [ -e "./linux" ]
        then
            cd linux
            KERNEL=kernel7
            simple_action "Making bcm2709_defconfig ..."
            make bcm2709_defconfig

            simple_action "Making zImage modules and dtbs ..."
            make -j4 zImage modules dtbs
            simple_action "Making modules_install ..."
            sudo make modules_install
            simple_action "Copying new files to /boot ..."
            sudo cp arch/arm/boot/dts/*.dtb /boot/
            sudo cp arch/arm/boot/dts/overlays/*.dtb* /boot/overlays/
            sudo cp arch/arm/boot/dts/overlays/README /boot/overlays/
            sudo scripts/mkknlimg arch/arm/boot/zImage /boot/$KERNEL.img

            simple_action "Device tree compiler ..."
            dtc -@ -I dts -O dtb i2cslave-bcm2708-overlay.dts -o i2cslave-bcm2708.dtbo
            simple_action "Copying to overlays ..."
            sudo cp i2cslave-bcm2708.dtbo /boot/overlays/

            cd ../
            simple_action "Cloning raspberry_slave_i2c"
            git clone https://github.com/marilafo/raspberry_slave_i2c.git
            cd raspberry_slave_i2c
            simple_action "Building daemon ..."
            gcc -o daemon i2ccat.c
        else
            second_action "Something went wrong when trying to clone repository ..."
        fi

        #simple_action "Decompressing sources ..."
        #ffiles=`basename $FILES`
        #decomp_files=${ffiles%.*}
        #if [ -e $decomp_files ]; then rm -rf $decomp_files; fi
        #tar -xzf $FILES
        #if [ $? -eq 0 ]
        #then
        #    simple_action "Compiling sources ..."
        #    make -C ./$decomp_files all
        #    if [ $? -eq 0 ]
        #    then
        #        simple_action "Copying executable and launching script ..."
        #        cp ./$decomp_files/$EXEC_NAME /usr/local/bin/$EXEC_NAME
        #        chmod a+x ./$decomp_files/$EXEC_NAME.sh
        #        cp ./$decomp_files/$EXEC_NAME.sh /etc/init.d/$EXEC_NAME
        #        simple_action "Adding program to init.d services ..."
        #        update-rc.d $EXEC_NAME defaults > /home/pi/armmanager.log
        #        simple_action "Enabling daemon ..."
        #        update-rc.d $EXEC_NAME enable >> /home/pi/armmanager.log
        #    else
        #        second_action "A problem occured when trying to compile files"
        #    fi
        #else
        #    second_action "Impossible to extract files from targz file"
        #    exit 1
        #fi

        #Not sure if python really needs to be installed
        #on slaves
        #echo "Checking python is installed ... "
        #if ! command_exists python;
        #then
        #    echo -n "Installing python "
        #    `$PACKAGEMANAGER python python-pip python-dev`
        #fi
        #
        #echo "Checking pip is installed ... "
        #if ! command_exists pip;
        #then
        #    echo -n "Installing pip ..."
        #    `$PACKAGEMANAGER python-pip`
        #    echo "Upgrading pip ..."
        #    pip install --upgrade pip
        #fi
        ##########################


        if [ $UPGRADE_CLEAN == true ]
        then
            second_action "Upgrading system ..."
            apt-get upgrade
            second_action "Cleaning eventual installation dependencies ..."
            apt-get autoremove
        fi

        if [ $INSTALL_ONLY == false ]
        then
            launch_bash
        fi

    #else
    #    second_action "No files provided, nothing to do ..."
    #    simple_action "Exitting ..."
    #    exit 1
    #fi
else
    launch_bash
fi

