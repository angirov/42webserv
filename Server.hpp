#ifndef SERVER_HPP
#define SERVER_HPP

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
#include <cstring>
#include <cstdlib> // For getenv

#define LISTENQ 10
#define BUFFERSIZE 10000
#define BUFFERTEST 100

#include "Request.hpp"
#include "Logger.hpp"
// #include "Configs.hpp"

typedef struct sockaddr SA;

class Server
{
private:
    bool handTesting;
    Logger lg;
    unsigned int buffsize;

public:
    int connfd;
    int clientlen;
    struct timeval tv;
    struct sockaddr_in clientaddr;
    char *buffer;
    int optval;
    struct sockaddr_in serveraddr;

    std::list<int> ports_l;
    std::list<int> client_fds_l;
    std::list<int> server_socket_fds_l;
    fd_set read_fd_set;
    fd_set write_fd_set;
    std::map<int, std::string> requests;
    std::map<int, std::string> responces;

    // Server(Configs configs);
    Server(std::list<int> ports_l);

    void fill_fd_sets();
    void init_server_sockets(std::list<int> ports_l);
    void accept_new_conn(int fd);
    std::string cout_list(std::list<int> const l);
    void handle_client_disconnect(std::list<int>::iterator &fd_itr);
    void do_select();
    void do_read(std::list<int>::iterator &fd_itr);
    void do_send();
    void run();
};

#endif