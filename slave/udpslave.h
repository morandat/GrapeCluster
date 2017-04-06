//
// Created by cyrbos on 4/3/17.
//

#ifndef SLAVE_UDPSLAVE_H
#define SLAVE_UDPSLAVE_H

#define MASTER_IP_ADDRESS "172.20.1.165"

#define PORT 42666

int udp_init(struct sockaddr_in* slave_info, struct sockaddr_in* master_info, char* ip_addr);
void udp_handle(int sock, struct daemon* daemon, struct sockaddr_in* master_info, socklen_t master_info_len,
                struct sockaddr_in* slave_info);
#endif //SLAVE_UDPSLAVE_H
