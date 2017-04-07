

#include "commands.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <curses.h>
#include <unistd.h>


int encode_ip(char *out, char ** in){
	int i = 0;
	for (i = 0 ; i < 4 ; i++){
		out[i] = atoi(in[i]);
	}	
	return 0;
}

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
		sscanf(path, "%s %s", t1, t2);
		char *word;
		int nb = 0;
		int i = 0;
		if (strcmp("inet", t1)== 0){
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
    					return 1;
    				}

    			}
    			word = strtok(NULL, " .:");
    			nb ++;
    			//nbr_words += 1;
    		}
		}
	}

    return 0;
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
	sprintf(cmd, "echo test");
	system(cmd);

    return 1;
}

int shutdown_slave(){
	char cmd[64];
	//printf("shutdown");
	sprintf(cmd, "shutdown -h now");
	system(cmd);
	//printf("\n");

    return 1;
}

int restart_slave(){
	char cmd[64];
	sprintf(cmd, "shutdown -r");
	system(cmd);

    return 1;
}

int get_cpu_usage() {
    float a[4], b[4], loadavg;
    FILE *fp;

    fp = fopen("/proc/stat", "r");
    fscanf(fp, "%*s %f %f %f %f", &a[0], &a[1], &a[2], &a[3]);
    fclose(fp);
    sleep(1);

    fp = fopen("/proc/stat", "r");
    fscanf(fp, "%*s %f %f %f %f", &b[0], &b[1], &b[2], &b[3]);
    fclose(fp);

    loadavg =
            ((b[0] + b[1] + b[2]) - (a[0] + a[1] + a[2])) / ((b[0] + b[1] + b[2] + b[3]) - (a[0] + a[1] + a[2] + a[3]));
    printf("Current CPU utilization is : %d\n", (int)(loadavg*100));

    return (int)loadavg*100;
}

int get_i2c(){
	FILE* place_file = fopen("/home/pi/place.txt", "r");
    fseek(place_file, 0, SEEK_END);
    long fsize = ftell(place_file);
    fseek(place_file, 0, SEEK_SET);
	

    char* place_file_str = malloc(fsize+1);
    fread(place_file_str, fsize, 1, place_file);
    fclose(place_file);
    place_file_str[fsize] = '\0';

    return place_file_str[0];
}

int count_args(char *msg, int msg_len) {
    int arg_num = 0;
    for (int i = 0; i < msg_len; ++i) {
        if (msg[i] == ';')
            arg_num++;
    }

    return arg_num;
}

void slice_args(char **args, char *msg, int msg_len, int arg_num) {
    int start = 0;
    int j = 0;

    for (int i = 0; i < msg_len; ++i) {//msg contains first order char, so start at 1
        if (msg[i] == ';') {
            msg[i] = 0;
            args[j] = msg + start;
            printf("args[%d]=%s", j, args[j]);

            start = i+1;
            j++;
        }
    }
}

void load_orders(char **orders) {
    FILE *orders_file = fopen(ORDERS_PATH, "r");
    fseek(orders_file, 0, SEEK_END);
    long fsize = ftell(orders_file);
    fseek(orders_file, 0, SEEK_SET);

    char *orders_file_str = malloc(fsize + 1);
    fread(orders_file_str, fsize, 1, orders_file);
    fclose(orders_file);
    orders_file_str[fsize] = 0;

    int oflen = strlen(orders_file_str);

    orders_len = count_args(orders_file_str, oflen);

    slice_args(orders, orders_file_str, oflen, orders_len);
}

int get_command_index(char* str) {
    for (int i = 0; i < orders_len; i++) {
        if (strcmp(orders[i], str) == 0)
            return i;
    }
    return -1;
}

int command_dispatcher(char* command_str, char** ip) {
	if (strcmp("cpu", command_str) == 0) {
		return get_cpu_usage();
	}
    else if (strcmp("test", command_str) == 0) {
        return test_communication();
    }
    else if (strcmp("shutdown", command_str) == 0) {
        return restart_slave();
    }
    else if (strcmp("get_ip", command_str) == 0) {
        return get_ip(ip);
    }
    else if (strcmp("get_i2c", command_str) == 0) {
        return get_i2c();
    }
    else if (strcmp("is_network", command_str) == 0) {
        return test_network();
    }
}
