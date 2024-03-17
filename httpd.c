#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MIN_ARGS 2
#define MAX_ARGS 3
#define SERVER_ARG_IDX 1

#define USAGE_STRING "usage: %s <server address> <port>\n"

void validate_arguments(int argc, char *argv[])
{
    if (argc < MIN_ARGS || argc > MAX_ARGS)
    {
        fprintf(stderr, USAGE_STRING, argv[0]);
        exit(EXIT_FAILURE);
    }
}

void send_request(int fd)
{
    char *line = NULL;
    size_t size;
    ssize_t num;

    while ((num = getline(&line, &size, stdin)) >= 0)
    {
        write(fd, line, num);

        char buffer[100];
        read(fd, buffer, sizeof(buffer));
        printf("echo: %s", buffer);
        memset(buffer, 0, sizeof(buffer));
    }

    free(line);
}

int connect_to_server(struct hostent *host_entry, int port)
{
    int fd;
    struct sockaddr_in their_addr;

    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("socket");
        return -1;
    }

    their_addr.sin_family = AF_INET;
    their_addr.sin_port = htons(port);
    their_addr.sin_addr = *((struct in_addr *)host_entry->h_addr);

    if (connect(fd, (struct sockaddr *)&their_addr, sizeof(struct sockaddr)) == -1)
    {
        perror("connect");
        close(fd);
        return -1;
    }

    return fd;
}

int main(int argc, char *argv[])
{
    validate_arguments(argc, argv);
    struct hostent *host_entry = gethostbyname(argv[SERVER_ARG_IDX]);
    if (host_entry == NULL)
    {
        fprintf(stderr, "Cannot resolve hostname: %s\n", argv[SERVER_ARG_IDX]);
        exit(EXIT_FAILURE);
    }

    int port = atoi(argv[2]);
    int fd = connect_to_server(host_entry, port);
    if (fd != -1)
    {
        send_request(fd);
        close(fd);
    }

    return 0;
}

