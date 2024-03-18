#define _GNU_SOURCE
#include "net.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>

#define PORT 1029
#define MAX_REQUEST_SIZE 1024

void send_error_response(int nfd, char *error_code)
{
    char response[MAX_REQUEST_SIZE];
    snprintf(response, MAX_REQUEST_SIZE, "HTTP/1.0 %s\r\n", error_code);
    write(nfd, response, strlen(response));
}

void handle_request(int nfd)
{
   FILE *network = fdopen(nfd, "rb");
   char *line = NULL;
   size_t size;
   ssize_t num;

   if (network == NULL)
   {
      perror("fdopen");
      close(nfd);
      return;
   }

   while ((num = getline(&line, &size, network)) >= 0) {
        char *type = strtok(line, " ");
        char *filename = strtok(NULL, " ");
        strtok(NULL, " ");

        if (strcmp(type, "GET") == 0) {
            int file_fd = open(filename + 1, O_RDONLY);
            
            if (file_fd == -1) {
                send_error_response(nfd, "404 Not Found");
                close(nfd);
                return;
            }

            struct stat file_stat;
            if (fstat(file_fd, &file_stat) == -1) {
                perror("fstat");
                close(file_fd);
                close(nfd);
                return;
            }

            char header[MAX_REQUEST_SIZE];
            snprintf(header, MAX_REQUEST_SIZE, "HTTP/1.0 200 OK\r\nContent-Type: text/html\r\nContent-Length: %ld\r\n\r\n", file_stat.st_size);
            write(nfd, header, strlen(header));

            char buffer[MAX_REQUEST_SIZE];
            ssize_t bytes_read;
            while ((bytes_read = read(file_fd, buffer, sizeof(buffer))) > 0) {
               write(nfd, buffer, bytes_read);
            }

            close(file_fd);
        }
        else if (strcmp(type, "HEAD") == 0)
        {

            int file_fd = open(filename + 1, O_RDONLY);
            if (file_fd == -1)
            {
                send_error_response(nfd, "404 Not Found");
                close(nfd);
                return;
            }

            struct stat file_stat;
            if (fstat(file_fd, &file_stat) == -1)
            {
                perror("fstat");
                close(file_fd);
                close(nfd);
                return;
            }
            char header[MAX_REQUEST_SIZE];
            snprintf(header, MAX_REQUEST_SIZE, "HTTP/1.0 200 OK\r\nContent-Type: text/html\r\nContent-Length: %ld\r\n\r\n", file_stat.st_size);
            write(nfd, header, strlen(header));
            close(file_fd);
        }
         else
        {
            send_error_response(nfd, "501 Not Implemented");
            close(nfd);
            return;
        }
    }

    free(line);
    fclose(network);
}

void run_service(int fd)
{
    while (1) {
        int nfd = accept_connection(fd);
        if (nfd != -1) {
            pid_t pid = fork();
            if (pid == -1) {
                perror("fork");
                close(nfd);
            } else if (pid == 0) {
                printf("Connection established\n");
                handle_request(nfd);
                printf("Connection closed\n");
                close(nfd);
                exit(42);
            } else {
                close(nfd);
            }
        }
    }
}

int main(void)
{
   int fd = create_service(PORT);

   if (fd == -1)
   {
      perror(0);
      exit(1);
   }

   printf("listening on port: %d\n", PORT);
   while (1) {
        run_service(fd);
    }
   close(fd);

   return 0;
}
