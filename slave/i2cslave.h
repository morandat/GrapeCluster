//
// Created by cyrbos on 3/31/17.
//

#ifndef SLAVE_I2CSLAVE_H
#define SLAVE_I2CSLAVE_H

#include "commands.h"
#include <sys/select.h>
#include <sys/types.h>

#define TX_BUF_SIZE  10 

#define DEFAULT_DEVICE "/dev/i2c_slave"

void i2c_handle(int i2c_fd, char tx_buffer[], int mode, fd_set* rdfs);
int i2c_init(int* mode, int argc, char* argv[]);

#endif //SLAVE_I2CSLAVE_H
