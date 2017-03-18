#!/bin/sh

mount /dev/mmcblk0p1 /mnt/mmcblk0p1

IMAGE_CRC32=image_crc32.txt
IMAGE=copy.img.gz
SERVER=192.168.1.35
QUICK_FIX=quick_fix.sh
QUICK_FIX_CRC32=quick_fix_crc32.sh
fifo_image=fifo_image

sleep 5
tftp -l /root/${IMAGE_CRC32} -r ${IMAGE_CRC32} -g ${SERVER} 
while [ "$?" -ne "0" ]; do
	sleep 1
	tftp -l /root/${IMAGE_CRC32} -r ${IMAGE_CRC32} -g ${SERVER} 
done

diff /mnt/mmcblk0p1/${IMAGE_CRC32} /root/${IMAGE_CRC32}
status=$?

if [ "$status" -ne "0" ]; then

    umount /mnt/mmcblk0p3
    mkfifo ${fifo_image}
    tftp -l ${fifo_image} -r ${IMAGE} -g ${SERVER} &
    gunzip -c ${fifo_image} > /dev/mmcblk0p3 
    cp /root/${IMAGE_CRC32} /mnt/mmcblk0p1/

fi

tftp -l /root/${QUICK_FIX_CRC32} -r ${QUICK_FIX_CRC32} -g ${SERVER}
diff /root/${QUICK_FIX_CRC32} /mnt/mmcblk0p1/${QUICK_FIX_CRC32}

if [ "$?" -ne "0" ]; then

    tftp -l /root/${QUICK_FIX} -r ${QUICK_FIX} -g ${SERVER}
    chmod +x /root/${QUICK_FIX}
    /root/${QUICK_FIX} && cp /root/${QUICK_FIX_CRC32} /mnt/mmcblk0p1/${QUICK_FIX_CRC32}
        
fi

reboot
