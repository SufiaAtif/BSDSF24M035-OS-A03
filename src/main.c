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

char *trim(char *text) {
    int start = 0;
    while (text[start] == ' ' || text[start] == '\t') {
        start++;
    }
    if (start > 0) {
        memmove(text, text + start, strlen(text + start) + 1);
    }
    int length = strlen(text);
    while (length > 0 && (text[length - 1] == ' ' || text[length - 1] == '\t' || text[length - 1] == '\n')) {
        text[length - 1] = '\0';
        length--;
    }
    return text;
}

int run_condition(char *condition) {
    char **args = tokenize(condition);
    int status;
    pid_t pid;

    if (args == NULL || args[0] == NULL) {
        printf("Invalid if condition.\n");
        return 1;
    }

    pid = fork();

    if (pid == -1) {
        perror("fork");
        return 1;
    }

    if (pid == 0) {
        execvp(args[0], args);
        perror("Command not found");
        exit(1);
    }

    waitpid(pid, &status, 0);

    for (int i = 0; args[i] != NULL; i++) {
        free(args[i]);
    }

    free(args);

    if (WIFEXITED(status)) {
        return WEXITSTATUS(status);
    }

    return 1;
}

void run_block(char block[50][MAX_LEN], int count) {
    for (int i = 0; i < count; i++) {
        char command[MAX_LEN];

        strcpy(command, block[i]);

        char **args = tokenize(command);

        if (args != NULL) {
            execute(args, 0, block[i]);

            for (int j = 0; args[j] != NULL; j++) {
                free(args[j]);
            }

            free(args);
        }
    }
}

void handle_if(char *first_line) {
    char condition[MAX_LEN];
    char then_commands[50][MAX_LEN];
    char else_commands[50][MAX_LEN];

    int then_count = 0;
    int else_count = 0;
    int in_else = 0;
    int found_then = 0;
    int found_fi = 0;

    strcpy(condition, trim(first_line + 2));

    char *line;

    while (1) {
        line = readline("> ");

        if (line == NULL) {
            printf("\nError: incomplete if statement.\n");
            return;
        }

        line = trim(line);

        if (strcmp(line, "then") == 0) {
            found_then = 1;
        }
        else if (strcmp(line, "else") == 0) {
            if (!found_then) {
                printf("Error: then is missing.\n");
                free(line);
                return;
            }

            in_else = 1;
        }
        else if (strcmp(line, "fi") == 0) {
            found_fi = 1;
            free(line);
            break;
        }
        else if (line[0] != '\0') {
            if (!found_then) {
                printf("Error: write then before commands.\n");
                free(line);
                return;
            }

            if (in_else) {
                if (else_count < 50) {
                    strcpy(else_commands[else_count], line);
                    else_count++;
                }
            }
            else {
                if (then_count < 50) {
                    strcpy(then_commands[then_count], line);
                    then_count++;
                }
            }
        }

        free(line);
    }

    if (!found_then || !found_fi) {
        printf("Error: incomplete if statement.\n");
        return;
    }

    int result = run_condition(condition);

    if (result == 0) {
        run_block(then_commands, then_count);
    }
    else {
        run_block(else_commands, else_count);
    }
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

        char *checked_command = trim(cmdline);

        if (strncmp(checked_command, "if ", 3) == 0) {
            handle_if(checked_command);
            free(cmdline);
            continue;
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
