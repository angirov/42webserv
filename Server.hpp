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

    std::string http200chunked;
    std::list<int> ports_l;
    std::list<int> client_fds_l;
    std::list<int> server_socket_fds_l;
    fd_set fds_listen;
    fd_set fds_listen_ret;
    std::map<int, std::string> requests;

    Server(std::list<int> ports_l) : ports_l(ports_l) {
        optval = 1;
        memset(buffer, 0, sizeof(buffer));
        FD_ZERO(&fds_listen);
        FD_ZERO(&fds_listen_ret);
        // fd_set fds_write;
        http200chunked = "HTTP/1.1 200 OK\r\n"
                         "Content-Type: text/plain\r\n"
                         "Transfer-Encoding: chunked\r\n"
                         "\r\n";
        tv.tv_sec = 0;
        tv.tv_usec = 10;
    }

    void init_server_sockets(std::list<int> ports_l);
    void cout_list(std::list<int> l);
    void run();
};

#endif