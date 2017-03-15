//#include "libs/jsmn/jsmn.h"

#include <stdlib.h>
#include <stdio.h>

#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

#include "daemon.h"
#include "utils.h"

enum status curr_status = ACTIVE;

int count_args(char* msg, ssize_t msg_len) {
    int arg_num = 0;
    for (int i = 0; i < msg_len; ++i) {
        printf("%c\n", msg[i]);
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
            start = i + 1;
            printf("Sliced : args[%i] = %s\n", j, args[j]);
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

int main(int argc, char *argv[]) {
    /* need to read the doc about this
    jsmn_parser parser;

    jsmn_init(&parser);
    jsmntok_t tokens[256];
    const char *js = "orders.json";
    int r;

    r = jsmn_parse(&parser, js, strlen(js), tokens, 256);
    if (r < 0) {
        printf("error parsing json %i\n", r);
    }
     */

    struct daemon daemon;

    struct sockaddr_in slave_info, master_info;


    memset((char *) &master_info, 0, sizeof(master_info));

    master_info.sin_family = AF_INET;
    master_info.sin_port = htons(PORT);
    master_info.sin_addr.s_addr = inet_addr("127.0.0.2");



    socklen_t master_info_len = sizeof(master_info);


    int sock;
    CHKERR(sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP));

    // zero out the structure
    memset((char *) &slave_info, 0, sizeof(slave_info));

    slave_info.sin_family = AF_INET;
    slave_info.sin_port = htons(PORT);
    slave_info.sin_addr.s_addr = inet_addr(argv[1]);

    CHKERR(bind(sock, (struct sockaddr*)&slave_info, sizeof(slave_info) ));
    printf("Sending configure message to master \n");
    CHKERR(sendto(sock, "configure", strlen("configure"), 0, (struct sockaddr *) &master_info, master_info_len));

    ssize_t recv_len;
    char buffer[BUFF_LEN];
    while (curr_status != STOPPED) {
        switch (curr_status) {
            case ACTIVE:
                printf("waiting for data...\n");

                CHKERR((recv_len = recvfrom(sock, buffer, BUFF_LEN, 0,
                                            (struct sockaddr *) &master_info, &master_info_len)));
                buffer[recv_len] = '\0';

                printf("received data : '%s'\n", buffer);
                if (recv_len > 0) {
                    int arg_num = count_args(buffer, recv_len);
                    char** args = slice_args(buffer, recv_len, arg_num);

                    if (strcmp(args[0], "9") == 0) {
                        curr_status = STOPPED;
                        close(sock);
                    }
                    else if(strcmp(args[0], "0") == 0) {
                        slave_info.sin_addr.s_addr = inet_addr(args[1]);
                        free_args(args, arg_num);
                    }
                    else {
                        int order_code = atoi(args[0]);
                        exec_order(order_code-1, &daemon);//, buffer, recv_len+1);
                        printf("order returned :\n%s\nSending to master...", daemon.exec_buff);
                        CHKERR(sendto(sock, daemon.exec_buff, daemon.exec_len, 0, (struct sockaddr *) &master_info, master_info_len));
                    }
                }
                else {
                    printf("received empty message\n");
                }
                break;
            default:
                close(sock);
                break;
        }
        //sleep(1);
    }
}
