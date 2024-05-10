#include <stdio.h>
#include <stdio.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include "communication.h"

int main(int argc, char *argv[]) {
    char *ip_address = "127.0.0.1";
    if (argc > 1) {
        ip_address = argv[1];
    }
    struct sockaddr_in server_address;
    char buffer[1024] = {0};
    bool connected = false;
    bool exit = false;

    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0) {
        perror("Socket Failed");
        exit(-1);
    }

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = inet_addr(ip_address);

    while (!connected) {
        int server_connection = connect(client_socket, (struct sockaddr*) &server_address, sizeof(server_address));

        if (server_connection < 0) {
            printf("Connection Failed\n");
            printf("Would you like to try again? (y/n): ");
            char response;
            char entered_char;
            while ((entered_char = getchar()) != '\n' && entered_char != EOF) response = entered_char;
            if (response == 'n') {
                printf("Exiting\n");
                exit(0);
            }
        } 
        else connected = true;
    }
    printf("Connection Established\n");
    printf("Waiting for server response...\n");
    //Server response loop
    //If server message not able to be read loop until retry limit reached, then exit

    ssize_t read_status = recv(client_socket, &buffer, sizeof(buffer) - 1, 0);
    if (read_status < 0) {
        perror("Read Failed");
        exit(-1);
    }

    //Process server response, if overloaded close current connection, try again in set amount of time
    //If connection established message recieved then server is ready to take request
    
    //Send request to server
    //If message_error recieved allow for request to be made again
    //Server will time out and close connection after retry limit reached

    //Recieve server response, if desired response recieved close connection
    //If invalid_message recieved restart request process

    ssize_t send_status = send(client_socket, client_message, strlen(client_message), 0);
    if (send_status < 0) {
        printf("Send Failed");
        exit(-1);
    }
    
    
    ssize_t read_status = recv(client_socket, &buffer, sizeof(buffer) - 1, 0);
    if (read_status < 0) {
        perror("Read Failed");
        exit(-1);
    }

    printf("Server Message: %s\n", buffer);
    close(client_socket);
    return 0;
}   