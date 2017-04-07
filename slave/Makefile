default: udp
	
udp:
	gcc $(CFLAGS) -Wall daemon_udp.c commands.c udpslave.c -o daemon_udp -std=c99

i2c:
	gcc $(CFLAGS) -Wall daemon_i2c.c commands.c i2cslave.c -o daemon_i2c -std=c99


clean:
	rm daemon_udp daemon_i2c
