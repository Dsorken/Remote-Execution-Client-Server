#include <stdio.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#define PORT 43434
#define THREAD_MAX 10
#define QUEUE_MAX 200

sem_t queue_semaphore;

void *thread_worker(void *client_socket) {
    //While program server not exited poll for queue
    //If queue has item lock queue semaphore, unqueue and then unlock queue 
    //Handle client request
    //When request handled close the connection and begin polling again
}

void handle_request(int client_socket) {
    char buffer[1024] = {0};
    char *server_message = "Message Recieved";

    ssize_t client_request = recv(client_socket, &buffer, sizeof(buffer) - 1, 0);
    if (client_request < 0) {
        perror("Client Message Error");
    }
    printf("Client Message: %s\n", buffer);
        
    ssize_t server_response = send(client_socket, server_message, strlen(server_message), 0);
    if (server_response < 0) {
        perror("Server Message Error");
    }
    printf("Message Sent\n");

    close(client_socket);
}

void spawn(pthread_t *threads[THREAD_MAX]) {
    for (int i = 0; i < THREAD_MAX; i++) {
        pthread_create(*threads[i], NULL, thread_worker, NULL);
    }
}

int main(int argc, char *argv[]) {
    //for each connection create a thread to handle that connection
    pthread_t threads[THREAD_MAX];
    spawn(threads);

    //Set up socket to listen on
    struct sockaddr_in server_address, client_address;
    socklen_t client_address_length = sizeof(client_address);

    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 1) {
        perror("Socket Failed");
        exit(-1);
    }

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);

    int bind_status = bind(server_socket, (struct sockaddr*) &server_address, sizeof(server_address));
    if (bind_status < 0) {
        perror("Bind Failed");
        exit(-1);
    }

    int listen_status = listen(server_socket, 10);
    if (listen_status < 0) {
        perror("Listen Failed");
        exit(-1);
    }

    while (1) {
        int client_socket = accept(server_socket, (struct sockaddr*) &client_address, &client_address_length);

        if (client_socket < 0) {
            perror("Accept Failed");
            exit(-1);
        }
        
        //Add to queue
    }

    for (int i = 0; i < THREAD_MAX; i++) {
        pthread_join(threads[i], NULL);
    }

    close(server_socket);
    return 0;
}

/*
Create a set of threads up to an arbitrary maximum
Send each new connection to be handled by those threads
Keep track of the amount of threads active
If the thread maximimum is reached start queueing the connection and request associated
Create an arbitrary queue maximum
If the max queue limit is reached send a cooldown timer to the client to let it wait for some threads to free up
Once a thread is free have it recieve from the queue and handle the request
If the queue is empty have the threads pick up directly from main()

*/