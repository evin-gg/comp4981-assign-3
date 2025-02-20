#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/wait.h>
// #include <limits.h>

#define BUFFER_SIZE 1024
#define MAX_ARGS 10
#define CD_ERR 27
#define TYPE_ERR 17
#define DIR_SUCCESS 28
#define NO_CMD 17
#define ECHO_ERR 24
#define EXIT_SHELL 17
#define MANY_ARGS 19

void tokenize(char buffer[BUFFER_SIZE], char *args[MAX_ARGS]);
void findDir(char *args[MAX_ARGS]);
int isBuiltin(char **args);
int isCmd(char **args);
void handleBuiltin(char **args, int cfd);
void freeArgs(char **args);
void sendAndReset(int fd, char *buffer);
ssize_t receiveData(int socket_fd, char *buffer);
ssize_t read_input(char *buffer, size_t buffer_size);
