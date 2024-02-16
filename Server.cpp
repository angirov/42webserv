#include "Server.hpp"

// Server::Server(Configs configs) {
//                                                               // <<<<<<<<<<<<< CONFIG <<<<<<<<<<<<<<<<
// };

Server::Server(std::list<int> ports_l) : ports_l(ports_l)
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
}

void Server::init_server_sockets(std::list<int> ports_l)
{
    std::list<int>::iterator it;
    for (it = ports_l.begin(); it != ports_l.end(); ++it)
    {
        int fd;
        // create a socket fd
        if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        {
            perror("socket");
            exit(EXIT_FAILURE);
        };

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
        serveraddr.sin_port = htons((unsigned short)*it);
        if (bind(fd, (SA *)&serveraddr, sizeof(serveraddr)) < 0)
        {
            perror("bind");
            exit(EXIT_FAILURE);
        };

        if (listen(fd, LISTENQ) < 0)
        {
            perror("listen");
            exit(EXIT_FAILURE);
        };
        lg.log(INFO, "Server listening on port" + lg.str(*it));
        fcntl(fd, F_SETFL, O_NONBLOCK);

        server_socket_fds_l.push_back(fd);
    }

    max_server_fd = *std::max_element(server_socket_fds_l.begin(), server_socket_fds_l.end());
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
        if (errno == EWOULDBLOCK || errno == EAGAIN)
        {
            lg.log(DEBUG, "No connection request for " + lg.str(fd));
        }
        else
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        fcntl(connfd, F_SETFL, O_NONBLOCK); // the socket is set as non blocking. for macOS only. should be changed for Linux
        client_fds_l.push_back(connfd);
        requests[connfd] = ""; // maybe not necessary

        lg.log(INFO, "Connection from " + std::string(inet_ntoa(clientaddr.sin_addr)) + ":" + lg.str(ntohs(clientaddr.sin_port)));
        lg.log(DEBUG, "Adding fd " + lg.str(connfd) + " to the list. New list " + cout_list(client_fds_l));
    }
}

void Server::run()
{
    init_server_sockets(ports_l);
    while (1)
    {
        if (handTesting)
            sleep(1);

        fill_fd_sets();
        do_select();
        do_send();
    }
}

void Server::do_read(std::list<int>::iterator &fd_itr)
{
    ssize_t num_bytes_recv = 0;
    num_bytes_recv = recv(*fd_itr, buffer, buffsize, 0);
    if (num_bytes_recv > 0)
    {
        lg.log(DEBUG, "Received " + lg.str(num_bytes_recv) + " bytes from fd " + lg.str(*fd_itr) + ": " + std::string(buffer, num_bytes_recv));
        requests[*fd_itr] += std::string(buffer, num_bytes_recv);
    }
    if (num_bytes_recv == 0)
    {
        handle_client_disconnect(fd_itr);
    }
    if (num_bytes_recv == -1)
    {
        perror("recv");
    }
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
    for (std::list<int>::iterator it = client_fds_l.begin(); it != client_fds_l.end(); ++it)
    {
        FD_SET(*it, &read_fd_set);
    }

    // Insert Write FDs
    for (std::list<int>::iterator it = client_fds_l.begin(); it != client_fds_l.end(); ++it)
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
    lg.log(DEBUG, "Select returned: " + lg.str(ret) +
                      " Max FD is " + lg.str(maxFd) +
                      " Server Fds: " + cout_list(server_socket_fds_l) +
                      " Client Fds: " + cout_list(client_fds_l));

    if (ret > 0)
    {
        // Check Server FDs
        for (std::list<int>::iterator it = server_socket_fds_l.begin(); it != server_socket_fds_l.end(); ++it)
        {
            lg.log(DEBUG, "Checking fd " + lg.str(*it));
            if (!FD_ISSET(*it, &read_fd_set))
                continue;
            accept_new_conn(*it);
        }

        // Check Client FDs for READING
        for (std::list<int>::iterator it = client_fds_l.begin(); it != client_fds_l.end(); ++it)
        {
            lg.log(DEBUG, "Checking fd " + lg.str(*it));
            if (!FD_ISSET(*it, &read_fd_set))
                continue;
            do_read(it);
        }
        // do writing???
    }
}

void Server::handle_client_disconnect(std::list<int>::iterator &fd_itr)
{
    lg.log(INFO, "Got zero bytes == Peer has closed the connection gracefully");
    close(*fd_itr);
    lg.log(DEBUG, "Removing fd " + lg.str(*fd_itr) + " from the list. ");
    fd_itr = client_fds_l.erase(fd_itr);
    lg.log(DEBUG, "New list: " + cout_list(client_fds_l));
}

void Server::do_send()
{
    for (std::list<int>::iterator it = client_fds_l.begin(); it != client_fds_l.end(); ++it)
    {
        // Check if any requests are fully read and process them
        // Should be independent of the return value of select but consider the last state of the read set
        if (!FD_ISSET(*it, &read_fd_set) && requests[*it].size() > 0)
        {
            lg.log(DEBUG, "Processing request from " + lg.str(*it) + ". Request:\n" + requests[*it]);
            responces[*it] = Request(requests[*it]).process(); // <<<<<<<<<<<<< REQUEST <<<<<<<<<<<<<<<<
            lg.log(DEBUG, "DONE processing request from " + lg.str(*it) + ". Rescponce:\n" + responces[*it]);
            requests[*it] = "";
        }
        // Check if any responces are ready to be sent
        if (FD_ISSET(*it, &write_fd_set) && responces[*it].size() > 0)
        {
            lg.log(DEBUG, "Sending responce for " + lg.str(*it));
            send(*it, responces[*it].c_str(), responces[*it].size(), 0); // Maybe responce has to be sent in pieces
            lg.log(INFO, "Sent " + lg.str(responces[*it].size()) + " bytes for client " + lg.str(*it));
            responces[*it] = "";
        }
    }
}
