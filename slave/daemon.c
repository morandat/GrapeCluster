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

#define BUFFER_LEN 1024

#define SLAVE_IP "127.0.0.2"

#define SLAVE_PORT 42333

#define CHKERR(X) \
    if ((X) < 0) \
    { \
        perror(#X); \
        exit(errno); \
    }


enum status {
    ACTIVE,
    STOPPED
};

enum status curr_status = ACTIVE;

char *orders[2] = {"ls ", "shutdown "};


void exec_order(int order_code, char *buffer, ssize_t len) {
    char* order = orders[order_code];
    printf("executing order %i : %s\n", order_code, order);

    char *args[len+1];

    args[0] = order;
    args[len-1] = "> toto.txt";
    args[len] = NULL;
    int start = 1;
    int j = 1;
    /* slice arguments like a cake, yummy */
    for (int i = 1; i < len-1; ++i) {
        if (buffer[i] == ';') {
            int length = i - start;
            args[j] = malloc(length * sizeof(char));
            strncpy(args[j], buffer + start, (size_t) length);
            start = i + 1;
            printf("args[%i] = %s\n", j, args[j]);
            j++;
        }
    }

    CHKERR(execvp(order, args));
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

    struct sockaddr_in slave_info, master_info;

    int sock;
    socklen_t master_info_len = sizeof(master_info);
    ssize_t recv_len;
    char buffer[BUFFER_LEN];


    CHKERR(sock=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP));

    // zero out the structure
    memset((char *) &slave_info, 0, sizeof(slave_info));

    slave_info.sin_family = AF_INET;
    slave_info.sin_port = htons(SLAVE_PORT);
    slave_info.sin_addr.s_addr = htonl(INADDR_ANY);


    CHKERR(bind(sock , (struct sockaddr*)&slave_info, sizeof(slave_info) ));

    while (curr_status != STOPPED) {
        switch (curr_status) {
            case ACTIVE:
                printf("waiting for data\n");

                CHKERR((recv_len = recvfrom(sock, buffer, BUFFER_LEN, 0,
                                            (struct sockaddr *) &master_info, &master_info_len)));
                buffer[recv_len] = '\0';

                printf("received data : '%s'\n", buffer);
                if (recv_len > 0) {
                    if (strcmp(&buffer[0], "9") == 0) {
                        curr_status = STOPPED;
                        close(sock);
                    } else {
                        int order_code = atoi(&buffer[0]);
                        exec_order(order_code, buffer, recv_len+1);
                        CHKERR(sendto(sock, buffer, (size_t) recv_len, 0, (struct sockaddr *) &master_info, master_info_len));
                    }
                }
                break;
            default:
                close(sock);
                break;
        }
        //sleep(1);
    }
}
