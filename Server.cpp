#include "Server.hpp"

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
    // FD_ZERO(&fds_listen); ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FD_ZERO(&fds_listen_ret);
    // fd_set fds_write;

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
            if (handTesting)
                sleep(1);
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
        client_fds_l.push_back(connfd);
        // FD_SET(connfd, &fds_listen); ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        requests[connfd] = "";

        lg.log(INFO, "Connection from " + std::string(inet_ntoa(clientaddr.sin_addr)) + ":" + lg.str(ntohs(clientaddr.sin_port)));
        lg.log(DEBUG, "Adding fd " + lg.str(connfd) + " to the list. New list " + cout_list(client_fds_l));
    }
}

void Server::run()
{
    init_server_sockets(ports_l);
    while (1)
    {
        accept_new_conn(*server_socket_fds_l.begin());
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

void Server::fill_fd_set() {
    FD_ZERO(&fds_listen_ret);
    for (std::list<int>::iterator it = client_fds_l.begin(); it != client_fds_l.end(); ++it) {
        FD_SET(*it, &fds_listen_ret);
    }
}


void Server::do_select()
{
    // fds_listen_ret = fds_listen; ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    fill_fd_set();
    int ret;
    ret = select(1024, &fds_listen_ret, NULL, NULL, &tv); // TODO: max has to be calculated
    lg.log(DEBUG, "select returned: " + lg.str(ret) + " Fds to monitor: " + cout_list(client_fds_l));

    if (ret > 0)
    {
        for (std::list<int>::iterator it = client_fds_l.begin(); it != client_fds_l.end(); ++it)
        {
            lg.log(DEBUG, "Checking fd " + lg.str(*it));
            if (!FD_ISSET(*it, &fds_listen_ret))
                continue;
            do_read(it);
        }
    }
}

void Server::handle_client_disconnect(std::list<int>::iterator &fd_itr)
{
    lg.log(INFO, "Got zero bytes == Peer has closed the connection gracefully");
    close(*fd_itr);
    lg.log(DEBUG, "Removing fd " + lg.str(*fd_itr) + " from the list. ");
    fd_itr = client_fds_l.erase(fd_itr);
    // FD_CLR(*fd_itr, &fds_listen); ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    lg.log(DEBUG, "New list: " + cout_list(client_fds_l));
}

void Server::do_send()
{
    // Check if request is ready to be processed
    for (std::list<int>::iterator it = client_fds_l.begin(); it != client_fds_l.end(); ++it)
    {
        if (!FD_ISSET(*it, &fds_listen_ret) && requests[*it].size() > 0)
        {
            std::string responce = Request(requests[*it]).process();
            send(*it, responce.c_str(), responce.size(), 0);
            requests[*it] = "";

            lg.log(INFO, "sent to the client " + lg.str(*it) + " :\n" + responce);
        }
    }
}
