#!/bin/bash

CLASS_ADDRESSES=(0x50 0x58 0x60 0x68 0x70 0x78)

usage(){
    "bcm_slave_mod_install [stack address] [pi position]"
}

find_address(){
    address=`printf '0x%x' $(($1 + ${CLASS_ADDRESSES[$2-1]}))`
    echo "Address of the pi is : $address"
}

echo "This script is going to compile and install bcm2835_slave_mod"
if [ -z $1 ]; then
    echo "You gave no arg, which means you are going to deal with default i2c address"
else
    find_address $1 $2
    echo $address > /etc/modprobe.d/i2c.conf
    echo $2 > /home/pi/place.txt
fi

echo "Moving to raspberry_slave_i2c repo ..."
cd /home/pi/armmanager/raspberry_slave_i2c

if [ -z $1 ]; then
    make CFLAGS=-DSLV_ADDRESS=$address all
else
    make all
fi

echo "Applying overlay to /boot/config.txt ..."
echo "dtoverlay=i2cslave-bcm2708" >> /boot/config.txt

if [ $? -ne 0 ]; then
    echo "Something went wrong when using make"
    echo "Try to install it on your own ?"
fi

echo "End of compilation"
echo "Exitting ..."
exit 0
