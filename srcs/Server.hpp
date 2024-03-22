/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mwagner <mwagner@student.42wolfsburg.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/22 12:28:09 by mwagner           #+#    #+#             */
/*   Updated: 2024/03/22 12:28:10 by mwagner          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include <sys/socket.h>
#include <netinet/in.h>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <strings.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <list>
#include <iostream>
#include <algorithm>
#include <iterator>
#include <map>
#include <set>
#include <sys/select.h>
#include <sstream>
#include <cstring>
#include <ctime>

#define BUFFERSIZE 10000
#define BUFFERTEST 10000

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
    size_t _client_max_body_size;
    std::vector<VirtServer> virtServers;

    //////////////////////////////////////////////////////////////////////

    bool handTesting;
    mutable Logger lg;
    unsigned int buffsize;
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
    std::list<int> writing_fds_l;
    int max_server_fd;
    fd_set read_fd_set;
    fd_set write_fd_set;

    // Client related stuff
    std::list<int> server_socket_fds_l;
    std::map<int, std::string> requests;
    std::map<int, std::string> responces;
    std::map<int, time_t> last_times;
    std::map<int, bool> keep_alive;
    std::map<int, int> clientRefs; // clientFd -> serverFd

    std::set<int> uniquePorts;
    std::map<int, int > portRefs; // serverFd -> port
    std::map<int, std::vector<vsIt> > virtServerRefs; // port -> virt server it

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
    void do_write(int fd);
    void check_request();
    void check_size();
    void do_timing();
    void check_timeout();
    void set_last_time(int fd);

    void createVirtServerRefs();
    const std::vector<vsIt>& getVirtServerRefs(int port) const;
    const std::vector<vsIt>& clientFd2vsIt(int clientFd) const;
    void displayVirtServerRefs() const;
    
    int getPortRef(int serverFd) const; // -> port
    void setPortRef(int serverFd, int port); // serverFd -> port

    int getClientRef(int clientFd) const; // -> serverFd 
    void setClientRef(int clientFd, int serverFd);

    void addToWriting(int value);
    void rmFromWriting(int value);
    std::list<int> getReadingFds();
    int maxOfList(std::list<int> & l) const;
    const std::vector<VirtServer> & getVirtServers() const;
    void run();
    void disconnectFd(int fd);
};

#endif