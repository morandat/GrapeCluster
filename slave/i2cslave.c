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
int is_addr;

void get_order(int order_code, char **order) {
    *order = orders[order_code];
}

void action(int call, char *out){
	char *order;

    if (call == 66) {
        printf("Received 66\n");
    }
    else {
        get_order(call, &order);
        printf("order %d %s\n", call, order);

        if (strcmp("test", order) == 0) {
            test_communication();
            
            out[0] = 'a';
            out[1] = 'b';
            out[2] = 'c';
            out[3] = 'd';
        }
        else if(strcmp("cpu", order) == 0){
            int cpu = get_cpu_usage();
            out[0] = '0';
            out[1] = '0';
            out[2] = '0';
            out[3] = cpu;

        }
        else if(strcmp("shutdown", order) == 0){
            shutdown_slave();
        }
        else if(strcmp("reboot", order) == 0){
            restart_slave();
        }
        else if(strcmp("get_ip", order) == 0){
            char tmp[4];
            get_ip(in);
            encode_ip(&tmp, in);
            int i;
            for(i = 0; i < 4 ; i++)
                out[i] = tmp[4];

        }
        else if(strcmp("get_i2c", order) == 0){
            char pos = get_i2c();
            out[0] = '0';
            out[1] = '0';
            out[2] = '0';
            out[3] = pos;
        }
        else if(strcmp("is_network", order) == 0){
            test = test_network();
            if(test == 1){
                out[0] = 1;
                out[1] = 1;
                out[2] = 1;
                out[3] = 1;
            }
            else{
                out[0] = 0;
                out[1] = 0;
                out[2] = 0;
                out[3] = 0;
            }
        }
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

    is_addr = 1;
    return fd;
}


void i2c_handle(int i2c_fd, char tx_buffer[], int mode, fd_set* rdfs) {
    int test;
    for (test = 0 ; test < TX_BUF_SIZE; test++)
	tx_buffer[test]=-1 ; 

    size_t length = read(i2c_fd, tx_buffer, TX_BUF_SIZE); 

    if (strcmp(tx_buffer, "8") == 0) {
        FD_CLR(i2c_fd, rdfs);
    }

    printf("Début test\n");

    char tx_answer[4];
    char endstring[]={ENDSYMB};

    
    for(int i = 0; i < length; i++)
	{
    	switch (mode) {
    	case 1:
        		printf("1: Data received : %c\n", tx_buffer[i]);
        		action(tx_buffer[i], tx_answer);
                if(tx_buffer[i] != 255 && tx_buffer[i] != '2' tx_buffer[i] != '3')
                    write(i2c_fd, tx_answer, 4);
        	break;
    	case 2:
        		printf("2 :Data received : %02x\n ", tx_buffer[i]);
        		action(tx_buffer[i], tx_answer);
                if(tx_buffer[i] != 255 && tx_buffer[i] != '2' tx_buffer[i] != '3')
                    write(i2c_fd, tx_answer, 4);
        	break;
    	default:
	   	
       		printf("3 :Data received : %d \n", tx_buffer[i]);
        		action(tx_buffer[i], tx_answer);
    		if(tx_buffer[i] != 255 && tx_buffer[i] != '2' tx_buffer[i] != '3')
        			write(i2c_fd, tx_answer, 4);
        	break;
    	}
   	}
 	  
}

