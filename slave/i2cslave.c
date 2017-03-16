#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <curses.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#define TX_BUF_SIZE   4000

#define DEFAULT_DEVICE "/dev/i2c_slave"

enum sys_call {
	ECHO, 
	LS,
	PS,
	SHUTDOWN
};

struct commande{
	enum sys_call call;
	char **option;
	int nb_element;
};

void action(struct commande com){
	
	char cmd[64];

	switch(com.call){
		case ECHO:
			printf("Echo\n");
			sprintf(cmd, "echo toto");
			system(cmd);
			printf("\n");
			break;
		case LS:
			printf("ls\n");
			sprintf(cmd, "ls");
			system(cmd);
			printf("\n");
			break;
		case PS:
			printf("ps\n");
			sprintf(cmd, "ps");
			system(cmd);
			printf("\n");
			break;
		case SHUTDOWN:
			printf("shutdown");
			sprintf(cmd, "shutdown -h now");
			system(cmd);
			printf("\n");
			break;
	}
}

int main(int argc, char **argv)
{
	char tx_buffer[TX_BUF_SIZE];
	int fd;
	uint8_t data;
	int length;
	int i;
	
		

	int opt;
	int mode = 0;

	int nb_opt = -1; 
	int is_commande = 0;
	struct commande com;	

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
			length = read(fd, tx_buffer, TX_BUF_SIZE);
			for(i = 0; i < length; i++)
			{
				switch (mode) {
				case 1:
					printf("1: Data received : %c\n", tx_buffer[i]);
					if(nb_opt = -1){
						printf("Nb option\n");
						nb_opt = tx_buffer[i];
						com.nb_element = nb_opt - 1;
						com.option = malloc(sizeof(char *) * com.nb_element);
						is_commande = 1;											
					}
					else if (is_commande == 1){		
						printf("Commande\n");
						com.call = tx_buffer[i];
						nb_opt --;
						is_commande = 0;
						if (nb_opt == 0)
							action(com);							
					}				
					break;
				case 2:
					printf("2 :Data received : %02x\n ", tx_buffer[i]);
					if(nb_opt = -1){
						printf("Nn option\n");
                                                nb_opt = tx_buffer[i];
                                                com.nb_element = nb_opt;
                                                com.option = malloc(sizeof(char *) * com.nb_element);
                                                is_commande = 1;                  
                                        }
                                        else if (is_commande == 1){             
                                        	printf("Commande\n"); 
					       	com.call = tx_buffer[i];
                                                nb_opt --;
                                                is_commande = 0;
						if(nb_opt == 0)
							action(com);
                                        }
					break;
				default:
					printf("3 :Data received : %d \n", tx_buffer[i]);
					if(nb_opt = -1){
                                         	printf("Nb option\n");
						nb_opt = tx_buffer[i];
                                                com.nb_element = nb_opt;
                                                com.option = malloc(sizeof(char *) * com.nb_element);
                                                is_commande = 1;                  
                                        }
                                        else if (is_commande == 1){             
                                                printf("Commande\n");
						com.call = tx_buffer[i];
                                                nb_opt --;
                                                is_commande = 0;
                                        	if(nb_opt == 0)
							action(com);
					}
					break;
				}
			}
			write(fd, tx_buffer, length);
	}

	close(fd);
	return 0;
}
