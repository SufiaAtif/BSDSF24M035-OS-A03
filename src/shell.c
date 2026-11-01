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

        start = cp;
        len = 1;

        while (*++cp != '\0' && !(*cp == ' ' || *cp == '\t')) {
            len++;
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
