#include <string.h>
#include <stdlib.h>

#define BUFFER_SIZE 1024
#define MAX_ARGS 10

void tokenize(char buffer[BUFFER_SIZE], char *args[MAX_ARGS]);
void findDir(char *args[MAX_ARGS]);
