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
	logger "[ Info ] Starting daemon ..."
	nohup $DAEMON
  if [ $? -ne 0 ]; then
    logger "[ Warning ] An error occured when trying to start daemon"
  else
	  logger "[ Info ] daemon started !"
  fi
}

do_stop() {
	logger "[ Info ] Stopping daemon"
	pkill $DAEMON
	logger "[ Info ] daemon stopped !"
}

do_restart() {
  logger "[ Info ] Going to restart daemon ..."
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
