#ifndef SERVER_HPP
# define SERVER_HPP

# include <sys/socket.h>
# include <netinet/in.h>
# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <strings.h>
# include <arpa/inet.h>
# include <fcntl.h>
# include <errno.h>
# include <list>
# include <iostream>
# include <algorithm>
# include <iterator>
# include <map>
# include <sys/select.h>
# include <sstream>
# include <cstring>

# define LISTENQ 10
# define BUFFERSIZE 1000

typedef struct sockaddr SA;

class Server {
public:
    int connfd;
    int clientlen;
    struct timeval tv;
    struct sockaddr_in clientaddr;
    char buffer[BUFFERSIZE];
    int listenfd;
    int optval;
    struct sockaddr_in serveraddr;

    std::string httpResponse;
    std::list<int> conn_l;
    fd_set fds_listen;
    fd_set fds_listen_ret;
    std::map<int, std::string> requests;

    Server() {
        optval = 1;
        memset(buffer, 0, sizeof(buffer));
        FD_ZERO(&fds_listen);
        FD_ZERO(&fds_listen_ret);
        // fd_set fds_write;
        httpResponse =  "HTTP/1.1 200 OK\r\n"
                        "Content-Type: text/plain\r\n"
                        "Transfer-Encoding: chunked\r\n"
                        "\r\n";
        tv.tv_sec = 1;
        tv.tv_usec = 0;
    }

    int open_listenfd(int port);
    void cout_list(std::list<int> l);
    void run(int port);
};

#endif