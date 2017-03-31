//
// Created by cyrbos on 3/15/17.
//

#ifndef SLAVE_DAEMON_H
#define SLAVE_DAEMON_H

#define BUFF_LEN 1024

#define MASTER_IP_ADDRESS "172.20.1.136"
#define PORT 42666

enum status {
    ACTIVE,
    STOPPED
};

//master 172.20.1.165
//slave 136
char ** orders;

struct daemon {
    char exec_buff[BUFF_LEN];
    size_t exec_len;
};


void exec_order(int order_code, struct daemon* daemon);

#endif //SLAVE_DAEMON_H
