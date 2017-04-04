//
// Created by cyrbos on 3/15/17.
//

#ifndef SLAVE_DAEMON_H
#define SLAVE_DAEMON_H

#define BUFF_LEN 1024

enum status {
    ACTIVE,
    STOPPED
};

char ** orders;

struct daemon {
    enum status curr_status;
    char exec_buff[BUFF_LEN];
    size_t exec_len;
};

int count_args(char* msg, ssize_t msg_len);
char** slice_args(char* msg, ssize_t msg_len, int arg_num);
void free_args(char** args, int arg_num);

void exec_order(int order_code, struct daemon* daemon);
void get_order(int order_code, char *order);

#endif //SLAVE_DAEMON_H
