CC = gcc

server: server.o
	$(CC) -o server server.o

server.o: server.c
	$(CC) -c server.c

client: client.o
	$(CC) -o client client.o

client.o: client.c
	$(CC) -c client.c