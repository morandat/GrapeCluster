//
// Created by cyrbos on 4/3/17.
//

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>


/* At least I tried
// Better to use define for constants
#define SUB_PROCESSES_NUMBER 3
#define FILE_OUT "out.txt"
#define FILE_LOGS "log.txt"

int get_cpu_usage() {
    char *command0[] = {"mpstat", NULL};
    char *command1[] = {"grep", "-A 5", "\"%idle\"", NULL};
    char *command2[] = {"tail", "-n 1", NULL};
    char *command3[] = {"awk", "'{print 100 - $13}'a", NULL};
    char **commands[SUB_PROCESSES_NUMBER + 1] = {command0, command1, command2, command3};

    pid_t pid[SUB_PROCESSES_NUMBER]; // good practice: fork() result is pid_t, not int
    int fd[SUB_PROCESSES_NUMBER][2];

    // I recommend opening files now, so if you can't you won't create unecessary processes
    int fd_file_out = open(FILE_OUT, O_WRONLY | O_CREAT | O_TRUNC, 00600);
    if (fd_file_out < 0)
    {
        perror("open(" FILE_OUT ")");
        return 2;
    }

    int fd_file_logs = open(FILE_LOGS, O_WRONLY | O_CREAT | O_TRUNC, 00600);
    if (fd_file_logs < 0)
    {
        perror("open(" FILE_LOGS ")");
        close(fd_file_out); // Not necessary, but I like to do it explicitly
        return 2;
    }

    for (int i = 0; i < SUB_PROCESSES_NUMBER; i++) // If you decide to add more steps, this loop will be handy
    {
        if (pipe(fd[i]) < 0)
        {
            perror("pipe");
            close(fd_file_out);
            close(fd_file_logs);
            if (i > 0)
            {
                close(fd[i - 1][0]);
            }
            return 2;
        }

        pid[i] = fork();
        if (pid[i] < 0)
        {
            perror("fork()");
            close(fd_file_out);
            close(fd_file_logs);
            if (i > 0)
            {
                close(fd[i - 1][0]);
            }
            close(fd[i][0]);
            close(fd[i][1]);
            return 2;
        }

        if (pid[i] == 0)
        {
            close(fd[i][0]); // First thing to do: close pipes and files you don't need any more
            close(fd_file_out);

            close(1);
            dup(fd[i][1]);
            close(fd[i][1]); // duplicated pipes are not useful any more

            close(2); // Also need to redirect stderr
            dup(fd_file_logs);
            close(fd_file_logs);

            if (i > 0)
            {
                close(0); // Also need to redirect stdin if this is not first process
                dup(fd[i - 1][0]);
                close(fd[i - 1][0]);
            }

            execvp(commands[i][0], commands[i]); // In a loop, we need a execv()/execvp()/execvpe() call
            return 2; // Should not be reached;
        }

        // sub process either did execvp() or return, he won't reach this point
        close(fd[i][1]);
        if (i > 0)
        {
            close(fd[i - 1][0]);
        }
    }

    close(fd_file_logs);

    close(0);
    dup(fd[SUB_PROCESSES_NUMBER - 1][0]);
    close(fd[SUB_PROCESSES_NUMBER - 1][0]);

    close(1);
    dup(fd_file_out);
    close(fd_file_out);

    execvp(commands[SUB_PROCESSES_NUMBER][0], commands[SUB_PROCESSES_NUMBER]);
    perror("execvp");
    return 2;
}*/