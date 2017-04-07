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
DAEMON=/usr/local/bin/daemon_udp
IP_ADDR=`cat /etc/daemon.d/ip_addr.conf`

do_start() {
	logger "[ Info ] Starting UDP daemon ..."
	nohup $DAEMON $IP_ADDR
  if [ $? -ne 0 ]; then
    logger "[ Warning ] An error occured when trying to start  UDP daemon"
  else
	  logger "[ Info ] UDP daemon started !"
  fi
}

do_stop() {
	logger "[ Info ] Stopping UDP daemon"
	pkill $DAEMON
	logger "[ Info ] UDP daemon stopped !"
}

do_restart() {
  logger "[ Info ] Going to restart UDP daemon ..."
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
