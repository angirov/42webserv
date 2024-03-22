/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mwagner <mwagner@student.42wolfsburg.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/22 12:28:04 by mwagner           #+#    #+#             */
/*   Updated: 2024/03/22 12:28:05 by mwagner          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

void Server::init()
{
    if (std::getenv("WEBSERV_HANDTESTING"))
        handTesting = true;
    else
        handTesting = false;

    std::string loglevel;
    const char *log_var = std::getenv("WEBSERV_LOGLEVEL");
    if (!log_var || std::string(log_var).empty())
        loglevel = "INFO";
    else
        loglevel = std::getenv("WEBSERV_LOGLEVEL");

    if (handTesting)
    {
        buffsize = BUFFERTEST;
        lg = Logger(loglevel, "", std::cout);
    }
    else
    {
        buffsize = BUFFERSIZE;
        lg = Logger(loglevel, "logfile.txt");
    }
    buffer = new char[buffsize];
    lg.log(INFO, "Logger level is " + lg.str(lg.getLevel()));

    lg.env("WEBSERV_HANDTESTING");
    lg.env("WEBSERV_LOGLEVEL");
    optval = 1;
    memset(buffer, 0, buffsize);
    FD_ZERO(&read_fd_set);
    FD_ZERO(&write_fd_set);

    tv.tv_sec = 0;
    tv.tv_usec = 10;
    time(&last_checked);
}

Server::Server(std::list<int> ports_l) : ports_l(ports_l)
{
    init();
}

Server::Server(Config config)
{
    init();
    _timeout = config.getTimeout();
    _maxClients = config.getMaxClients();
    _client_max_body_size = config.getClientMaxBodySize();

    virtServers = config.getVirtServers();
}

void Server::init_server_sockets()
{
    std::set<int>::iterator it;
    // std::list<int>::iterator it;
    for (it = uniquePorts.begin(); it != uniquePorts.end(); ++it)
    {
        int fd;
        int port = *it;

        // create a socket fd
        if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        {
            perror("socket");
            exit(EXIT_FAILURE);
        };
        setPortRef(fd, port);

        // eliminate addr already in use error
        if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR,
                       (const void *)&optval, sizeof(int)) < 0)
        {
            perror("setsockopt");
            exit(EXIT_FAILURE);
        };

        // This fd is endpoint for all requests to port on ANY IP for this host
        bzero((char *)&serveraddr, sizeof(serveraddr));
        serveraddr.sin_family = AF_INET;
        serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
        serveraddr.sin_port = htons((unsigned short)port);
        if (bind(fd, (SA *)&serveraddr, sizeof(serveraddr)) < 0)
        {
            perror("bind");
            exit(EXIT_FAILURE);
        };

        if (listen(fd, _maxClients) < 0)
        {
            perror("listen");
            exit(EXIT_FAILURE);
        };
        fcntl(fd, F_SETFL, O_NONBLOCK);

        server_socket_fds_l.push_back(fd);
    }
}

std::string Server::cout_list(std::list<int> l)
{
    std::stringstream ss;
    ss << "[ ";
    for (std::list<int>::iterator it = l.begin(); it != l.end(); ++it)
    {
        if (it != l.begin())
            ss << ", ";
        ss << *it;
    }
    ss << " ]";
    return ss.str();
}

void Server::accept_new_conn(int fd)
{
    clientlen = sizeof(clientaddr);
    connfd = accept(fd, (SA *)&clientaddr, (socklen_t *)&clientlen);
    // https://stackoverflow.com/questions/7003234/
    if (connfd == -1)
    {
        perror("accept");
    }
    else
    {
        fcntl(connfd, F_SETFL, O_NONBLOCK); // the socket is set as non blocking. for macOS only. should be changed for Linux
        client_fds_l.push_back(connfd);
        requests[connfd] = ""; // necessary because there could be values of old fd
        responces[connfd] = "";
        keep_alive[connfd] = true;
        setClientRef(connfd, fd);
        set_last_time(connfd);

        lg.log(INFO, "Connection from " + std::string(inet_ntoa(clientaddr.sin_addr)) + ":" + lg.str(ntohs(clientaddr.sin_port)));
        lg.log(DEBUG, "Adding fd " + lg.str(connfd) + " to the list. New list: " + cout_list(client_fds_l));
    }
}

void Server::createVirtServerRefs()
{
    std::multimap<int, vsIt> collector;

    const std::vector<VirtServer> &vs = getVirtServers();
    std::vector<VirtServer>::const_iterator vs_it;
    for (vs_it = vs.begin(); vs_it != vs.end(); ++vs_it)
    {
        collector.insert(std::make_pair((*vs_it).getPort(), vs_it));
        uniquePorts.insert((*vs_it).getPort());
    }

    std::multimap<int, vsIt>::iterator mmit;
    for (mmit = collector.begin(); mmit != collector.end(); ++mmit)
    {
        virtServerRefs[(*mmit).first].push_back((*mmit).second);
    }
}

int Server::getClientRef(int clientFd) const
{
    std::map<int, int>::const_iterator it = clientRefs.find(clientFd);

    if (it != clientRefs.end())
    {
        return (*it).second;
    }
    else
    {
        return -1;
    }
}

void Server::setClientRef(int clientFd, int serverFd)
{
    clientRefs[clientFd] = serverFd;
}

const std::vector<vsIt> &Server::getVirtServerRefs(int port) const
{
    std::map<int, std::vector<vsIt> >::const_iterator it = virtServerRefs.find(port);

    if (it != virtServerRefs.end())
    {
        return (*it).second;
    }
    else
    {
        return notFoundVirtServerVec;
    }
}

const std::vector<vsIt> &Server::clientFd2vsIt(int clientFd) const
{
    return getVirtServerRefs(getPortRef(getClientRef(clientFd)));
}

int Server::getPortRef(int serverFd) const
{
    std::map<int, int>::const_iterator it = portRefs.find(serverFd);

    if (it != portRefs.end())
    {
        return (*it).second;
    }
    else
    {
        return -1;
    }
}

void Server::setPortRef(int serverFd, int port)
{
    portRefs[serverFd] = port;
}

void Server::set_last_time(int fd)
{
    time_t now;
    time(&now);
    last_times[fd] = now;
}

void Server::run()
{
    createVirtServerRefs();
    init_server_sockets();
    max_server_fd = *std::max_element(server_socket_fds_l.begin(), server_socket_fds_l.end());
    displayServer();

    while (1)
    {
        if (handTesting)
            usleep(1000000);
        do_timing();
        fill_fd_sets();
        do_select();
        check_request();
        // check_size();
    }
}

void Server::check_timeout()
{
    time_t now;
    time(&now);
    std::list<int>::iterator it = client_fds_l.begin();
    while (it != client_fds_l.end())
    {
        double secs = difftime(now, last_times[*it]);

        if (secs > _timeout)
        {
            lg.log(DEBUG, "Timeout for " + lg.str(*it) + "; diff: " + lg.str(secs) + "; Timeout: " + lg.str(_timeout));
            close(*it);
            lg.log(DEBUG, "Disconnecting: Old list: " + cout_list(client_fds_l));
            lg.log(DEBUG, "Removing fd " + lg.str(*it) + " from the list. ");
            it = client_fds_l.erase(it); // returns the next one
            lg.log(DEBUG, "Disconnecting: New list: " + cout_list(client_fds_l));
        }
        else
        {
            ++it;
        }
    }
}

void Server::do_timing()
{
    time_t now;
    time(&now);
    double secs = difftime(now, last_checked);
    if (secs > 0)
        check_timeout();
    last_checked = now;
}

void Server::do_read(std::list<int>::iterator &fd_itr)
{
    ssize_t num_bytes_recv = 0;
    num_bytes_recv = recv(*fd_itr, buffer, buffsize, 0);
    if (num_bytes_recv > 0)
    {
        lg.log(DEBUG, "Received " + lg.str(num_bytes_recv) + " bytes from fd " + lg.str(*fd_itr) + ": " + std::string(buffer, num_bytes_recv));
        requests[*fd_itr] += std::string(buffer, num_bytes_recv);
        set_last_time(*fd_itr);
    }
    if (num_bytes_recv == 0)
    {
        lg.log(INFO, "Server: recv got zero bytes i.e. peer has closed the connection gracefully. Fd: " + lg.str(*fd_itr));
        disconnectFd(*fd_itr);
    }
    if (num_bytes_recv == -1)
    {
        lg.log(ERROR, "Server: recv got -1 responce code while reading fd: " + lg.str(*fd_itr));
        disconnectFd(*fd_itr);
    }
}

void Server::disconnectFd(int fd)
{
    
    lg.log(DEBUG, "Disconnecting and Removing fd " + lg.str(fd) + " from the list.");
    close(fd);
    removeAllElementsByValue(client_fds_l, fd);
    lg.log(DEBUG, "Disconnected: New list: " + cout_list(client_fds_l));
}

void Server::fill_fd_sets()
{
    FD_ZERO(&read_fd_set);
    FD_ZERO(&write_fd_set);

    // Insert Server FDs
    for (std::list<int>::iterator it = server_socket_fds_l.begin(); it != server_socket_fds_l.end(); ++it)
    {
        FD_SET(*it, &read_fd_set);
    }

    // Insert Read FDs
    std::list<int> readingFds = getReadingFds();
    for (std::list<int>::iterator it = readingFds.begin(); it != readingFds.end(); ++it)
    {
        FD_SET(*it, &read_fd_set);
    }

    // Insert Write FDs
    for (std::list<int>::iterator it = writing_fds_l.begin(); it != writing_fds_l.end(); ++it)
    {
        FD_SET(*it, &write_fd_set);
    }
}

int Server::find_maxFd()
{
    int maxFd;
    std::list<int>::iterator max_client_fd_itr = std::max_element(client_fds_l.begin(), client_fds_l.end());
    if (max_client_fd_itr != client_fds_l.end())
    {
        maxFd = std::max(max_server_fd, *max_client_fd_itr);
    }
    else
    {
        maxFd = max_server_fd;
    }
    return maxFd;
}

void Server::do_select()
{
    int ret;
    int maxFd = find_maxFd();
    ret = select(maxFd + 1, &read_fd_set, &write_fd_set, NULL, &tv);
    // lg.log(DEBUG, "Select returned: " + lg.str(ret) +
    //                   " Max FD is " + lg.str(maxFd) +
    //                   " Server Fds: " + cout_list(server_socket_fds_l) +
    //                   " Reading Fds: " + cout_list(getReadingFds()) +
    //                   " Writing Fds: " + cout_list(writing_fds_l));

    if (ret > 0)
    {
        // Check Server FDs
        for (std::list<int>::iterator it = server_socket_fds_l.begin(); it != server_socket_fds_l.end(); ++it)
        {
            lg.log(DEBUG, "Checking server fd " + lg.str(*it));
            if (!FD_ISSET(*it, &read_fd_set))
                continue;
            accept_new_conn(*it);
        }

        // Check Client FDs for READING
        std::list<int> readingFds = getReadingFds();
        for (std::list<int>::iterator it = readingFds.begin(); it != readingFds.end(); ++it)
        {
            // lg.log(DEBUG, "Checking reading fd " + lg.str(*it));
            if (!FD_ISSET(*it, &read_fd_set))
                continue;
            do_read(it);
        }
        std::list<int>::iterator it = writing_fds_l.begin();
        while (it != writing_fds_l.end())
        {
            // lg.log(DEBUG, "Checking writing fd " + lg.str(*it));
            if (!FD_ISSET(*it, &write_fd_set))
                continue;
            do_write(*it);
            it = writing_fds_l.erase(it);
        }
    }
    // lg.log(DEBUG, "Done select");
}

void Server::check_request()
{
    std::list<int> readingFds = getReadingFds();
    for (std::list<int>::iterator it = readingFds.begin(); it != readingFds.end(); ++it)
    {
        bool sizeExceeded = (getHTTPBodySize(requests[*it]) > _client_max_body_size);
        // Check if any requests are fully read and process them
        // Should be independent of the return value of select but consider the last state of the read set
        if ((sizeExceeded || (!FD_ISSET(*it, &read_fd_set) && requests[*it].size() > 0)))
        {
            lg.log(DEBUG, "Processing request from " + lg.str(*it) + ". Request:\n" + requests[*it]);
            responces[*it] = Request(*this, *it, requests[*it], sizeExceeded).process(); // <<<<<<<<<<<<< REQUEST <<<<<<<<<<<<<<<<
            lg.log(DEBUG, "DONE processing request from " + lg.str(*it) + ". Response:\n" + responces[*it]);
            writing_fds_l.push_back(*it);
            requests[*it] = "";
        }
    }
}

void Server::check_size()
{
    for (std::list<int>::iterator it = client_fds_l.begin(); it != client_fds_l.end(); ++it)
    {
        size_t size = getHTTPBodySize(requests[*it]);
        // lg.log(DEBUG, "Checking client_max_body_size for " + lg.str(*it) + "; max size: " + lg.str(_client_max_body_size) + "; already received: " + lg.str(size));
        if (size > _client_max_body_size)
        {
            lg.log(ERROR, "Client_max_body_size EXCEEDED for " + lg.str(*it) + "; max size: " + lg.str(_client_max_body_size) + "; already received: " + lg.str(size));
            disconnectFd(*it);
            requests[*it] = "";
        }
    }
}

void Server::do_write(int fd)
{
    lg.log(DEBUG, "Sending response for " + lg.str(fd));
    int ret;
    bool sizeExceeded = isInFirstLine(responces[fd], "413");
    lg.log(INFO, "Sending " + lg.str(responces[fd].size()) + " bytes for client " + lg.str(fd));
    ret = send(fd, responces[fd].c_str(), responces[fd].size(), 0); // Maybe response has to be sent in pieces
    if (ret == 0)
    {
        lg.log(ERROR, "Server: recv got 0 responce code while reading fd: " + lg.str(fd));
        disconnectFd(fd);
    }
    else if (ret < 0)
    {
        lg.log(ERROR, "Server: recv got -1 responce code while reading fd: " + lg.str(fd));
        disconnectFd(fd);
    }
    else if (sizeExceeded)
    {
        lg.log(ERROR, "Server: sent error 413 to: " + lg.str(fd) + " . Going to disconnenct.");
        disconnectFd(fd);
    }
    set_last_time(fd);
    responces[fd] = "";
}

void Server::displayServer() const
{
    std::cout << "Displaying Server variables:\n";
    std::cout << "Timeout: " << _timeout << std::endl;
    std::cout << "Max Clients: " << _maxClients << std::endl;
    std::cout << "Client Max Body Size: " << _client_max_body_size << std::endl;
    displayVirtServerRefs();
    // Display VirtServer and Location objects using display() functions
    for (size_t i = 0; i < virtServers.size(); ++i)
    {
        virtServers[i].display();
    }
}
const std::vector<VirtServer> &Server::getVirtServers() const
{
    return virtServers;
}

void Server::displayVirtServerRefs() const
{
    std::map<int, std::vector<vsIt> >::const_iterator it;
    std::cout << "virtServerRefs: \n";
    for (it = virtServerRefs.begin(); it != virtServerRefs.end(); ++it)
    {
        std::cout << ">>>> Port: " << (*it).first << " Servers: ";

        std::vector<vsIt>::const_iterator vs_it;
        for (vs_it = (*it).second.begin(); vs_it != (*it).second.end(); ++vs_it)
        {
            const vsIt server_iter = *vs_it;
            std::cout << *(*server_iter).getServerNames().begin() << "";
        }
        std::cout << std::endl;
    }
}

void Server::addToWriting(int value)
{
    // clients_fds_l.remove(value);
    writing_fds_l.push_back(value);
}

void Server::rmFromWriting(int value)
{
    // clients_fds_l.remove(value);
    writing_fds_l.remove(value);
}

std::list<int> Server::getReadingFds()
{
    return deductLists(client_fds_l, writing_fds_l);
}