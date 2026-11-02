#ifndef SHELL_H
#define SHELL_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

#include <readline/readline.h>
#include <readline/history.h>

#define MAX_LEN 512
#define MAXARGS 10
#define ARGLEN 30
#define MAX_JOBS 20
#define PROMPT "FCIT> "

typedef struct {
    pid_t pid;
    char command[MAX_LEN];
} Job;

char** tokenize(char* cmdline);
int execute(char** arglist, int background, char* command);

void store_history_command(char* cmd);
void show_history(void);
char* get_history(int number);

void add_job(pid_t pid, char* command);
void remove_job(pid_t pid);
void show_jobs(void);
void reap_jobs(void);

#endif
