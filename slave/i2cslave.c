#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <curses.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#define TX_BUF_SIZE   4

#define DEFAULT_DEVICE "/dev/i2c_slave"

enum sys_call {
	TEST,
	CPU, 
	SHUTDOWN,
	RESTART,
	GET_IP, 
	GET_I2C, 
	IS_NETWORK
};

int test_communication(){
	char cmd[64];
	sprintf(cmd, "echo toto");
	system(cmd);
}

int shutdown_slave(){
	char cmd[64];
	//printf("shutdown");
	sprintf(cmd, "shutdown -h now");
	system(cmd);
	//printf("\n");
}

int restart_slave(){
	char cmd[64];
	sprintf(cmd, "shutdown -r");
	system(cmd);
}


char * action(enum sys_call call){
	
	
	int i = 0; 

	switch(call){
		case TEST:
			return "abcd";
			break;
		case CPU:
			//get_cpu();
			break;
		case SHUTDOWN:
			shutdown_slave();
			return "abcd";
			break;
		case RESTART:
			restart_slave();
			return "abcd";
			break;
		case GET_IP:
			//get_ip();
			return "abcd";
			break;
		case GET_I2C:
			//get_i2c();
			return "abcd";
			break;
		case IS_NETWORK:
			//is_network();
			return "abcd";
			break;
	}

	return "abcd";
}

int main(int argc, char **argv)
{
	char tx_buffer[TX_BUF_SIZE];
	char tx_answer[TX_BUF_SIZE];
	int fd;
	uint8_t data;
	int length;
	int i;		

	int opt;
	int mode = 0; 
	
	enum sys_call commande;

	FILE *usage_file = stderr;
	const char *input = DEFAULT_DEVICE;

	while ((opt = getopt(argc, argv, "hxc")) != -1) {
		switch (opt) {
		case 'd':
			mode = 0; // TODO defines
			break;
		case 'c':
			mode = 1; // TODO defines
			break;
		case 'x':
			mode = 2; // TODO defines
			break;
		case 'h':
			usage_file = stdout;
		default: /* '?' */
			fprintf(usage_file, "Usage: %s [-a addr] [-x|-c|-d] [device]\n", argv[0]);
			fprintf(usage_file, "defaults device: %s addr: %x\n", DEFAULT_DEVICE, 0x42);
			exit(usage_file == stdout ? EXIT_SUCCESS : EXIT_FAILURE);
		}
	}

	if (optind < argc) {
		input = argv[optind];
	}

	if ((fd = open(input, O_RDWR)) == -1) {
		perror("open i2c device");
		exit(EXIT_FAILURE);
	}

	while (1) {	
			length = read(fd, tx_buffer, 4);
			for(i = 0; i < length; i++)
			{
				switch (mode) {
				case 1:
					printf("1: Data received : %c\n", tx_buffer[i]);
					tx_answer = action(tx_buffer[i]);
					write(fd, tx_answer, 4);
					break;
				case 2:
					printf("2 :Data received : %02x\n ", tx_buffer[i]);
					tx_answer = action(tx_buffer[i]);
					write(fd, tx_answer, 4);
					break;
				default:
					printf("3 :Data received : %d \n", tx_buffer[i]);
					tx_answer = action(tx_buffer[i]);
					write(fd, tx_answer, 4);
					break;
				}
			}
			tx_buffer[0] = 0; 
			tx_buffer[1] = 1;
			tx_buffer[2] = 2; 
			tx_buffer[3] = 4;
			write(fd, tx_buffer, 4);
	}

	close(fd);
	return 0;
}
