CC = gcc
CFLAGS = -Wall -std=c99 -pedantic
CLIENT = httpd
CLIENT_OBJS = httpd.o
SERVER = server
SERVER_OBJS = server.o net.o
PROGS = $(CLIENT) $(SERVER)

all : $(PROGS)

$(CLIENT) : $(CLIENT_OBJS)
	$(CC) $(CFLAGS) -o $(CLIENT) $(CLIENT_OBJS)

httpd.o : httpd.c
	$(CC) $(CFLAGS) -c httpd.c

$(SERVER) : $(SERVER_OBJS)
	$(CC) $(CFLAGS) -o $(SERVER) $(SERVER_OBJS)

server.o : server.c net.h
	$(CC) $(CFLAGS) -c server.c

net.o : net.c net.h
	$(CC) $(CFLAGS) -c net.c


clean :
	rm *.o $(PROGS) core

