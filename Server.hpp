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
#include <time.h>

#define LISTENQ 10
#define BUFFERSIZE 10000
#define BUFFERTEST 100

#include "Request.hpp"
#include "Logger.hpp"
#include "conf/ConfigClass.hpp"
#include "utils.hpp"

typedef struct sockaddr SA;

struct Server
{
// private:
    int _timeout;
    int _maxClients;
    int _client_max_body_size;
    std::vector<VirtServer> virtServers;

    //////////////////////////////////////////////////////////////////////

    bool handTesting;
    Logger lg;
    unsigned int buffsize;
    int timeout;
    time_t last_checked;

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
    int max_server_fd;
    fd_set read_fd_set;
    fd_set write_fd_set;

    // Client related stuff
    std::list<int> server_socket_fds_l;
    std::map<int, std::string> requests;
    std::map<int, std::string> responces;
    std::map<int, time_t> last_times;
    std::map<int, bool> keep_alive;
    std::map<int, int> clientRefs;

    std::map<int, std::vector<vsIt> > virtServerRefs;
    
    Server(Config config);
    Server(std::list<int> ports_l);

    void init();
    void displayServer() const;
    void fill_fd_sets();
    void init_server_sockets();
    void accept_new_conn(int fd);
    std::string cout_list(std::list<int> const l);
    void handle_client_disconnect(std::list<int>::iterator &fd_itr);
    void do_select();
    int find_maxFd();
    void do_read(std::list<int>::iterator &fd_itr);
    void do_send();
    void do_timing();
    void check_timeout();
    void set_last_time(int fd);

    void createVirtServerRefs();
    void displayVirtServerRefs() const;
    
    int getClientRef(int clientFd) const;
    void setClientrRef(int clientFd, int serverFd);

    const std::vector<VirtServer> & getVirtServers() const;
    void run();
};

#endif