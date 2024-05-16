CC = gcc

all: server client

server: server.o client_queue.o
	$(CC) -o server server.o client_queue.o -pthread

server.o: server.c
	$(CC) -c server.c

client_queue.o: client_queue.c
	$(CC) -c client_queue.c

client: client.o
	$(CC) -o client client.o

client.o: client.c
	$(CC) -c client.c