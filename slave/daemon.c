//#include "libs/jsmn/jsmn.h"

#include <stdlib.h>
#include <stdio.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BUFFER_SIZE 1024
#define IN_PORT 42333
#define OUT_PORT 42666 //two sockets to simulate communication on the same computer

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

char orders[2] = {"ls", "shutdown"};


void* exec_order(int order) {
    printf("executing order %i : %s\n", order, orders[order]);
    exec(orders[order]);
}

int main() {
    /* need to read the doc about this shit
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



    int sock;
    CHKERR(sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP));


    char buffer[1024];

    struct sockaddr_in to = { 0 };
    int to_size = sizeof to;

    to.sin_addr = *(struct in_addr *) ;
    to.sin_port = htons(IN_PORT);
    to.sin_family = AF_INET;

    int received;

    while (!exit) {
        switch(curr_status) {
            case ACTIVE:
                CHKERR((received = recvfrom(sock, buffer, BUFFER_LENGTH - 1, 0, (struct sockaddr *) &to, &to_size)) < 0);
                buffer[received] = '\0';

                if (received > 0) {
                    exec_order(buffer[0]);
                    CHKERR(sendto(sock, buffer, BUFFER_lENGTH, 0, (struct sockaddr *) &to, to_size));
                }

                break;
            case STOPPED:
                close(sock);
            default:
                close(sock);
                break;
        }

    }
}