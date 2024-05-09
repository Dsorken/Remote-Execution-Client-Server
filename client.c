#include <stdio.h>
#include <stdio.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define PORT 43434

int main(int argc, char *argv[]) {
    struct sockaddr_in server_address;
    char buffer[1024] = {0};
    char *client_message = "Client Message";

    int client_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (client_socket < 0) {
        perror("Socket Failed");
        exit(-1);
    }

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");

    int server_connection = connect(client_socket, (struct sockaddr*) &server_address, sizeof(server_address));

    if (server_connection < 0) {
        perror("Connection Failed");
        exit(-1);
    }

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