#include <stdio.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#define PORT 43434

int main(int argc, char *argv[]) {
    //Create socket to listen on
    //bind socket to ip and port (43434)
    //for each connection create a thread to handle that connection
    struct sockaddr_in server_address, client_address;
    socklen_t client_address_length = sizeof(client_address);
    char buffer[1024] = {0};
    char *server_message = "Message Recieved";

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

    int client_socket = accept(server_socket, (struct sockaddr*) &client_address, &client_address_length);

    if (client_socket < 0) {
        perror("Accept Failed");
        exit(-1);
    }

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
    close(server_socket);
    return 0;
}