#!/bin/bash

CLASS_ADDRESSES=(0x50 0x58 0x60 0x68 0x70 0x78)

usage(){
    "Usage : bcm_slave_mod_install [stack address] [pi position]"
}

find_address(){
    address=`printf '0x%x' $(($1 + ${CLASS_ADDRESSES[$2-1]}))`
    echo "Address of the pi is : $address"
}

if [ $# -eq 0 ]; then
    usage
    exit 1
fi

if [ "$EUID" -ne 0 ]
    then echo "Please run as root"
    exit 1
fi

echo "This script is going to compile and install bcm2835_slave_mod"
if [ -z $1 ]; then
    echo "You gave no arg, which means you are going to deal with default i2c address"
else
    if [ -z $2 ]; then
        echo "You have to give stack address AND pi position"
        echo "Exitting ..."
        exit 1
    fi
    find_address $1 $2
    echo $address > /etc/modprobe.d/i2c.conf
    echo $2 > /home/pi/place.txt
    echo "bcm2835_slave_mod slave_add=$address" >> /etc/modules
fi

echo "Moving to raspberry_slave_i2c repo ..."
cd /home/pi/armmanager/raspberry_slave_i2c

echo "Compiling module ..."
if [ -z $1 ]; then
    make CFLAGS=-DSLV_ADDRESS=$address all
else
    make all
fi

echo "Moving module to kernel modules ..."
sudo cp bcm2835_slave_mod.ko /lib/modules/$(uname -r)/kernel/drivers/
echo "Updating modules dependencies ..."
sudo depmod -a

echo "Applying overlay to /boot/config.txt ..."
echo "dtoverlay=i2cslave-bcm2708" >> /boot/config.txt

echo "Install kernel modules ? (y/n)"
read answer
if [ "$answer" == "y" ]; then
    cd /home/pi/armmanager
    KERNEL=kernel7
    simple_action "Making modules_install ..."
    make modules_install

    simple_action "Copying new files to /boot ..."
    sudo cp arch/arm/boot/dts/*.dtb /boot/
    sudo cp arch/arm/boot/dts/overlays/*.dtb* /boot/overlays/
    sudo cp arch/arm/boot/dts/overlays/README /boot/overlays/
    sudo cp arch/arm/boot/zImage /boot/$KERNEL.img
    cd ../
fi

if [ $? -ne 0 ]; then
    echo "Something went wrong when using make"
    echo "Try to install it on your own ?"
fi

echo "End of compilation"
echo "Exitting ..."
exit 0
