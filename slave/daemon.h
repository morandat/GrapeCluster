//
// Created by cyrbos on 3/15/17.
//

#ifndef SLAVE_DAEMON_H
#define SLAVE_DAEMON_H

#define BUFF_LEN 1024
#define PORT 42666

enum status {
    ACTIVE,
    STOPPED
};

char *orders[2] = {"ps", "shutdown"};

struct daemon {
    char exec_buff[BUFF_LEN];
    size_t exec_len;
};


void exec_order(int order_code, struct daemon* daemon);

#endif //SLAVE_DAEMON_H
