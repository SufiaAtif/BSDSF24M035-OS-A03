#include "shell.h"

int execute(char* arglist[]) {

    // Built-in command: exit
    if (strcmp(arglist[0], "exit") == 0) {
        exit(0);
    }

    // Built-in command: cd
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

    // Built-in command: help
    if (strcmp(arglist[0], "help") == 0) {
        printf("Available commands:\n");
        printf("exit - Exit the shell\n");
        printf("cd - Change directory\n");
        printf("help - Display available commands\n");
        printf("jobs - Display job information\n");

        return 0;
    }

    // Built-in command: jobs
    if (strcmp(arglist[0], "jobs") == 0) {
        printf("Job control is not implemented yet.\n");
        return 0;
    }

    // External commands
    int status;
    int cpid = fork();

    switch (cpid) {
        case -1:
            perror("fork failed");
            exit(1);

        case 0:
            execvp(arglist[0], arglist);
            perror("Command not found");
            exit(1);

        default:
            waitpid(cpid, &status, 0);
            return 0;
    }
}
