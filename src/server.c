#include "../include/setup.h"
#include "../include/utils.h"
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUFFER_SIZE 1024
#define MAX_ARGS 10

typedef struct data_t
{
    int                fd;
    int                cfd;
    struct sockaddr_in addr;
    socklen_t          addr_len;
} data_t;

static volatile sig_atomic_t running = 1;    // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

static void setup(data_t *d, char s[INET_ADDRSTRLEN]);
static void setup_sig_handler(void);
static void sig_handler(int sig);
static void cleanup(const data_t *d);

int main(void)
{
    data_t data = {0};
    char   addr_str[INET_ADDRSTRLEN];
    int    retval = EXIT_SUCCESS;
    pid_t  p;

    setup(&data, addr_str);

    while(running)
    {
        data.cfd = accept(data.fd, NULL, 0);
        if(data.cfd < 0)
        {
            retval = EXIT_FAILURE;
            goto cleanup;
        }

        p = fork();
        if(p < 0)
        {
            perror("Fork");
        }

        else if(p == 0)
        {
            printf("Client connected\n");
            while(1)
            {
                char    buffer[BUFFER_SIZE];
                char   *args[MAX_ARGS];
                ssize_t bytes_received;

                memset(buffer, 0, BUFFER_SIZE);
                bytes_received = recv(data.cfd, buffer, BUFFER_SIZE, 0);
                if(bytes_received <= 0)
                {
                    printf("Client disconnected.\n");
                    close(data.cfd);
                    break;
                }

                tokenize(buffer, args);
                find_dir(args);

                if(is_built_in(args))
                {
                    handle_built_in(args, data.cfd);
                }

                else
                {
                    pid_t exec_p;

                    exec_p = fork();
                    if(exec_p < 0)
                    {
                        perror("exevc");
                    }

                    else if(exec_p == 0)
                    {
                        dup2(data.cfd, STDOUT_FILENO);
                        if(execv(args[0], args) == -1)
                        {
                            perror("execv");
                            send(data.cfd, "No command found", NO_CMD, 0);
                            _exit(1);
                        }

                        close(data.cfd);
                        _exit(0);
                    }

                    else
                    {
                        waitpid(exec_p, NULL, 0);
                    }
                }

                free_args(args);
            }
            _exit(0);
        }
    }

cleanup:
    cleanup(&data);
    exit(retval);
}

static void setup(data_t *d, char s[INET_ADDRSTRLEN])
{
    find_address(&(d->addr.sin_addr.s_addr), s);
    d->addr_len = sizeof(struct sockaddr_in);
    d->fd       = setup_server(&(d->addr));
    find_port(&(d->addr), s);
    setup_sig_handler();
}

static void setup_sig_handler(void)
{
    struct sigaction sa;

    memset(&sa, 0, sizeof(sa));
#if defined(__clang__)
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wdisabled-macro-expansion"
#endif
    sa.sa_handler = sig_handler;
#if defined(__clang__)
    #pragma clang diagnostic pop
#endif
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    if(sigaction(SIGINT, &sa, NULL) == -1)
    {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

static void sig_handler(int sig)
{
    const char *message = "\nSIGINT received. Server shutting down.\n";
    write(1, message, strlen(message));
    running = 0;
}

#pragma GCC diagnostic pop

static void cleanup(const data_t *d)
{
    close(d->fd);
    if(d->cfd > 0)
    {
        close(d->cfd);
    }
}
