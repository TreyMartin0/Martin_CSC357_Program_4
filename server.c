#define _GNU_SOURCE
#include "net.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define PORT 1029


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

   while ((num = getline(&line, &size, network)) >= 0)
   {
      write(nfd, line, num);
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
