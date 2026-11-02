#include "shell.h"

char *commands[] = {
    "cd", "exit", "help", "jobs", "history",
    "ls", "pwd", "cat", "mkdir", "touch",
    "clear", "gcc", "make", NULL
};

char *command_generator(const char *text, int state) {
    static int index;
    static int length;

    if (state == 0) {
        index = 0;
        length = strlen(text);
    }

    while (commands[index] != NULL) {
        char *name = commands[index++];

        if (strncmp(name, text, length) == 0) {
            return strdup(name);
        }
    }

    return NULL;
}

char **my_completion(const char *text, int start, int end) {
    if (start == 0) {
        return rl_completion_matches(text, command_generator);
    }

    return rl_completion_matches(text, rl_filename_completion_function);
}

int main() {
    char* cmdline;
    char** arglist;
    char* command;
    char* next;
    char* end;
    int background;
    int length;

    rl_attempted_completion_function = my_completion;

    while (1) {

        reap_jobs();

        cmdline = readline(PROMPT);

        if (cmdline == NULL) {
            break;
        }

        if (cmdline[0] == '!' &&
            cmdline[1] >= '0' && cmdline[1] <= '9') {

            int number = atoi(cmdline + 1);
            char* previous = get_history(number);

            if (previous == NULL) {
                printf("No such command in history.\n");
                free(cmdline);
                continue;
            }

            free(cmdline);
            cmdline = strdup(previous);
            printf("%s\n", cmdline);
        }

        if (cmdline[0] != '\0') {
            add_history(cmdline);
            store_history_command(cmdline);
        }

        command = cmdline;

        while (command != NULL) {

            next = strchr(command, ';');

            if (next != NULL) {
                *next = '\0';
                next++;
            }

            length = strlen(command);

            if (length > 0) {
                end = command + length - 1;

                while (end >= command &&
                       (*end == ' ' || *end == '\t')) {
                    *end = '\0';
                    end--;
                }
            }

            background = 0;

            length = strlen(command);

            if (length > 0) {
                end = command + length - 1;

                if (*end == '&') {
                    background = 1;
                    *end = '\0';
                }
            }

            arglist = tokenize(command);

            if (arglist != NULL) {

                execute(arglist, background, command);

                for (int i = 0; arglist[i] != NULL; i++) {
                    free(arglist[i]);
                }

                free(arglist);
            }

            command = next;
        }

        free(cmdline);
    }

    printf("\nShell exited.\n");

    return 0;
}
