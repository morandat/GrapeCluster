//
// Created by cyrbos on 4/3/17.
//

#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "daemon.h"
#include "udpslave.h"
#include "utils.h"
#include "commands.h"

int udp_init(struct sockaddr_in* slave_info, struct sockaddr_in* master_info, char* ip_addr) {

    memset((char *) master_info, 0, sizeof(master_info));

    master_info->sin_family = AF_INET;
    master_info->sin_port = htons(PORT);
    master_info->sin_addr.s_addr = inet_addr(MASTER_IP_ADDRESS);

    int sock;
    CHKERR(sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP));

    memset((char*) slave_info, 0, sizeof(slave_info));

    slave_info->sin_family = AF_INET;
    slave_info->sin_port = htons(PORT);
    slave_info->sin_addr.s_addr = inet_addr(ip_addr);

    CHKERR(setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &(int){ 1 }, sizeof(int)));
    CHKERR(bind(sock, (struct sockaddr*)slave_info, sizeof(*slave_info)));

    return sock;
}

void udp_handle(int sock, struct daemon* daemon, struct sockaddr_in* master_info, socklen_t master_info_len,
                struct sockaddr_in* slave_info) {
    ssize_t recv_len = 0;
    char buffer[BUFF_LEN];
    CHKERR((recv_len = recvfrom(sock, buffer, BUFF_LEN, 0,
                                (struct sockaddr *) master_info, &master_info_len)));
    buffer[recv_len] = '\0';
	
    printf("received data : '%s'\n", buffer);
    if (recv_len > 0) {
        int arg_num = count_args(buffer, recv_len);
        char *args[MAX_ARG_SIZE];
	    slice_args(args, buffer, recv_len, arg_num);
        if (atoi(args[0]) == get_command_index("configure")) {
            slave_info->sin_addr.s_addr = inet_addr(args[1]);
        }
        else if (atoi(args[0]) == get_command_index("cpu")) {
            char cpu_usage[20];
            sprintf(cpu_usage, "cpu:%d", get_cpu_usage());
            sendto(sock, cpu_usage, strlen(cpu_usage), 0, (struct sockaddr *) master_info, master_info_len);
            printf("Sending cpu usage to master\n");
        }
        else {
            printf("Received unrecognized request\n");
        }
    }
    else {
        printf("received empty message\n");
    }
}
