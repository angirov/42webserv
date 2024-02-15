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
#include <map>
#include <sys/select.h>
#include <sstream>

#define LISTENQ 10
#define BUFFERSIZE 1000

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
    std::ostream_iterator<int> out_it(std::cout, ", ");
    for(std::list<int>::iterator it = l.begin(); it != l.end(); ++it)
    {
        *out_it++ = *it;
    }
    std::cout << "]" << std::endl;
}

void run(int port) {
    int listenfd, connfd, clientlen;
    struct sockaddr_in clientaddr;
    char buffer[BUFFERSIZE] = {0};
    std::string httpResponse = "HTTP/1.1 200 OK\r\n"
                                "Content-Type: text/plain\r\n"
                                "Transfer-Encoding: chunked\r\n"
                                "\r\n";
    std::list<int> conn_l;
    fd_set fds_listen;
    fd_set fds_listen_ret;
    FD_ZERO(&fds_listen);
    FD_ZERO(&fds_listen_ret);
    // fd_set fds_write;

    struct timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;

    std::map<int, std::string> requests;



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
            FD_SET(connfd, &fds_listen);
            if (FD_ISSET(connfd, &fds_listen)) {
                std::cout << "DEBUG: Checked if ISSET for the new client socket: TRUE\n";
            } else {
                std::cout << "DEBUG: Checked if ISSET for the new client socket: FALSE\n";
            };
            requests[connfd] = "";
            std::cout << "Adding fd " << connfd << " to the list. New list: [";
            cout_list(conn_l);
            printf("Connection from %s:%d\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
        }

        std::cout << ">>> DEBUG: before select" << std::endl;
        fds_listen_ret = fds_listen;
        int ret;
        ret = select(1024, &fds_listen_ret, NULL, NULL, &tv); // TODO: max has to be calculated
        std::cout << ">>> DEBUG: after select: returned: " << ret << std::endl;

        if (ret > 0)
        {
            for (std::list<int>::iterator it = conn_l.begin(); it != conn_l.end(); ++it)
            {
                std::cout << "Checking fd " << *it;
                ssize_t num_bytes_recv = 0;
                if (!FD_ISSET(*it, &fds_listen_ret))
                {
                    std::cout << " --> NOTHING to read\n";
                    continue;
                }
                std::cout << " --> READY to read\n";
                num_bytes_recv = recv(*it, buffer, BUFFERSIZE, 0);
                if (num_bytes_recv > 0)
                {
                    printf("Received %d bytes: %.*s from fd %d\n", (int)num_bytes_recv, (int)num_bytes_recv, buffer, *it);
                    requests[*it] += std::string(buffer, num_bytes_recv);
                }
                if (num_bytes_recv == 0)
                {
                    std::cout << "Got zero bytes == Peer has closed the connection gracefully\n";
                    close(*it);
                    std::cout << "Removing fd " << *it << " from the list. "; 
                    it = conn_l.erase(it);
                    FD_CLR(*it, &fds_listen);
                    std::cout << "New list: [";
                    cout_list(conn_l);
                    continue;
                }
                if (num_bytes_recv == -1)
                {
                    // if (errno == EWOULDBLOCK || errno == EAGAIN)
                    // {
                    //     std::cout << "Checked connection of fd " << *it << ": nothing to read\n";
                    //     if (requests[*it].size() > 0)
                    //     {
                    //         requests[*it] = "RESPONCE: " + requests[*it] + ". Thank you for using our server!";
                    //         send(*it, requests[*it].c_str(), requests[*it].size(), 0);
                    //         requests[*it] = "";
                    //     }
                    //     sleep(2);
                    //     ;
                    // }
                    // else
                    // {
                    perror("recv");
                    // }
                }
            }
        }

        // Check if request is ready to be processed
        for (std::list<int>::iterator it = conn_l.begin(); it != conn_l.end(); ++it)
        {
            if (!FD_ISSET(*it, &fds_listen_ret) && FD_ISSET(*it, &fds_listen)) {
                if (requests[*it].size() > 0)
                {
                    std::string res;
                    res += "RESPONCE: your request was:\n";
                    res += "============================\n";
                    res += requests[*it];
                    res += "============================\n";
                    res += "Thank you for using our server!";

                    std::stringstream ss;
                    ss << httpResponse
                    << std::hex << res.length() << "\r\n"
                    << res << "\r\n0\r\n\r\n";

                    std::string to_send = ss.str();
                    send(*it, to_send.c_str(), to_send.size(), 0);
                    requests[*it] = "";

                    std::cout << ">>> DEBUG: sent to the client " << *it <<
                    " : " << res << std::endl;
                }
            }
        }

    }
    close(listenfd);
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(0);
    }
    int port = atoi(argv[1]);
    
    run(port);

    return 0;
}
