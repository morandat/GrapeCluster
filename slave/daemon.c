//#include "libs/jsmn/jsmn.h"

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>


#include "daemon.h"
#include "i2cslave.h"
#include "udpslave.h"

#include "commands.h"

#include "utils.h"

enum status curr_status = ACTIVE;

int orders_num;

int count_args(char* msg, ssize_t msg_len) {
    int arg_num = 0;
    for (int i = 0; i < msg_len; ++i) {
        if (msg[i] == ';')
            arg_num++;
    }

    return arg_num;
}

char** slice_args(char* msg, ssize_t msg_len, int arg_num) {
    char** args = malloc(sizeof(char*) * arg_num);

    int start = 0;
    int j = 0;

    for (int i = 0; i < msg_len; ++i) {//msg contains first order char, so start at 1
        if (msg[i] == ';') {
            int length = i - start;
            args[j] = malloc(length * sizeof(char));
            strncpy(args[j], msg + start, (size_t) length);
            args[j][length] = 0;
            start = i + 1;
            //sprintf("Sliced : args[%i] = %s\n", j, args[j]);
            j++;
        }
    }

    return args;
}

void free_args(char** args, int arg_num) {
    for (arg_num; arg_num > 0; arg_num--) {
        free(args[arg_num-1]);
    }
    free(args);
}

void exec_order(int order_code, struct daemon* daemon) {//, char** args, int arg_num) {
    char* order = orders[order_code];
    printf("executing order %i : %s\n", order_code, order);

    fflush(stdin);
    fflush(stdout);
    FILE* pipe = popen(order, "r");
    if (pipe == NULL) {
        perror("popen failed");
        exit(EXIT_FAILURE);
    }


    daemon->exec_len = fread(daemon->exec_buff, sizeof(char), BUFF_LEN - 1, pipe);
    int err = ferror(pipe);
    if (err != 0) {
        perror("fread");
        exit(err);
    }

    pclose(pipe);
}

int order_str_to_code(char* str) {
    for (int i = 0; i < orders_num; i++) {
        if(strcmp(orders[i], str) == 0)
            return i;
    }
}


int get_cpu_usage() {
    float a[4], b[4], loadavg;
    FILE *fp;
    char dump[50];
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

char** load_orders() {
    FILE* orders_file = fopen("../orders.txt", "r");
    fseek(orders_file, 0, SEEK_END);
    long fsize = ftell(orders_file);
    fseek(orders_file, 0, SEEK_SET);

    char* orders_file_str = malloc(fsize+1);
    fread(orders_file_str, fsize, 1, orders_file);
    fclose(orders_file);
    orders_file_str[fsize] = 0;

    int oflen = strlen(orders_file_str);

    orders_num = count_args(orders_file_str, oflen);

    return slice_args(orders_file_str, oflen, orders_num);
}

int main(int argc, char *argv[]) {
    orders = load_orders();
    struct daemon daemon;

    daemon.curr_status = ACTIVE;

    char tx_buffer[TX_BUF_SIZE];
    int mode;

    int i2c_fd = i2c_init(&mode, argc, argv);
    uint8_t data;

    struct sockaddr_in master_info;
    struct sockaddr_in slave_info;
    socklen_t master_info_len;

    int sock;
    if (argc < 2) {
        printf("Please provide ip address as first parameter\n");
        exit(EXIT_FAILURE);
    }

    CHKERR(sock = udp_init(&slave_info, &master_info, argv[1]));

    master_info_len = sizeof(master_info);

    fd_set rfds;

    FD_ZERO(&rfds);
    //FD_SET(i2c_fd, &rfds);
    FD_SET(sock, &rfds);

    printf("Sending configure message to master \n");
    CHKERR(sendto(sock, "configure", strlen("configure"), 0, (struct sockaddr *) &master_info, master_info_len));

    int max_fd = sock+1;//(sock > i2c_fd) ? sock : i2c_fd;


    ssize_t recv_len;
    char buffer[BUFF_LEN];
    while (daemon.curr_status != STOPPED) {
        switch (daemon.curr_status) {
            case ACTIVE:
                printf("waiting for data...\n");
                struct timeval timeval;
                timeval.tv_sec = 1;
                timeval.tv_usec = 0;
                int fd_modified_count = select(max_fd, &rfds, NULL, NULL, &timeval);

                CHKERR(fd_modified_count);

                /*if (fd_modified_count == 0) {
                    continue;
                }*/

                if(FD_ISSET(sock, &rfds)) {
                    printf("received data over udp\n");
                    udp_handle(sock, &daemon, &master_info, master_info_len, &slave_info);
                }
                else if(FD_ISSET(i2c_fd, &rfds)) {
                    printf("received data over i2c\n");
                    i2c_handle(i2c_fd, tx_buffer, mode);
                }

                break;
            default:
                close(sock);
                break;
        }
        //sleep(1);
    }

    free(orders);
}
