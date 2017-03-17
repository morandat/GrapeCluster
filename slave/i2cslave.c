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

void action(struct commande *com){
	
	char cmd[64];
	int i = 0; 

	switch(com->call){
		case ECHO:
			printf("Echo\n");
			sprintf(cmd, "echo");
			for (i =  0 ; i < com->nb_element ; i++){
				sprintf(cmd, "%s %s", cmd, com->option[i]);
			}
			system(cmd);
			printf("\n");
			break;
		case LS:
			printf("ls\n");
			sprintf(cmd, "ls");
			for (i =  0 ; i < com->nb_element ; i++){
				sprintf(cmd, "%s %s", cmd, com->option[i]);
			}
			system(cmd);
			printf("\n");
			break;
		case PS:
			printf("ps\n");
			sprintf(cmd, "ps");
			for (i =  0 ; i < com->nb_element ; i++){
				sprintf(cmd, "%s %s", cmd, com->option[i]);
			}
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

//15 2 -la -lb 
//Attendre de voir comment sont traité les chaines
void decode_data(struct commande *com, int *is_com, int *nb_opt, char *c){
	/**nb_opt = c[0];
	com->nb_element = nb_opt -1;
	com->option = malloc(sizeof(char *) * com->nb_element);

	com->call = c[1];
	*nb_opt --; 
	
	if(*nb_opt == 0)
		action(com);

	else{

	}*/

	if(*nb_opt = -1){
		printf("Nb option\n");
		*nb_opt = c[0];
		com->nb_element = *nb_opt - 1;
		com->option = malloc(sizeof(char *) * com->nb_element);
		*is_com = 1;											
	}
	else if (*is_com == 1){		
		printf("Commande\n");
		com->call = c[0];
		*nb_opt -= 1;
		*is_com = 0;
		if (*nb_opt == 0){
			action(com);
			*nb_opt = -1;
		}							
	}
	else{
		printf("Option :\n");
		com->option[*nb_opt] = c;
		if (*nb_opt == 0){
			action(com);
			*nb_opt = -1;
		}
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

	int is_commande = 0; 
	int nb_opt = -1; 
	struct commande *com = malloc(sizeof(struct commande));	

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
					break;
				case 2:
					printf("2 :Data received : %02x\n ", tx_buffer[i]);
					break;
				default:
					printf("3 :Data received : %d \n", tx_buffer[i]);
					break;
				}
			}
			decode_data(com, &is_commande, &nb_opt, tx_buffer);					

			write(fd, tx_buffer, length);
	}

	close(fd);
	return 0;
}
