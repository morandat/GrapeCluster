//
// Created by cyrbos on 3/31/17.
//

#ifndef SLAVE_I2CSLAVE_H
#define SLAVE_I2CSLAVE_H

#define TX_BUF_SIZE   4

#define DEFAULT_DEVICE "/dev/null"

void i2c_handle(int i2c_fd, char tx_buffer[], int mode, fd_set* rfds);
int i2c_init(int* mode, int argc, char* argv[]);

#endif //SLAVE_I2CSLAVE_H
