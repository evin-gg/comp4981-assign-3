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
void find_dir(char *args[MAX_ARGS]);
int is_built_in(char **args);
int is_cmd(char **args);
void handle_built_in(char **args, int cfd);
void free_args(char **args);
void send_and_reset(int fd, char *buffer);
ssize_t receive_data(int socket_fd, char *buffer);
ssize_t read_input(char *buffer, size_t buffer_size);
