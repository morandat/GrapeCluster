//
// Created by cyrbos on 3/31/17.
//

#ifndef SLAVE_I2CSLAVE_H
#define SLAVE_I2CSLAVE_H

#include "commands.h"

#define TX_BUF_SIZE   4

#define DEFAULT_DEVICE "/dev/i2c_slave"

void i2c_handle(int i2c_fd, char tx_buffer[], int mode);
int i2c_init(int* mode, int argc, char* argv[], char **ord);

#endif //SLAVE_I2CSLAVE_H
