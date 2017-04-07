//
// Created by cyrbos on 3/15/17.
//

#ifndef SLAVE_DAEMON_H
#define SLAVE_DAEMON_H

#define BUFF_LEN 1024

#define PLACE_FILE_PATH "/home/pi/place.txt"

enum status {
    ACTIVE,
    STOPPED
};

struct daemon {
    enum status curr_status;
    char exec_buff[BUFF_LEN];
    size_t exec_len;
};

#endif //SLAVE_DAEMON_H
