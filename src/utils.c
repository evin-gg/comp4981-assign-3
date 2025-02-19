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

int isBuiltin(char **args)
{
    int flag;

    if(strcmp(args[0], "cd") == 0 || strcmp(args[0], "type") == 0 || strcmp(args[0], "exit") == 0)
    {
        flag = 1;
    }

    else
    {
        flag = 0;
    }

    return flag;
}

void handleBuiltin(char **args, int cfd)
{
    if(strcmp(args[0], "cd") == 0)
    {
        if(args[1] == NULL)
        {
            send(cfd, "cd: No specified directory", CD_ERR, 0);
        }

        else
        {
            if(chdir(args[1]) == -1)
            {
                send(cfd, "cd: No specified directory", CD_ERR, 0);
            }
        }
    }

    else if(strcmp(args[0], "type") == 0)
    {
        printf("\nThe Type Arg: %s\n", args[1]);
        if(args[1] == NULL)
        {
            send(cfd, "type: missing argument\n", TYPE_ERR, 0);
        }
        else
        {
            char response[BUFFER_SIZE];
            snprintf(response, sizeof(response), "%s is a shell built-in\n", args[1]);
            send(cfd, response, strlen(response), 0);
        }
    }
}

void freeArgs(char **args)
{
    for(int i = 0; args[i] != NULL; i++)
    {
        free(args[i]);
    }
}
