#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <errno.h>
#include <list>
#include <iostream>
#include <algorithm>
#include <iterator>

#define LISTENQ 10
#define BUFFERSIZE 1024

typedef struct sockaddr SA;

int open_listenfd(int port)
{
    int listenfd, optval = 1;
    struct sockaddr_in serveraddr;

    // create a socket fd
    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        return -1;

    // eliminate addr already in use error
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR,
                   (const void *)&optval, sizeof(int)) < 0)
        return -1;

    // Listenfd is endpoint for all requests to port on ANY IP for this host
    bzero((char *)&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons((unsigned short)port);
    if (bind(listenfd, (SA *)&serveraddr, sizeof(serveraddr)) < 0)
        return -1;

    if (listen(listenfd, LISTENQ) < 0)
        return -1;
    printf("Server listening on port %d...\n", port);
    return listenfd;
}

void cout_list(std::list<int> l)
{
    std::copy(std::begin(l), std::end(l), std::ostream_iterator<int>(std::cout, ", "));
    std::cout << "]" << std::endl;
}

int main(int argc, char **argv)
{
    int listenfd, connfd, port, clientlen;
    struct sockaddr_in clientaddr;
    struct hostent *hp;
    char *haddr_ptr;
    char buffer[BUFFERSIZE] = {0};
    std::list<int> conn_l;

    if (argc != 2)
    {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(0);
    }
    port = atoi(argv[1]);

    listenfd = open_listenfd(port);
    fcntl(listenfd, F_SETFL, O_NONBLOCK);
    while (1)
    {
        clientlen = sizeof(clientaddr);
        connfd = accept(listenfd, (SA *)&clientaddr, (socklen_t *)&clientlen);
        // https://stackoverflow.com/questions/7003234/
        if (connfd == -1)
        {
            if (errno == EWOULDBLOCK || errno == EAGAIN)
            {
                printf("Checked server socket... No connection attempt ...\n");
                sleep(1);
                // continue;
            }
            else
            {
                perror("accept");
                exit(EXIT_FAILURE);
            }
        }
        else
        {
            fcntl(connfd, F_SETFL, O_NONBLOCK);
            conn_l.push_back(connfd);
            std::cout << "Adding fd " << connfd << " to the list. New list: [";
            cout_list(conn_l);
            printf("Connection from %s:%d\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
        }

        for (std::list<int>::iterator it = conn_l.begin(); it != conn_l.end(); ++it)
        {
            std::cout << "Checked connection of fd " << *it << " out of [";
            cout_list(conn_l);
            ssize_t num_bytes_recv = 0;

            // while (printf("Checking connection...\n") && (num_bytes_recv = recv(*it, buffer, BUFFERSIZE, 0) > 0))
            num_bytes_recv = recv(*it, buffer, BUFFERSIZE, 0);
            if (num_bytes_recv > 0)
            {
                printf("Received %d bytes: %.*s  \n from fd %d\n", (int)num_bytes_recv, (int)num_bytes_recv, buffer, *it);
                buffer[num_bytes_recv] = '!';
                send(*it, buffer, num_bytes_recv + 1, 0);
            }
            if (num_bytes_recv == 0)
            {
                std::cout << "Got zero bytes == Peer has closed the connection gracefully\n";
                close(*it);
                std::cout << "Removing fd " << *it << std::endl;
                it = conn_l.erase(it);;
                std::cout << " from the list. New list: [";
                cout_list(conn_l);
                continue;
            }
            if (num_bytes_recv == -1)
            {
                if (errno == EWOULDBLOCK || errno == EAGAIN)
                {
                    std::cout << "Checked connection of fd " << *it << ": nothing to read\n";
                    sleep(2);
                    ;
                }
                else
                {
                    perror("recv");
                }
            }
        }
    }
    close(listenfd);
    return 0;
}