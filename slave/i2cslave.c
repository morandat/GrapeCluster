#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <curses.h>
#include <string.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "i2cslave.h"
#include "utils.h"

#define ENDSYMB		  240
#define DEFAULT_DEVICE "/dev/i2c_slave"


char **orders;

void get_order(int order_code, char **order){
    *order = orders[order_code];

void action(int call, char **out){
		
	int i = 0; 
	int test = 0;
	char *in[4];

	char *order;

    if (call == 66) {
        printf("Received 66\n");
    }
    else {
        get_order(call, &order);


        if (strcmp("test", order) == 0) {
            test_communication();
            *out = "abcd";
        }
        /*else if(strcmp("cpu", order) == 0){
            int cpu = get_cpu_usage();
            char c = (char)cpu;
            sprintf(out, "000%c", c);
        }
        else if(strcmp("shutdown", order) == 0){
            shutdown_slave();
        }
        else if(strcmp("reboot", order) == 0){
            restart_slave();
        }
        else if(strcmp("get_ip", order) == 0){
            get_ip(in);
            encode_ip(out, in);
        }
        else if(strcmp("get_i2c", order) == 0){
            printf("Coucou\n");
            out = "0042";
        }
        else if(strcmp("is_network", order) == 0){
            test = test_network();
            if(test == 1)
                out = "1111";
            else
                out = "0000";
        }*/
    }
}

int i2c_init(int* mode, int argc, char* argv[], char **ord) {
    int opt;
    *mode = 0;

    orders = ord;

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

void i2c_handle(int i2c_fd, char tx_buffer[], int mode, fd_set* wfds) {
    size_t length = read(i2c_fd, tx_buffer, TX_BUF_SIZE);
    char *tx_answer;
    char endstring[]={ENDSYMB};

    for(int i = 0; i < length; i++)
    {
            switch (mode) {
            case 1:
                printf("1: Data received : %c\n", tx_buffer[i]);
                action(tx_buffer[i], &tx_answer);
                //write(i2c_fd, endstring, 1);
                if (FD_ISSET(i2c_fd, wfds)) {
                    write(i2c_fd, tx_answer, 4);
                }
                //write(i2c_fd, endstring, 1);
                break;
            case 2:
                printf("2 :Data received : %02x\n ", tx_buffer[i]);
                action(tx_buffer[i], &tx_answer);
                //write(i2c_fd, endstring, 1);
                if (FD_ISSET(i2c_fd, wfds)) {
                    write(i2c_fd, tx_answer, 4);
                }
                //write(i2c_fd, endstring, 1);
                break;
            default:
                printf("3 :Data received : %d \n", tx_buffer[i]);
                action(tx_buffer[i], &tx_answer);
                //write(i2c_fd, endstring, 1);
                //write(i2c_fd, tx_answer, 4);
                if (FD_ISSET(i2c_fd, wfds)) {
                    write(i2c_fd, tx_answer, 4);
                }
                //write(i2c_fd, endstring, 1);
                break;
            }
        }
    }
}
    //decode_data(com, &is_commande, &nb_opt, tx_buffer);

    //write(i2c_fd, tx_buffer, length);
}

/*
int main(int argc, char **argv)
{
	char tx_buffer[TX_BUF_SIZE];
	char *tx_answer;
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


	char endstring[]={ENDSYMB};

	while (1) {	
			length = read(fd, tx_buffer, 4);
			for(i = 0; i < length; i++)
			{
				switch (mode) {
				case 1:
					printf("1: Data received : %c\n", tx_buffer[i]);
					tx_answer = action(tx_buffer[i]);
					write(fd, endstring, 1);
					write(fd, tx_answer, 4);
					write(fd, endstring, 1);
					break;
				case 2:
					printf("2 :Data received : %02x\n ", tx_buffer[i]);
					tx_answer = action(tx_buffer[i]);
					write(fd, endstring, 1);
					write(fd, tx_answer, 4);
					write(fd, endstring, 1);
					break;
				default:
					printf("3 :Data received : %d \n", tx_buffer[i]);
					tx_answer = action(tx_buffer[i]);
					write(fd, endstring, 1);
					write(fd, tx_answer, 4);
					write(fd, endstring, 1);
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
