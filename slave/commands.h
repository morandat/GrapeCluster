//
// Created by cyrbos on 4/3/17.
//

#ifndef SLAVE_COMMANDS_H
#define SLAVE_COMMANDS_H

#define MAX_ARG_SIZE 16

#define ORDERS_PATH "../orders.txt"

char * orders[MAX_ARG_SIZE];

int orders_len;

/**
 *
 * @param msg formatted string as orders.txt file
 * @param msg_len length of msg
 * @return the number of args in msg string, formatted as orders.txt file
 */
int count_args(char* msg, int msg_len);

/**
 *
 * @param args arrays of char*, each being one of unformatted substrings
 * @param msg formatted string as orders.txt file
 * @param msg_len length of msg
 * @param arg_num number of substrings in msg (given by count_args())
 */
void slice_args(char** args, char* msg, int msg_len, int arg_num);

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


/**
 *
 * @param str the order str
 * @return the index of str in orders
 */
int get_command_index(char* str);

/**
 *
 * @param command_str name of the command to translate into a function call
 * @param ip needed to call get_ip() function
 */
void command_dispatcher(char* command_str, char** ip);

#endif //SLAVE_COMMANDS_H


