from Daemon import Daemon
import sys


if len(sys.argv) >= 2:
    ip = sys.argv[1]
else:
    ip = "127.0.0.2"

daemon = Daemon(ip)
daemon.start()