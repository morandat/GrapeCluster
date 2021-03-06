#!/bin/bash

usage() {
	echo "Usage : rasparchitect [OPTIONS] ..."
	echo "Modify the filesystem of a Raspberry pi from the inside."
	echo ""
	echo "Available options are :"
	echo "	--chroot-only		Executes only a chroot. Useless if you try to execute this script from the inside of the Raspberry."
	echo ""
	echo "	--install-only		Will only execute actions to install. No Bash will be launched."
	echo ""
	echo "	-uc, --upgrade-clean"
	echo "				At the end of installation, will execute a system upgrade and clean of packages."
	echo ""
	echo "	-nu, --no-update	No package repository will be updated."
	echo ""
	echo "	-nd, --no-daemon	No daemon will be compiled (and set as service) using this option."
	echo ""
	echo "	-nk, --no-kernel-recompile"
	echo "				The rasbperry pi kernel will not be recompiled if you use this fonction."
	echo ""
	echo "	-h, --help			Print this menu and exit"
	echo ""
	echo "Do not hesitate to report bugs to <paul.breton@enseirb-matmeca.fr>"
	echo "Full project available on GitHub at <https://github.com/Pravez/GrapeCluster>"
	echo "Thank you for using this script !"
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

launch_bash() {
	second_action "Launching bash emulator inside image"
	simple_action "Just use exit command to quit bash and chroot"
	cd /
	bash
}

check_and_install_package() {
	echo -ne "Checking \e[1m$1\e[0m is installed ..."
	if ! command_exists $2; then
		echo -ne " Not installed ! Installing ..."
		$($PACKAGEMANAGER $3)
	fi
	echo ""
}

check_and_install() {
	echo -ne "Checking package \e[1m$1\e[0m is installed ..."
	dpkg -s $1 &>/dev/null
	if [ $? == 1 ]; then
		echo -ne " Not installed ! Installing ..."
		$($PACKAGEMANAGER $1)
	fi
	echo ""
}

install_as_service(){
	if [ -e $1 ]; then
		simple_action "Copying executable and launching script for $1 ..."
		cp ./$1 /usr/local/bin/$1
		chmod a+x ./$1.sh
		cp ./$1.sh /etc/init.d/$1
		simple_action "Adding program to init.d services ..."
		update-rc.d $1 defaults >/home/pi/armmanager.log
		simple_action "Enabling $1 ..."
		update-rc.d $1 enable >>/home/pi/armmanager.log

	else
		second_action "Impossible to proceed service creator, executable does not exist."
	fi
}

#we just modify the PATH ...
main_action "In-chroot script ISO conjurer"
second_action "Modifying path ..."
PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:$PATH
FILES=""
EXEC_NAME_UDP=daemon_udp
EXEC_NAME_I2C=daemon_i2c
CHROOT_ONLY=false
INSTALL_ONLY=false
PACKAGEMANAGER="apt-get install -qq -o=Dpkg::Use-Pty=0"
UPGRADE_CLEAN=false
NO_UPDATE=false
DAEMON=true
RECOMPILE_KERNEL=true

second_action "Updating locale ..."
export LANGUAGE=en_GB.utf8
export LANG=en_GB.utf8
export LC_ALL=en_GB.utf8

simple_action "Moving to /home/pi/armmanager"
cd /home/pi/armmanager

simple_action "chmoding scripts for future use ..."
sudo chmod a+x bcm_slave_mod_install.sh
sudo chmod a+x rasparchitect.sh

for i in "$@"; do
	case $i in
		--chroot-only)
			CHROOT_ONLY=true
			;;
		--install-only)
			INSTALL_ONLY=true
			;;
		-uc | --upgrade-clean)
			UPGRADE_CLEAN=true
			;;
		-nu | --no-update)
			NO_UPDATE=true
			;;
		-y | --yum)
			PACKAGEMANAGER="yum install"
			;;
		-nd | --no-daemon)
			DAEMON=false
			;;
		-nrk | --no-kernel-recompile)
			RECOMPILE_KERNEL=false
			;;
		-h | --help)
			usage
			;;
		*)
			simple_action "Unkown option : $i" # unknown option
			;;
	esac
done

if [ $CHROOT_ONLY == true ] && [ $INSTALL_ONLY == true ]; then
	simple_action "So you want to chroot only, but to install only ... What shall I do ?"
	simple_action "It's too complicated for me."
	simple_action "My answer is 42."
	exit 42
fi

if [ $CHROOT_ONLY == false ]; then
	main_action "Verifying and Installing dependencies ..."
	if [ ! $NO_UPDATE ]; then
		second_action "Updating package list"
		apt-get update
	fi

	check_and_install_package gcc gcc gcc
	check_and_install_package make make build-essential
	check_and_install_package git git git
	check_and_install_package bc bc bc
	check_and_install libncurses5-dev
	check_and_install sysstat
	#not needed
	#check_and_install python
	#check_and_install python-pip
	#check_and_install python-dev

	if [ $DAEMON == true ]; then
		main_action "Installing daemon"

		if [ $RECOMPILE_KERNEL == true ]; then

			second_action "Recompiling Kernel"
			simple_action "Cloning from https://github.com/raspberrypi/linux ..."

			if [ -e "./linux" ]; then
				cd linux
				git pull
			else
				git clone --depth=1 https://github.com/raspberrypi/linux
				cd linux
			fi

			KERNEL=kernel7
			simple_action "Making bcm2709_defconfig ..."
			make bcm2709_defconfig

			simple_action "Making zImage modules and dtbs ..."
			make -j4 zImage modules dtbs
			simple_action "Making modules_install ..."
			make modules_install

			#Testing with only overlays
			#simple_action "Going to make dtbs, and overlays."
			#make -j4 zImage dtbs
			#make arch/arm/boot/dts

			simple_action "Copying new files to /boot ..."
			cp arch/arm/boot/dts/*.dtb /boot/
			cp arch/arm/boot/dts/overlays/*.dtb* /boot/overlays/
			cp arch/arm/boot/dts/overlays/README /boot/overlays/
			scripts/mkknlimg arch/arm/boot/zImage /boot/$KERNEL.img
			cd ../
		fi

		simple_action "Moving to home directory"

		second_action "Building modules and daemon"
		if [ ! -e raspberry_slave_i2c ]; then
			simple_action "Cloning raspberry_slave_i2c"
			git clone https://github.com/marilafo/raspberry_slave_i2c.git
		fi
		cd raspberry_slave_i2c

		simple_action "Device tree compiler ..."
		dtc -@ -I dts -O dtb i2cslave-bcm2708-overlay.dts -o i2cslave-bcm2708.dtbo
		simple_action "Copying to overlays ..."
		cp i2cslave-bcm2708.dtbo /boot/overlays/

		cd ../
		second_action "Starting daemon compilation ..."
		if [ -e slave ]; then
			cd slave
			simple_action "Building daemon ..."
			make udp
			make i2c
		else
			second_action "Unable to find Daemon sources"
		fi

		install_as_service $EXEC_NAME_I2C
		install_as_service $EXEC_NAME_UDP
		
	fi

	if [ $UPGRADE_CLEAN == true ]; then
		second_action "Upgrading system ..."
		apt-get upgrade
		second_action "Cleaning eventual installation dependencies ..."
		apt-get autoremove
	fi

	if [ $INSTALL_ONLY == false ]; then
		launch_bash
	fi

else
	launch_bash
fi
