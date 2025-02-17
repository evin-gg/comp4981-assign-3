#include "../include/setup.h"
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

// #define MSGLEN 13
#define BUFSIZE 1024

typedef struct data_t
{
    int                fd;
    struct sockaddr_in addr;
    // socklen_t          addr_len;
    in_port_t port;
} data_t;

static volatile sig_atomic_t running = 1;    // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

static void setup(data_t *d, const char *addr_str);
static void setup_sig_handler(void);
static void sig_handler(int sig);
static void cleanup(const data_t *d);

int main(int argc, char *argv[])
{
    data_t data     = {0};
    char  *addr_str = NULL;
    char  *port_str = NULL;
    int    retval   = EXIT_SUCCESS;
    // char   buf[BUFSIZE + 1];
    // buf[BUFSIZE] = '\0';

    parse_args(argc, argv, &addr_str, &port_str, &data.port);
    setup(&data, addr_str);

    // read(data.fd, buf, MSGLEN);
    // write(STDOUT_FILENO, buf, MSGLEN + 1);

    /* Do stuff here */
    while(running)
    {
        char    buffer[BUFSIZE];
        ssize_t bytes_received;

        printf("> ");
        fgets(buffer, BUFSIZE, stdin);
        buffer[strcspn(buffer, "\n")] = 0;    // Remove newline

        if(strcmp(buffer, "exit") == 0)
        {
            break;
        }

        // Send message
        send(data.fd, buffer, strlen(buffer), 0);

        // Receive response
        memset(buffer, 0, BUFSIZE);
        bytes_received = recv(data.fd, buffer, BUFSIZE, 0);
        if(bytes_received <= 0)
        {
            printf("Server disconnected.\n");
            break;
        }

        printf("Server response: %s\n", buffer);
    }

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"     // Example warning suppression
#pragma GCC diagnostic ignored "-Wunused-function"     // If needed
#pragma GCC diagnostic ignored "-Wunreachable-code"    // If needed'
    cleanup(&data);
    exit(retval);

#pragma GCC diagnostic pop
}

static void setup(data_t *d, const char *addr_str)
{
    d->fd = setup_client(&d->addr, addr_str, d->port);
    setup_sig_handler();
}

/* Pairs SIGINT with sig_handler */
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

/* Write to stdout a shutdown message and set exit_flag to end while loop in main */
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
}
