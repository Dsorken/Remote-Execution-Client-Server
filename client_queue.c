#include "client_queue.h"

client_queue* client_queue_initialize(int max_size) {
    client_queue *queue = (client_queue*) malloc(sizeof(client_queue));
    queue->front = -1;
    queue->back = -1;
    queue->max_size = max_size;
    queue->connections = (int**) malloc(max_size*sizeof(int*));
    for (int i = 0; i < max_size; i++) queue->connections[i] = (int*) malloc(sizeof(int));
    return queue;
}

bool client_queue_isfull(client_queue *queue) {
    if (queue->front == queue->back + 1) return true;
    return false;
}

bool client_queue_isempty(client_queue *queue) {
    if (queue->front == -1) return true;
    return false;
}

int client_queue_enqueue(client_queue *queue, int connection) {
    if (client_queue_isfull(queue)) return -1;
    if (queue->front == -1) queue->front = queue->back = 0;
    else queue->back = (queue->back + 1) % queue->max_size;
    *(queue->connections[queue->back]) = connection;
    return 0;
}

int client_queue_dequeue(client_queue *queue) {
    if (client_queue_isempty(queue)) return -1;
    int connection = *(queue->connections[queue->front]);
    if (queue->front == queue->back) queue->front = queue->back = -1;
    else queue->front = (queue->front + 1) % queue->max_size;
    return connection;
}

void client_queue_destroy(client_queue *queue) {
    for (int i = 0; i < queue->max_size; i++) free(queue->connections[i]);
    free(queue->connections);
    free(queue);
}