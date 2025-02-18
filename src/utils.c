#include "../include/utils.h"

void tokenize(char buffer[BUFFER_SIZE], char *args[MAX_ARGS])
{
    int         token_counter;
    const char *token;
    char       *saveptr;

    token_counter = 0;

    token = strtok_r(buffer, " ", &saveptr);
    while(token != NULL && token_counter < MAX_ARGS - 1)
    {
        args[token_counter] = strdup(token);
        token_counter++;
        token = strtok_r(NULL, " ", &saveptr);
    }
    args[token_counter] = NULL;
}

void findDir(char *args[MAX_ARGS])
{
    if(args[0] == NULL)
    {
        return;
    }

    if(strcmp(args[0], "ls") == 0)
    {
        free(args[0]);
        args[0] = strdup("/bin/ls");
    }
    else if(strcmp(args[0], "pwd") == 0)
    {
        free(args[0]);
        args[0] = strdup("/usr/bin/pwd");
    }
    else if(strcmp(args[0], "echo") == 0)
    {
        free(args[0]);
        args[0] = strdup("/usr/bin/echo");
    }
}
