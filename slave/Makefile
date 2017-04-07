default: udp
	
udp:
	gcc -Wall daemon_udp.c commands.c udpslave.c -o daemon_udp

i2c:
	gcc -Wall daemon_i2c.c commands.c i2cslave.c -o daemon_i2c


clean:
	rm daemon_udp daemon_i2c
