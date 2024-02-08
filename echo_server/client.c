#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

// #define SERVER_IP "127.0.0.1"   // Server IP address
// #define SERVER_PORT 12345       // Server port number
#define BUFFER_SIZE 1024

int main(int argc, char ** argv) {
    int client_socket;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];

    if (argc != 3) {
        perror("usage: client <IP> <port>\n");
        exit(EXIT_FAILURE);
    }
    char * SERVER_IP = argv[1];
    int SERVER_PORT = atoi(argv[2]);

    // Create a TCP/IP socket
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Initialize server address struct
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
        perror("inet_pton");
        exit(EXIT_FAILURE);
    }

    // Connect to the server
    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("connect");
        exit(EXIT_FAILURE);
    }
    printf("Connected to %s:%d\n", SERVER_IP, SERVER_PORT);

    // Main loop
    while (1) {
        // Get user input
        printf("Enter message (or 'quit' to exit): ");
        if (fgets(buffer, BUFFER_SIZE, stdin) == NULL) {
            perror("fgets");
            break;
        }
        buffer[strcspn(buffer, "\n")] = '\0'; // Remove trailing newline

        // Send the message to the server
        if (send(client_socket, buffer, strlen(buffer), 0) == -1) {
            perror("send");
            break;
        }

        // Check if user wants to quit
        if (strcmp(buffer, "quit") == 0) {
            printf("Exiting...\n");
            break;
        }

        // Receive response from the server
        int num_bytes_recv;
        if ((num_bytes_recv = recv(client_socket, buffer, BUFFER_SIZE - 1, 0)) == -1) {
            perror("recv");
            break;
        }
        buffer[num_bytes_recv] = '\0'; // Null-terminate the received data

        // Print server response
        printf("Server response: %s\n", buffer);
    }

    // Close the socket
    close(client_socket);

    return 0;
}
