//#include "libs/jsmn/jsmn.h"

#include <stdlib.h>
#include <stdio.h>

#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

#include "commands.h"
#include "daemon.h"
#include "i2cslave.h"
#include "utils.h"

enum status curr_status = ACTIVE;

int orders_num;

int count_args(char* msg, ssize_t msg_len) {
    int arg_num = 0;
    for (int i = 0; i < msg_len; ++i) {
        if (msg[i] == ';')
            arg_num++;
    }

    return arg_num;
}

char** slice_args(char* msg, ssize_t msg_len, int arg_num) {
    char** args = malloc(sizeof(char*) * arg_num);

    int start = 0;
    int j = 0;

    for (int i = 0; i < msg_len; ++i) {//msg contains first order char, so start at 1
        if (msg[i] == ';') {
            int length = i - start;
            args[j] = malloc(length * sizeof(char));
            strncpy(args[j], msg + start, (size_t) length);
            args[j][length] = 0;
            start = i + 1;
            //sprintf("Sliced : args[%i] = %s\n", j, args[j]);
            j++;
        }
    }

    return args;
}

void free_args(char** args, int arg_num) {
    for (arg_num; arg_num > 0; arg_num--) {
        free(args[arg_num-1]);
    }
    free(args);
}

void exec_order(int order_code, struct daemon* daemon) {//, char** args, int arg_num) {
    char* order = orders[order_code];
    printf("executing order %i : %s\n", order_code, order);

    fflush(stdin);
    fflush(stdout);
    FILE* pipe = popen(order, "r");
    if (pipe == NULL) {
        perror("popen failed");
        exit(EXIT_FAILURE);
    }


    daemon->exec_len = fread(daemon->exec_buff, sizeof(char), BUFF_LEN - 1, pipe);
    int err = ferror(pipe);
    if (err != 0) {
        perror("fread");
        exit(err);
    }

    pclose(pipe);
}

int order_str_to_code(char* str) {
    for (int i = 0; i < orders_num; i++) {
        if(strcmp(orders[i], str) == 0)
            return i;
    }
}


int get_cpu_usage() {
    float a[4], b[4], loadavg;
    FILE *fp;
    char dump[50];
    fp = fopen("/proc/stat", "r");
    fscanf(fp, "%*s %f %f %f %f", &a[0], &a[1], &a[2], &a[3]);
    fclose(fp);
    sleep(1);

    fp = fopen("/proc/stat", "r");
    fscanf(fp, "%*s %f %f %f %f", &b[0], &b[1], &b[2], &b[3]);
    fclose(fp);

    loadavg =
            ((b[0] + b[1] + b[2]) - (a[0] + a[1] + a[2])) / ((b[0] + b[1] + b[2] + b[3]) - (a[0] + a[1] + a[2] + a[3]));
    printf("Current CPU utilization is : %d\n", (int)(loadavg*100));

    return (int)loadavg*100;
}

int init_network(struct sockaddr_in* slave_info, struct sockaddr_in* master_info, char* ip_addr) {

    memset((char *) master_info, 0, sizeof(master_info));

    master_info->sin_family = AF_INET;
    master_info->sin_port = htons(PORT);
    master_info->sin_addr.s_addr = inet_addr(MASTER_IP_ADDRESS);

    int sock;
    CHKERR(sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP));

    memset((char*) slave_info, 0, sizeof(slave_info));

    slave_info->sin_family = AF_INET;
    slave_info->sin_port = htons(PORT);
    slave_info->sin_addr.s_addr = inet_addr(ip_addr);

    CHKERR(setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &(int){ 1 }, sizeof(int)));
    CHKERR(bind(sock, (struct sockaddr*)slave_info, sizeof(*slave_info)));

    return sock;
}

char** load_orders() {
    FILE* orders_file = fopen("../orders.txt", "r");
    fseek(orders_file, 0, SEEK_END);
    long fsize = ftell(orders_file);
    fseek(orders_file, 0, SEEK_SET);

    char* orders_file_str = malloc(fsize+1);
    fread(orders_file_str, fsize, 1, orders_file);
    fclose(orders_file);
    orders_file_str[fsize] = 0;

    int oflen = strlen(orders_file_str);

    orders_num = count_args(orders_file_str, oflen);

    return slice_args(orders_file_str, oflen, orders_num);
}

