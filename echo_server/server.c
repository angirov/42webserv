#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>
#include <arpa/inet.h>

#define LISTENQ 10
#define BUFFERSIZE 1024

typedef struct sockaddr SA;

int open_listenfd(int port) {
    int listenfd, optval=1;
    struct sockaddr_in serveraddr;

    // create a socket fd
    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        return -1;

    // eliminate addr already in use error
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR,
                    (const void *)&optval, sizeof(int)) < 0)
        return -1;

    // Listenfd is endpoint for all requests to port on ANY IP for this host
    bzero((char *) &serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons((unsigned short)port);
    if (bind(listenfd, (SA *) &serveraddr, sizeof(serveraddr)) < 0)
        return -1;

    if (listen(listenfd, LISTENQ) < 0)
        return -1;
    printf("Server listening on port %d...\n", port);
    return listenfd;
}

int main(int argc, char ** argv) {
    int listenfd, connfd, port, clientlen;
    struct sockaddr_in clientaddr;
    struct hostent *hp;
    char *haddr_ptr;
    char buffer[BUFFERSIZE];

    if (argc != 2) {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(0);
    }
    port = atoi(argv[1]);

    listenfd = open_listenfd(port);
    while (1) {
        clientlen = sizeof(clientaddr);
        if ((connfd = accept(listenfd, (SA *) &clientaddr, &clientlen)) < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        printf("Connection from %s:%d\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

        ssize_t num_bytes_recv;
        while ((num_bytes_recv = recv(connfd, buffer, BUFFERSIZE, 0)) > 0) {
            printf("Received %.*s\n", (int)num_bytes_recv, buffer);
            send(connfd, buffer, num_bytes_recv, 0);
        }

        if (num_bytes_recv == -1) {
            perror("recv");
            exit(EXIT_FAILURE);
        }

        close(connfd);
    }
    close(listenfd);
    return 0;
}