#ifndef CLIENT_QUEUE_H
#define CLIENT_QUEUE_H

#include <stdlib.h>
#include <stdbool.h>

typedef struct {
    int **connections;
    int front;
    int back;
    int max_size;
} client_queue;

client_queue* client_queue_initialize(int max_size);
bool client_queue_isfull(client_queue *queue);
bool client_queue_isempty(client_queue *queue);
int client_queue_enqueue(client_queue *queue, int connection);
int client_queue_dequeue(client_queue *queue);
void client_queue_destroy(client_queue *queue);

#endif