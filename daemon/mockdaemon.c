#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/time.h>

int stop = 0;

void sig_handler(int signo)
{
    if (signo == SIGINT || signo == SIGTERM || signo == SIGSTOP){
        stop = 1;
    }
}

int main() {

    FILE* file;

    signal(SIGINT, sig_handler);
    signal(SIGTERM, sig_handler);
    signal(SIGSTOP, sig_handler);

    file = fopen("/tmp/testlog.txt", "a");

    fprintf(file, "[LOG] Beginning Loop at %d !!\n", getpid());

    int rounds = 0;

    while(!stop) {
        fprintf(file, "[LOG] Loop n° %d\n", rounds);
        rounds ++;
        sleep(1);
    }

    fprintf(file, "[LOG] Exitting loop ...\n");
    fclose(file);

    return EXIT_SUCCESS;
}