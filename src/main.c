#include "shell.h"

int main() {

    char* cmdline;
    char** arglist;

    while ((cmdline = read_cmd(PROMPT, stdin)) != NULL) {

        if (cmdline[0] == '!' && cmdline[1] >= '0' && cmdline[1] <= '9') {

            int number = atoi(cmdline + 1);
            char* previous = get_history(number);

            if (previous == NULL) {
                printf("No such command in history.\n");
                free(cmdline);
                continue;
            }

            strcpy(cmdline, previous);
            printf("%s\n", cmdline);
        }

        if (cmdline[0] != '\0') {
            add_history(cmdline);
        }

        if ((arglist = tokenize(cmdline)) != NULL) {

            execute(arglist);

            for (int i = 0; arglist[i] != NULL; i++) {
                free(arglist[i]);
            }

            free(arglist);
        }

        free(cmdline);
    }

    printf("\nShell exited.\n");
    return 0;
}
