#!/bin/sh
### BEGIN INIT INFO
# Provides:          mockdaemon
# Required-Start:    $remote_fs $syslog
# Required-Stop:     $remote_fs $syslog
# Default-Start:     2 3 4 5
# Default-Stop:      0 1 6
# X-Interactive:     false
# Short-Description: Example init script
# Description:       Start/stop an example script
### END INIT INFO

DESC="Daemon script"
NAME=daemon
DAEMON=/usr/local/bin/daemon

do_start() {
  logger "DAEMON !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
  if [ ! -e /home/pi/.daemon ]; then
    logger "[ Info ] Installing needed modules ..."
    cd /home/pi
    if [[ ( ! -e /home/pi/raspberry_slave_i2c ) && ( ping -q -c 1 -W 1 google.com >/dev/null ) ]]; then
		  git clone https://github.com/marilafo/raspberry_slave_i2c.git
      cd raspberry_slave_i2c
    fi
    if [ -e /home/pi/raspberry_slave_i2c ]; then
      cd raspberry_slave_i2c
      make
      rmmod i2c_dev
      rmmod i2c_bcm2835
      insmod bcm2835_slave_mod.ko
      modprobe -a bcm2835_slave_mod
      if [ $? -eq 0 ]; then
        touch /home/pi/.daemon
        logger "[ Info ] Modules installed !"
      else
        logger "[ Warning ] There was a problem when trying to install modules"
      fi
    fi
  fi
    
	logger "[ Info ] Starting daemon ..."
	nohup $DAEMON
	logger "[ Info ] daemon started !"
}

do_stop() {
	logger "[ Info ] Stopping daemon"
	pkill $DAEMON
	logger "[ Info ] daemon stopped !"
}

case "$1" in
	start)
		do_start
		;;
	stop)
		do_stop
		;;
esac

exit 0
