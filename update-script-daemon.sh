#!/bin/sh

mount /dev/mmcblk0p1 /mnt/mmcblk0p1

IMAGE_MDR5=imagemd5.txt
IMAGE=copy.img.gz
SERVER=192.168.1.35
fifo_image=fifo_image

sleep 5
tftp -l /root/${IMAGE_MDR5} -r ${IMAGE_MDR5} -g ${SERVER}
while [ "$?" -ne "0" ]; do
	sleep 1
	tftp -l /root/${IMAGE_MDR5} -r ${IMAGE_MDR5} -g ${SERVER}
done

diff /mnt/mmcblk0p1/${IMAGE_MDR5} /root/${IMAGE_MDR5}
status=$?

if [ "$status" -ne "0" ]; then

	umount /mnt/mmcblk0p3
	mkfifo ${fifo_image}
	tftp -l ${fifo_image} -r ${IMAGE} -g ${SERVER} &
	gunzip -c ${fifo_image} >/dev/mmcblk0p3
	cp /root/${IMAGE_MDR5} /mnt/mmcblk0p1/

fi

reboot
