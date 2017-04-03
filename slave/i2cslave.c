#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <curses.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "i2cslave.h"
#include "utils.h"

#define ENDSYMB		  240

enum sys_call {
	TEST,
	CPU, 
	SHUTDOWN,
	RESTART,
	GET_IP, 
	GET_I2C, 
	IS_NETWORK
};


char * action(enum sys_call call){
		
	int i = 0; 
	int test = 0;
	char *in[4];
	char out[4];

	switch(call){
		case TEST:
			test_communication();
			return "1111";
			break;
		case CPU:
			//get_cpu();
			break;
		case SHUTDOWN:
			shutdown_slave();
			break;
		case RESTART:
			restart_slave();
			break;
		case GET_IP:
			get_ip(in);
			encode_ip(out, in);
			return out;
			break;
		case GET_I2C:
			//get_i2c();
			return "abcd";
			break;
		case IS_NETWORK:
			test = test_network();
			if(test == 1)
				return "1111";
			else
				return "0000";
			break;
	}
}

int i2c_init(int* mode, int argc, char* argv[]) {
    int opt;
    *mode = 0;

    FILE *usage_file = stderr;
    const char *input = DEFAULT_DEVICE;

    while ((opt = getopt(argc, argv, "hxc")) != -1) {
        switch (opt) {
            case 'd':
                *mode = 0; // TODO defines
                break;
            case 'c':
                *mode = 1; // TODO defines
                break;
            case 'x':
                *mode = 2; // TODO defines
                break;
            case 'h':
                usage_file = stdout;
            default: /* '?' */
                fprintf(usage_file, "Usage: %s [-a addr] [-x|-c|-d] ip_address\n", argv[0]);
                fprintf(usage_file, "defaults device: %s addr: %x\n", DEFAULT_DEVICE, 0x42);
                exit(usage_file == stdout ? EXIT_SUCCESS : EXIT_FAILURE);
        }
    }

    int fd;

    CHKERR(fd = open(input, O_RDWR));

    return fd;
}

void i2c_handle(int i2c_fd, char tx_buffer[], int mode) {
    char *tx_answer;
    enum sys_call commande;
    char endstring[]={ENDSYMB};


    ssize_t length = read(i2c_fd, tx_buffer, TX_BUF_SIZE);
    for(int i = 0; i < length; i++)
    {
        switch (mode) {
            case 1:
                printf("1: Data received : %c\n", tx_buffer[i]);
				tx_answer = action(tx_buffer[i]);
				write(i2c_fd, endstring, 1);
				write(i2c_fd, tx_answer, 4);
				write(i2c_fd, endstring, 1);
                break;
            case 2:
                printf("2 :Data received : %02x\n ", tx_buffer[i]);
				tx_answer = action(tx_buffer[i]);
				write(i2c_fd, endstring, 1);
				write(i2c_fd, tx_answer, 4);
				write(i2c_fd, endstring, 1);            
                break;
            default:
            	printf("3 :Data received : %d \n", tx_buffer[i]);
				tx_answer = action(tx_buffer[i]);
				write(i2c_fd, endstring, 1);
				write(i2c_fd, tx_answer, 4);
				write(i2c_fd, endstring, 1);
				break;

        }
    }
    //decode_data(com, &is_commande, &nb_opt, tx_buffer);

    //write(i2c_fd, tx_buffer, length);
}

/*
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
		default:
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
}*/
