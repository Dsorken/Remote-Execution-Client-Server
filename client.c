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
    bool exit_client = false;

    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0) {
        perror("Socket Failed");
        exit(-1);
    }

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = inet_addr(ip_address);

    bool server_ready = false;
    while (!server_ready) {
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
        int retry = 0;
        ssize_t read_status = recv(client_socket, &buffer, sizeof(buffer) - 1, 0);
        while (read_status <= 0) {
            printf("Server Message Error\n");
            if (retry > POLL_MAX) {
                printf("Exiting\n");
                exit(-1);
            }
            retry++;
            read_status = recv(client_socket, &buffer, sizeof(buffer) - 1, 0);
        }
        printf("Server Response Recieved\n");

        //Process server response, if overloaded close current connection, try again in set amount of time
        //If connection established message recieved then server is ready to take request
        if (strcmp(buffer, OVERLOADED) == 0) {
            printf("Server Overloaded\n");
            printf("Waiting %d seconds before retrying\n", TIMEOUT);
        }
        else if (strcmp(buffer, CONNECTION_ESTABLISHED) == 0) {
            printf("Server Ready\n");
            server_ready = true;
        }
    }

    //Send request to server
    //If message_error recieved allow for request to be made again
    //Server will time out and close connection after retry limit reached
    bool invalid_message = true;
    while (invalid_message) {
        invalid_message = false;
        printf("Enter Request: ");
        char client_message[COMMAND_LENGTH + INT_LENGTH];
        char entered_char;
        int i = 0;
        while ((entered_char = getchar()) != '\n' && entered_char != EOF) {
            if (i < COMMAND_LENGTH + INT_LENGTH - 1) client_message[i] = entered_char;
            i++;
        }

        memset(buffer, 0, sizeof(buffer));
        ssize_t send_status = send(client_socket, client_message, strlen(client_message), 0);
        if (send_status < 0) {
            printf("Send Failed");
            exit(-1);
        }

        int retry = 0;
        ssize_t server_status = recv(client_socket, &buffer, sizeof(buffer) - 1, 0);
        while (server_status <= 0) {
            printf("Server Message Error\n");
            if (retry > POLL_MAX) {
                printf("Exiting\n");
                exit(-1);
            }
            retry++;
            server_status = recv(client_socket, &buffer, sizeof(buffer) - 1, 0);
        }

        if (strcmp(buffer, MESSAGE_ERROR) == 0) {
            printf("Server Message Error\n");
            invalid_message = true;
        }

        //Recieve server response, if desired response recieved close connection
        //If invalid_message recieved restart request process
        else if (strcmp(buffer, MESSAGE_PROCCESSING) == 0) {
            memset(buffer, 0, sizeof(buffer));
            printf("Server Processing Request...\n");
            retry = 0;
            ssize_t server_status = recv(client_socket, &buffer, sizeof(buffer) - 1, 0);
            while (server_status <= 0) {
                printf("Server Message Error\n");
                if (retry > POLL_MAX) {
                    printf("Exiting\n");
                    exit(-1);
                }
                retry++;
                server_status = recv(client_socket, &buffer, sizeof(buffer) - 1, 0);
            }
            if (strcmp(buffer, INVALID_MESSAGE) == 0) {
                printf("Invalid Request\n");
                invalid_message = true;
            } 
            else if (strcmp(buffer, REQUEST_PROCESSED) == 0) printf("Request Processed\n");
            else printf("Calculated Value: %s\n", buffer);
        }
    }

    printf("Closing Connection\n");
    close(client_socket);
    return 0;
}   