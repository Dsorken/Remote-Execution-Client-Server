#include <stdio.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include "client_queue.h"
#define INT_LENGTH 12
#define BUFFER_LENGTH 1024
#define COMMAND_LENGTH 5
#define PORT 43434
#define THREAD_MAX 10
#define QUEUE_MAX 200
#define POLL_MAX 10
#define CONNECTION_ESTABLISHED "G"
#define OVERLOADED "O"
#define MESSAGE_PROCCESSING "P"
#define REQUEST_PROCESSED "PC"
#define MESSAGE_ERROR "E"
#define INVALID_MESSAGE "I"
#define JOB1 "J1"
#define JOB2 "J2"

client_queue *queue;
sem_t queue_semaphore;

void handle_request(int client_socket) {
    char buffer[BUFFER_LENGTH] = {0};
    bool job_complete = false;
    
    while (!job_complete) {
        //Continue to process client if invalid message submitted
        int retry = 0;
        ssize_t client_request = recv(client_socket, &buffer, sizeof(buffer) - 1, 0);
        while (client_request < 0) {
            //Allow client to resend message until correct or until retry count maxed out
            //Unable to get client message
            perror("Client Message Error");
            if (retry > POLL_MAX) return;

            //Respond with error
            ssize_t server_response = send(client_socket, MESSAGE_ERROR, strlen(MESSAGE_ERROR), 0);
            if (server_response < 0) {
                //If server message as well connection is broken so close connection
                perror("Server Message Error");
                close(client_socket);
                return;
            } 
            else client_request = recv(client_socket, &buffer, sizeof(buffer) - 1, 0);
            retry++;
        }
        printf("Client %d message: %s\n", client_socket, buffer);

        ssize_t server_response = send(client_socket, MESSAGE_PROCCESSING, strlen(MESSAGE_PROCCESSING), 0);
        if (server_response < 0) {
            perror("Server Message Error");
            close(client_socket);
            return;
        }
        printf("Proccessing message sent to %d\n", client_socket);

        //Get the command from buffer
        char command[COMMAND_LENGTH] = {0};
        strncpy(command, buffer, 4);

        if (strcmp(command, JOB1) == 0) {
            job_one();
            job_complete = true;
        }
        else if (strcmp(command, JOB2) == 0) {
            //get value following command from buffer, should be an integer
            char value_str[INT_LENGTH] = {0};
            memcpy(value_str, buffer + COMMAND_LENGTH, INT_LENGTH - 1);

            //Convert to integer and run calculation
            int value = atoi(value_str);
            value = job_two(value);

            //Convert calculated value back to string and send to client
            snprintf(value_str, sizeof(value_str), "%d", value);
            ssize_t server_response = send(client_socket, value_str, strlen(value_str), 0);
            if (server_response < 0) {
                perror("Server Message Error");
                close(client_socket);
                return; 
            }
            job_complete = true;

        } else {
            //Send error message
            ssize_t server_response = send(client_socket, INVALID_MESSAGE, strlen(INVALID_MESSAGE), 0);
            if (server_response < 0) {
                perror("Server Message Error");
                close(client_socket);
                return;
            }
        }
    }

    close(client_socket);
}

void job_one() {
    //Simulate arbitrary process
    sleep(5);
}

int job_two(int value) {
    //Simulates an arbitrary calculation with a return value
    sleep(3);
    return value*2;
}

void *thread_worker() {
    //While program server not exited poll for queue
    //If queue has item lock queue semaphore, unqueue and then unlock queue 
    //Handle client request
    //When request handled close the connection and begin polling again
    while (1) {
        sem_wait(&queue_semaphore);
        int client_socket = client_queue_dequeue(queue);
        sem_post(&queue_semaphore);

        if (client_socket <= 0) {
            handle_request(client_socket);
        }
    }
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

    queue = client_queue_initialize(QUEUE_MAX);
    sem_init(&queue_semaphore, 0, 1);

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
        } else {
            //Add to queue
            sem_wait(&queue_semaphore);
            int queue_status = client_queue_enqueue(queue, client_socket);
            sem_post(&queue_semaphore);

            if (queue_status < 0) {
                //Let client know to wait
                ssize_t overload_response = send(client_socket, OVERLOADED, strlen(OVERLOADED), 0);
                close(client_socket);
            }
        }
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