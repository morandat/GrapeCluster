#!/bin/sh
### BEGIN INIT INFO
# Provides:          daemon_i2c
# Required-Start:    $remote_fs $syslog
# Required-Stop:     $remote_fs $syslog
# Default-Start:     2 3 4 5
# Default-Stop:      0 1 6
# X-Interactive:     false
# Short-Description: daemon i2c init script
# Description:       Start/stop and restart
### END INIT INFO

DESC="Daemon script"
NAME=daemon
DAEMON=/usr/local/bin/daemon_i2c

do_start() {
	logger "[ Info ] Starting I2C daemon ..."
	nohup $DAEMON
  if [ $? -ne 0 ]; then
    logger "[ Warning ] An error occured when trying to start I2C daemon"
  else
	  logger "[ Info ] I2C daemon started !"
  fi
}

do_stop() {
	logger "[ Info ] Stopping I2C daemon"
	pkill $DAEMON
	logger "[ Info ] I2C daemon stopped !"
}

do_restart() {
  logger "[ Info ] Going to restart I2C daemon ..."
  do_stop
  do_start
}

case "$1" in
	start)
		do_start
		;;
	stop)
		do_stop
		;;
  restart)
    do_restart
    ;;
esac

exit 0
