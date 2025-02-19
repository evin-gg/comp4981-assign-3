#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/wait.h>


#define BUFFER_SIZE 1024
#define MAX_ARGS 10
#define CD_ERR 27
#define TYPE_ERR 24
#define DIR_SUCCESS 28

void tokenize(char buffer[BUFFER_SIZE], char *args[MAX_ARGS]);
void findDir(char *args[MAX_ARGS]);
int isBuiltin(char **args);
void handleBuiltin(char **args, int cfd);
void freeArgs(char **args);
// void forkCmd(char **args);
