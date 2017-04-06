//
// Created by cyrbos on 4/3/17.
//

#ifndef SLAVE_COMMANDS_H
#define SLAVE_COMMANDS_H


/**
 *Return the cpu usage of a rasp in percentage
 *
 */
int get_cpu_usage();

/**
 * in : array of 4 strings
 * each string is one number of the ip address
 * ex : 172.100.20.1 in = {"172", "100", "20", "1"}
 * 
 * out : an array of 4 char to store the results
 * is in char because we need to send only 4 bytes
 * 
 * Given an array of 4 string in, this function return an array of 4 char
 * which is the translation of string into char
 */
int encode_ip(char *out, char ** in);

/**
* Parse the ifconfig function to return an array of 4 strings
* which contains the ip address
*
*/
int get_ip(char ** array);

/**
 * Test with ping if the network is avalaible
 * return 1 if it is ok
 * else return 0
 */
int test_network();

/**
 * Test which allows to verify that the rasp receive
 * the data in i2c
 */
int test_communication();

/**
 * Shutdown a slave
 *
 */
int shutdown_slave();

/**
 * Restart a slave
 *
 */
int restart_slave();

/**
 * Return a percentage of the cpu usage
 *
 */
int get_cpu_usage();

#endif //SLAVE_COMMANDS_H


