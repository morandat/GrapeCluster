//#include "libs/jsmn/jsmn.h"

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>


#include "daemon.h"
#include "i2cslave.h"

#include "commands.h"

#include "utils.h"

int main(int argc, char *argv[]) {
    load_orders(orders);
    struct daemon daemon;

    daemon.curr_status = ACTIVE;

    char tx_buffer[TX_BUF_SIZE];
    int mode;

    int i2c_fd = i2c_init(&mode, argc, argv, orders);

    if (argc < 2) {
        printf("Please provide ip address as first parameter\n");
        exit(EXIT_FAILURE);
    }

    fd_set rfds;

    FD_ZERO(&rfds);

    FD_SET(i2c_fd, &rfds);

    printf("Sending configure message to master \n");

    FILE *place_file = fopen(PLACE_FILE_PATH, "r");
    fseek(place_file, 0, SEEK_END);
    long fsize = ftell(place_file);
    fseek(place_file, 0, SEEK_SET);


    char *place_file_str = malloc(fsize + 1);

    fread(place_file_str, fsize, 1, place_file);

    fclose(place_file);
    place_file_str[fsize] = '\0';

    char conf_msg[12];

    sprintf(conf_msg, "configure;%c", place_file_str[0]);

    while (daemon.curr_status != STOPPED) {
        switch (daemon.curr_status) {
            case ACTIVE:

                struct timeval timeval;
                timeval.tv_sec = 1;
                timeval.tv_usec = 0;
                int fd_modified_count = select(i2c_fd + 1, &rfds, NULL, NULL, &timeval);

                CHKERR(fd_modified_count);

               if (FD_ISSET(i2c_fd, &rfds)) {
                    printf("received data over i2c\n");
                    i2c_handle(i2c_fd, tx_buffer, mode, &rfds);
                }

                break;
            default:
                close(i2c_fd);
                break;
        }
        //sleep(1);
    }


}