int main(int argc, char *argv[]) {
    fd_set rfds;

    FD_ZERO(&rfds);

    char tx_buffer[TX_BUF_SIZE];
    int i2c_fd;
    uint8_t data;
    size_t length;
    int i;

    int opt;
    int mode = 0;

    //int is_commande = 0;
    int nb_opt = -1;
    //struct commande *com = malloc(sizeof(struct commande));

    FILE *usage_file = stderr;
    const char *input = DEFAULT_DEVICE;

    while ((opt = getopt(argc, argv, "hxc")) != -1) {
        switch (opt) {
            case 'd':
                mode = 0; // TODO defines
                break;
            case 'c':
                mode = 1; // TODO defines
                break;
            case 'x':
                mode = 2; // TODO defines
                break;
            case 'h':
                usage_file = stdout;
            default: /* '?' */
                fprintf(usage_file, "Usage: %s [-a addr] [-x|-c|-d] ip_address\n", argv[0]);
                fprintf(usage_file, "defaults device: %s addr: %x\n", DEFAULT_DEVICE, 0x42);
                exit(usage_file == stdout ? EXIT_SUCCESS : EXIT_FAILURE);
        }
    }

    /*if (optind < argc) {
        input = argv[optind]; //+1 for ip ?
    }*/

    if ((i2c_fd = open(input, O_RDWR)) == -1) {
        perror("open i2c device");
        exit(EXIT_FAILURE);
    }

    //FD_SET(i2c_fd, &rfds);

    orders = load_orders();

    struct daemon daemon;

    struct sockaddr_in master_info;
    struct sockaddr_in slave_info;
    socklen_t master_info_len;
    int sock;
    if (argc < 2) {
        printf("Please provide ip address as first parameter\n");
        exit(EXIT_FAILURE);
    }

    CHKERR(sock = init_network(&slave_info, &master_info, argv[1]));
    FD_SET(sock, &rfds);


    master_info_len = sizeof(master_info);
    printf("Sending configure message to master \n");
    CHKERR(sendto(sock, "configure", strlen("configure"), 0, (struct sockaddr *) &master_info, master_info_len));

    int max_fd = sock+1;//(sock > i2c_fd) ? sock : i2c_fd;


    ssize_t recv_len;
    char buffer[BUFF_LEN];
    while (curr_status != STOPPED) {
        switch (curr_status) {
            case ACTIVE:
                printf("waiting for data...\n");
                struct timeval timeval;
                timeval.tv_sec = 1;
                timeval.tv_usec = 0;
                int fd_modified_count = select(max_fd, &rfds, NULL, NULL, &timeval);

                CHKERR(fd_modified_count);

                /*if (fd_modified_count == 0) {
                    continue;
                }*/

                if(FD_ISSET(sock, &rfds)) {
                    CHKERR((recv_len = recvfrom(sock, buffer, BUFF_LEN, 0,
                                               (struct sockaddr *) &master_info, &master_info_len)));
                    buffer[recv_len] = '\0';

                    printf("received data : '%s'\n", buffer);
                    if (recv_len > 0) {
                        int arg_num = count_args(buffer, recv_len);
                        char** args = slice_args(buffer, recv_len, arg_num);
                        if (strcmp(args[0], "9") == 0) {
                            curr_status = STOPPED;
                            close(sock);
                        }
                        else if(strcmp(args[0], "0") == 0) {
                            printf("Received master configuration\n");
                            slave_info.sin_addr.s_addr = inet_addr(args[1]);

                        }
                        else if(strcmp(args[0], "1") == 0) {
                            char cpu_usage[20];
                            sprintf(cpu_usage, "cpu:%d", get_cpu_usage());
                            sendto(sock, cpu_usage, strlen(cpu_usage), 0, (struct sockaddr *) &master_info, master_info_len);
                            printf("Sending cpu usage to master\n");
                        }
                        else {
                            printf("Order code : %s", args[0]);
                            int order_code = atoi(args[0]);
                            exec_order(order_code-1, &daemon);
                            printf("order returned :\n%s\nSending to master...", daemon.exec_buff);
                            CHKERR(sendto(sock, daemon.exec_buff, daemon.exec_len, 0, (struct sockaddr *) &master_info, master_info_len));
                        }
                        free_args(args, arg_num);
                    }
                    else {
                        printf("received empty message\n");
                    }
                }
                else if(FD_ISSET(i2c_fd, &rfds)) {
                    printf("received data over i2c\n");
                    length = read(i2c_fd, tx_buffer, TX_BUF_SIZE);
                    for(i = 0; i < length; i++)
                    {
                        switch (mode) {
                            case 1:
                                printf("1: Data received : %c\n", tx_buffer[i]);
                                break;
                            case 2:
                                printf("2 :Data received : %02x\n ", tx_buffer[i]);
                                break;
                            default:
                                printf("3 :Data received : %d \n", tx_buffer[i]);
                                break;
                        }
                    }
                    //decode_data(com, &is_commande, &nb_opt, tx_buffer);

                    write(i2c_fd, tx_buffer, length);
                }

                break;
            default:
                close(sock);
                break;
        }
        //sleep(1);
    }

    free(orders);
}
