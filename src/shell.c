#include "shell.h"

char my_history_list[20][MAX_LEN];
int history_count = 0;

void store_history_command(char* cmd) {

    if (history_count < 20) {
        strcpy(my_history_list[history_count], cmd);
        history_count++;
    }
    else {
        for (int i = 0; i < 19; i++) {
            strcpy(my_history_list[i], my_history_list[i + 1]);
        }

        strcpy(my_history_list[19], cmd);
    }
}

void show_history(void) {

    for (int i = 0; i < history_count; i++) {
        printf("%d %s\n", i + 1, my_history_list[i]);
    }
}

char* get_history(int number) {

    if (number < 1 || number > history_count) {
        return NULL;
    }

    return my_history_list[number - 1];
}

char** tokenize(char* cmdline) {

    if (cmdline == NULL || cmdline[0] == '\0' || cmdline[0] == '\n') {
        return NULL;
    }

    char** arglist = (char**)malloc(sizeof(char*) * (MAXARGS + 1));

    for (int i = 0; i < MAXARGS + 1; i++) {
        arglist[i] = (char*)malloc(sizeof(char) * ARGLEN);
        bzero(arglist[i], ARGLEN);
    }

    char* cp = cmdline;
    char* start;
    int len;
    int argnum = 0;

    while (*cp != '\0' && argnum < MAXARGS) {

        while (*cp == ' ' || *cp == '\t') {
            cp++;
        }

        if (*cp == '\0') {
            break;
        }

        /* Check whether the current character is an operator */
        if (*cp == '<' || *cp == '>' || *cp == '|') {

            arglist[argnum][0] = *cp;
            arglist[argnum][1] = '\0';

            cp++;
            argnum++;
            continue;
        }

        start = cp;
        len = 0;

        /* Read a word until space or an operator is found */
        while (*cp != '\0' && *cp != ' ' && *cp != '\t'
               && *cp != '<' && *cp != '>' && *cp != '|') {

            len++;
            cp++;
        }

        if (len >= ARGLEN) {
            len = ARGLEN - 1;
        }

        strncpy(arglist[argnum], start, len);
        arglist[argnum][len] = '\0';

        argnum++;
    }

    if (argnum == 0) {

        for (int i = 0; i < MAXARGS + 1; i++) {
            free(arglist[i]);
        }

        free(arglist);
        return NULL;
    }

    arglist[argnum] = NULL;
    return arglist;
}

Variable *variables = NULL;

void set_variable(char *name, char *value) {

    Variable *current = variables;

    while (current != NULL) {
        if (strcmp(current->name, name) == 0) {
            free(current->value);
            current->value = strdup(value);
            return;
        }

        current = current->next;
    }

    Variable *new_variable = malloc(sizeof(Variable));

    if (new_variable == NULL) {
        perror("malloc");
        return;
    }

    new_variable->name = strdup(name);
    new_variable->value = strdup(value);
    new_variable->next = variables;

    variables = new_variable;
}

char *get_variable(char *name) {

    Variable *current = variables;

    while (current != NULL) {
        if (strcmp(current->name, name) == 0) {
            return current->value;
        }

        current = current->next;
    }

    return NULL;
}

void show_variables(void) {

    Variable *current = variables;

    while (current != NULL) {
        printf("%s=%s\n", current->name, current->value);
        current = current->next;
    }
}

int handle_assignment(char *command) {

    char *equal = strchr(command, '=');

    if (equal == NULL || equal == command) {
        return 0;
    }

    int name_length = equal - command;

    if (!((command[0] >= 'A' && command[0] <= 'Z') ||
          (command[0] >= 'a' && command[0] <= 'z') ||
          command[0] == '_')) {
        return 0;
    }

    for (int i = 1; i < name_length; i++) {
        if (!((command[i] >= 'A' && command[i] <= 'Z') ||
              (command[i] >= 'a' && command[i] <= 'z') ||
              (command[i] >= '0' && command[i] <= '9') ||
              command[i] == '_')) {
            return 0;
        }
    }

    char name[MAX_LEN];
    char value[MAX_LEN];

    if (name_length >= MAX_LEN) {
        return 0;
    }

    strncpy(name, command, name_length);
    name[name_length] = '\0';

    strcpy(value, equal + 1);

    int length = strlen(value);

    if (length >= 2 &&
        ((value[0] == '"' && value[length - 1] == '"') ||
         (value[0] == '\'' && value[length - 1] == '\''))) {

        value[length - 1] = '\0';
        memmove(value, value + 1, length - 1);
    }

    set_variable(name, value);

    return 1;
}

void expand_variables(char **arglist) {

    for (int i = 0; arglist[i] != NULL; i++) {

        if (arglist[i][0] == '$') {

            char *value = get_variable(arglist[i] + 1);

            if (value != NULL) {
                free(arglist[i]);
                arglist[i] = strdup(value);
            }
        }
    }
}
