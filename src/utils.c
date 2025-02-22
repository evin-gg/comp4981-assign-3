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

void find_dir(char *args[MAX_ARGS])
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

    if(strcmp(args[0], "cat") == 0)
    {
        free(args[0]);
        args[0] = strdup("/bin/cat");
    }
}

int is_built_in(char **args)
{
    int flag;

    if(strcmp(args[0], "cd") == 0 || strcmp(args[0], "type") == 0 || strcmp(args[0], "exit") == 0 || strcmp(args[0], "pwd") == 0 || strcmp(args[0], "echo") == 0)
    {
        flag = 1;
    }

    else
    {
        flag = 0;
    }

    return flag;
}

int is_cmd(char **args)
{
    int flag;

    if(strcmp(args[1], "cd") == 0 || strcmp(args[1], "type") == 0 || strcmp(args[1], "exit") == 0 || strcmp(args[1], "ls") == 0 || strcmp(args[1], "pwd") == 0 || strcmp(args[1], "echo") == 0)
    {
        flag = 1;
    }

    else
    {
        flag = 0;
    }
    return flag;
}

void handle_built_in(char **args, int cfd)
{
    if(strcmp(args[0], "cd") == 0)
    {
        if(args[1] == NULL)
        {
            send(cfd, "cd: No specified directory", CD_ERR, 0);
            return;
        }

        if(args[2] == NULL)
        {
            if(chdir(args[1]) == -1)
            {
                send(cfd, "cd: No specified directory", CD_ERR, 0);
            }
            send(cfd, "Successful directory change", DIR_SUCCESS, 0);
        }

        else
        {
            send(cfd, "Too many arguments", MANY_ARGS, 0);
        }
    }

    else if(strcmp(args[0], "type") == 0)
    {
        if(args[1] == NULL || is_cmd(args) == 0)
        {
            send(cfd, "type: not found\n", TYPE_ERR, 0);
        }
        else
        {
            char response[BUFFER_SIZE];
            snprintf(response, sizeof(response), "%s is a shell built-in\n", args[1]);
            send(cfd, response, strlen(response), 0);
        }
    }

    else if(strcmp(args[0], "pwd") == 0)
    {
        char pwd[BUFFER_SIZE];

        if(getcwd(pwd, BUFFER_SIZE) != NULL)
        {
            send(cfd, pwd, BUFFER_SIZE, 0);
        }

        else
        {
            perror("getcwd");
        }
    }

    else if(strcmp(args[0], "echo") == 0)
    {
        if(args[1] == NULL)
        {
            send(cfd, "echo requires arguments", ECHO_ERR, 0);
        }

        else
        {
            char   buffer[BUFFER_SIZE];
            size_t current_len;

            memset(buffer, 0, BUFFER_SIZE);

            current_len = 0;
            for(int i = 1; i < MAX_ARGS && args[i] != NULL; i++)
            {
                current_len += strlcat(buffer + current_len, args[i], BUFFER_SIZE - current_len);

                if(args[i + 1] != NULL)
                {
                    current_len += strlcat(buffer + current_len, " ", BUFFER_SIZE - current_len);
                }
            }
            send(cfd, buffer, current_len, 0);
        }
    }

    else if(strcmp(args[0], "exit") == 0)
    {
        send(cfd, "Exiting shell...", EXIT_SHELL, 0);
        close(cfd);
        printf("Client disconnected.\n");
        _exit(0);
    }
}

void free_args(char **args)
{
    for(int i = 0; args[i] != NULL; i++)
    {
        free(args[i]);
    }
}

void send_and_reset(int fd, char *buffer)
{
    send(fd, buffer, strlen(buffer), 0);
    memset(buffer, 0, BUFFER_SIZE);
}

ssize_t receive_data(int socket_fd, char *buffer)
{
    ssize_t bytes_received;

    bytes_received = recv(socket_fd, buffer, BUFFER_SIZE, 0);

    if(bytes_received <= 0)
    {
        printf("Server disconnected or error receiving data.\n");
        return -1;
    }

    printf("%s\n", buffer);
    return bytes_received;
}

ssize_t read_input(char *buffer, size_t buffer_size)
{
    ssize_t input_bytes;

    input_bytes = read(STDIN_FILENO, buffer, buffer_size - 1);
    if(input_bytes < 0)
    {
        perror("read");
        return -1;
    }

    buffer[strcspn(buffer, "\n")] = 0;

    return input_bytes;
}
