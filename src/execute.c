#include "shell.h"
#include <fcntl.h>

int execute(char* arglist[]) {

    if (arglist[0] == NULL) {
        return 0;
    }

    if (strcmp(arglist[0], "exit") == 0) {
        exit(0);
    }

    if (strcmp(arglist[0], "cd") == 0) {

        if (arglist[1] == NULL) {
            if (chdir(getenv("HOME")) != 0) {
                perror("cd");
            }
        }
        else {
            if (chdir(arglist[1]) != 0) {
                perror("cd");
            }
        }

        return 0;
    }

    if (strcmp(arglist[0], "help") == 0) {
        printf("Available commands:\n");
        printf("exit - Exit the shell\n");
        printf("cd - Change directory\n");
        printf("help - Display available commands\n");
        printf("jobs - Display job information\n");
        printf("history - Display command history\n");
        return 0;
    }

    if (strcmp(arglist[0], "jobs") == 0) {
        printf("Job control is not implemented yet.\n");
        return 0;
    }

    if (strcmp(arglist[0], "history") == 0) {
        show_history();
        return 0;
    }

    char* left[MAXARGS + 1];
    char* right[MAXARGS + 1];

    char* input_file = NULL;
    char* output_file = NULL;

    int i = 0;
    int j = 0;
    int pipe_found = 0;

    while (arglist[i] != NULL) {

        if (strcmp(arglist[i], "|") == 0) {
            pipe_found = 1;
            break;
        }

        i++;
    }

    if (pipe_found) {

        for (i = 0; i < j + MAXARGS + 1; i++) {
            if (arglist[i] == NULL) {
                break;
            }
        }

        i = 0;
        j = 0;

        while (arglist[i] != NULL && strcmp(arglist[i], "|") != 0) {
            left[j] = arglist[i];
            j++;
            i++;
        }

        left[j] = NULL;
        i++;

        j = 0;

        while (arglist[i] != NULL) {
            right[j] = arglist[i];
            j++;
            i++;
        }

        right[j] = NULL;

        if (left[0] == NULL || right[0] == NULL) {
            printf("Error: invalid pipe command.\n");
            return 1;
        }

        int fd[2];
        int status;

        if (pipe(fd) == -1) {
            perror("pipe");
            return 1;
        }

        int pid1 = fork();

        if (pid1 == 0) {
            close(fd[0]);
            dup2(fd[1], STDOUT_FILENO);
            close(fd[1]);

            execvp(left[0], left);
            perror("Command not found");
            exit(1);
        }

        int pid2 = fork();

        if (pid2 == 0) {
            close(fd[1]);
            dup2(fd[0], STDIN_FILENO);
            close(fd[0]);

            execvp(right[0], right);
            perror("Command not found");
            exit(1);
        }

        close(fd[0]);
        close(fd[1]);

        waitpid(pid1, &status, 0);
        waitpid(pid2, &status, 0);

        return 0;
    }

    char* args[MAXARGS + 1];
    i = 0;
    j = 0;

    while (arglist[i] != NULL) {

        if (strcmp(arglist[i], "<") == 0) {

            if (arglist[i + 1] == NULL) {
                printf("Error: input filename missing.\n");
                return 1;
            }

            input_file = arglist[i + 1];
            i = i + 2;
        }
        else if (strcmp(arglist[i], ">") == 0) {

            if (arglist[i + 1] == NULL) {
                printf("Error: output filename missing.\n");
                return 1;
            }

            output_file = arglist[i + 1];
            i = i + 2;
        }
        else {
            args[j] = arglist[i];
            i++;
            j++;
        }
    }

    args[j] = NULL;

    if (args[0] == NULL) {
        printf("Error: command missing.\n");
        return 1;
    }

    int status;
    int cpid = fork();

    switch (cpid) {

        case -1:
            perror("fork failed");
            return 1;

        case 0:

            if (input_file != NULL) {

                int fd = open(input_file, O_RDONLY);

                if (fd < 0) {
                    perror("open input file");
                    exit(1);
                }

                dup2(fd, STDIN_FILENO);
                close(fd);
            }

            if (output_file != NULL) {

                int fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);

                if (fd < 0) {
                    perror("open output file");
                    exit(1);
                }

                dup2(fd, STDOUT_FILENO);
                close(fd);
            }

            execvp(args[0], args);
            perror("Command not found");
            exit(1);

        default:
            waitpid(cpid, &status, 0);
            return 0;
    }
}
