#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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



int get_ip(char ** array){
	FILE *fp;
	char path[1035];

	fp = popen("/bin/ifconfig", "r");
	if (fp == NULL){
		printf("Failed to run ifconfig");
		exit(1);
	}
	
	char t1[1024];
	char t2[1024];
	
	/*char array[0][4];
	char array[1][4];
	char array[2][4];
	char array[3][4];*/

	
	while(fgets(path, sizeof(path) - 1, fp) != NULL) {
		//printf("YOOO : %s", path);
		sscanf(path, "%s %s", t1, t2);
		char *word;
		int nb = 0;
		int i = 0;
		if (strcmp("inet", t1)== 0){
			//printf("%s", path);
			word = strtok(path, " .:");
			while (word != NULL) {
				if(nb == 0){
					for(i = 0; i < 4 ; i++)
						array[0][i] = 0;
					for(i = 0; i < 4 ; i++)
						array[1][i] = 0;
					for(i = 0; i < 4 ; i++)
						array[2][i] = 0;
					for(i = 0; i < 4 ; i++)
						array[3][i] = 0;

				}
    			else if(nb == 2)
    				strcpy(array[0], word);
    			else if (nb == 3)
    				strcpy(array[1], word);
    			else if (nb == 4)
    				strcpy(array[2], word);
    			else if (nb == 5)
    				strcpy(array[3], word);
    			else if (nb == 6){
    				printf("%s : %s : %s : %s\n",array[0], array[1], array[2], array[3]);
    				if (strcmp(array[0], "127") == 0 && strcmp(array[1], "0") == 0 && strcmp(array[2], "0") == 0){
    					printf("No\n");
    				}
    				else{
    					printf("C'est fini\n");
    					return;
    				}

    			}
    			word = strtok(NULL, " .:");
    			nb ++;
    			//nbr_words += 1;
    		}

		}
	}
}

//connect: Network is unreachable
//PING 8.8.8.8
int test_network(){
	FILE *fp;
	char path[1035];
	char test[40];
	char end[1035];

	fp = popen("/bin/ping -c 1 8.8.8.8", "r");
	if (fp == NULL){
		printf("Failed to run ping");
		exit(1);
	}
		
	fgets(path, sizeof(path) - 1, fp);
	sscanf(path, "%s %s", test, end);
	//printf("%s ", test);
	if(strcmp("PING", test) != 0 || strcmp("connect:", test) == 0){
		pclose(fp);
		return 0;
	}
	pclose(fp);
	return 1;
}

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
	int test = 0;

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
			char *array[4];
			get_ip(array);
			return "abcd";
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

	return "abcd";
}

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

	char endline = 240;
	char endstring[]={endline};

	while (1) {	
			length = read(fd, tx_buffer, 4);
			for(i = 0; i < length; i++)
			{
				switch (mode) {
				case 1:
					printf("1: Data received : %c\n", tx_buffer[i]);
					tx_answer = action(tx_buffer[i]);
					write(fd, tx_answer, 4);
					write(fd, endstring, 1);
					break;
				case 2:
					printf("2 :Data received : %02x\n ", tx_buffer[i]);
					tx_answer = action(tx_buffer[i]);
					write(fd, tx_answer, 4);
					write(fd, endstring, 1);
					break;
				default:
					printf("3 :Data received : %d \n", tx_buffer[i]);
					tx_answer = action(tx_buffer[i]);
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
}
