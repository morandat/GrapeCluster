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
                struct sockaddr_in* slave_info, fd_set* rdfs, int i2c_fd) {
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
	
	for (int i = 0; i < arg_num; i++) {
		printf("arg[%d]=%s", i, args[i]);
	}
        if (strcmp(args[0], "9") == 0) {
            daemon->curr_status = STOPPED;
            close(sock);
        }
        else if (strcmp(args[0], "0") == 0) {
            printf("Received master configuration\n");
            slave_info->sin_addr.s_addr = inet_addr(args[1]);

        }
        else if (strcmp(args[0], "1") == 0) {
            char cpu_usage[20];
            sprintf(cpu_usage, "cpu:%d", get_cpu_usage());
	printf("%d %s\n", cpu_usage, inet_ntoa(master_info->sin_addr));
            sendto(sock, cpu_usage, strlen(cpu_usage), 0, (struct sockaddr *) master_info, master_info_len);
            printf("Sending cpu usage to master\n");
        }
        else if(strcmp(args[0], "8") == 0) {
            FD_SET(i2c_fd, rdfds);
        }
        else {
            printf("Order code : %s", args[0]);
            int order_code = atoi(args[0]);
            exec_order(order_code - 1, daemon);
            printf("order returned :\n%s\nSending to master...", daemon->exec_buff);
            CHKERR(sendto(sock, daemon->exec_buff, daemon->exec_len, 0, (struct sockaddr *) master_info,
                          master_info_len));
        }
        //free_args(args, arg_num);
    }
    else {
        printf("received empty message\n");
    }
}
