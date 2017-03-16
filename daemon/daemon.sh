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

do_start()
{
   initlog -c "echo [ Info ] Starting daemon ..."
   $DAEMON &
   initlog -c "echo [ Info ] daemon started !"
}

do_stop()
{
   initlog -c "echo [ Info ] Stopping daemon"
   pkill $DAEMON
   initlog -c "echo [ Info ] daemon stopped !"
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