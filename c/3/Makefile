CC = gcc

all: server client

server: server.o
	$(CC) -o $@ $^

client: client.o
	$(CC) -o $@ $^

server.o: server.c

client.o: client.c

.c.o:
	$(CC) -Wall -g -c $^
