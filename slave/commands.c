

#include "commands.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <curses.h>
#include <unistd.h>


int encode_ip(char *out, char * i1, char *i2, char *i3, char *i4){
	
	out[0] = atoi(i1);	
	out[1] = atoi(i2);
	out[2] = atoi(i3);
	out[3] = atoi(i4);
	return 0;
}

int get_ip(char *i1, char *i2, char *i3, char *i4){
	FILE *fp;
	char path[1035];

	fp = popen("/sbin/ifconfig", "r");
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
						i1[i] = 0;
					for(i = 0; i < 4 ; i++)
						i2[i] = 0;
					for(i = 0; i < 4 ; i++)
						i3[i] = 0;
					for(i = 0; i < 4 ; i++)
						i4[i] = 0;

				}
    			else if(nb == 2)
    				strcpy(i1, word);
    			else if (nb == 3)
    				strcpy(i2, word);
    			else if (nb == 4)
    				strcpy(i3, word);
    			else if (nb == 5)
    				strcpy(i4, word);
    			else if (nb == 6){
    			//	printf("%s : %s : %s : %s\n",i1, i2, i3, i4);
    				if (strcmp(i1, "127") == 0 && strcmp(i2, "0") == 0 && strcmp(i3, "0") == 0){
    			//		printf("No\n");
    				}
    				else{
    			//		printf("C'est fini\n");
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

    return (int)(loadavg*100);
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