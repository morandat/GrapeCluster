from Daemon import Daemon
import sys

if sys.argv[1]:
    ip = sys.argv[1]

else:
    ip = "127.0.0.2"

daemon = Daemon(ip)
daemon.start()