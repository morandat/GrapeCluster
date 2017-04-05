#!/bin/bash

usage(){
    "bcm_slave_mod_install [i2c address]"
}

echo "This script is going to compile and install bcm2835_slave_mod"
if [ ! -z $1 ]; then
    echo "You gave no arg, which means you are going to deal with default i2c address"
fi

echo "Moving to raspberry_slave_i2c repo ..."
cd /home/pi/armmanager/raspberry_slave_i2c

if [ -z $1 ]; then
    make CFLAGS=-DSLV_ADDRESS=$1 all
else
    make all
fi

if [ $? -ne 0 ]; then
    echo "Something went wrong when using make"
    echo "Try to install it on your own ?"
fi

echo "End of compilation"
echo "Exitting ..."
exit 0
