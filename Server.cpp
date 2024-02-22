#include "Server.hpp"

// Server::Server(Configs configs) {
//                                                               // <<<<<<<<<<<<< CONFIG <<<<<<<<<<<<<<<<
// };

void Server::init() {
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
    timeout = 5;
    time(&last_checked);
}

Server::Server(std::list<int> ports_l) : ports_l(ports_l)
{
    init();
}

Server::Server(Config config) {
    init();
    _timeout = config.getTimeout();
    _maxClients = config.getMaxClients();
    _client_max_body_size = config.getClientMaxBodySize();

    virtServers = config.getVirtServers();
}

void Server::init_server_sockets()
{
    std::vector<VirtServer> const vsVec = getVirtServers();
    // std::list<int>::iterator it;
    for (vsIt vs_it = vsVec.begin(); vs_it != vsVec.end(); ++vs_it)
    {
        int fd;
        int port = (*vs_it).getPort();
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
        serveraddr.sin_port = htons((unsigned short)port);
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
        lg.log(INFO, "Server listening on port" + lg.str(port));
        fcntl(fd, F_SETFL, O_NONBLOCK);

        server_socket_fds_l.push_back(fd);
        setVirtServerRef(fd, vs_it);
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
        requests[connfd] = ""; // necessary because there could be values of old fd
        responces[connfd] = "";
        keep_alive[connfd] = true;
        setVirtServerRef(fd, getVirtServerRef(fd));
        set_last_time(connfd);

        lg.log(INFO, "Connection from " + std::string(inet_ntoa(clientaddr.sin_addr)) + ":" + lg.str(ntohs(clientaddr.sin_port)));
        lg.log(DEBUG, "Adding fd " + lg.str(connfd) + " to the list. New list " + cout_list(client_fds_l));
    }
}

const vsIt& Server::getVirtServerRef(int fd) const {
    std::map<int, vsIt>::const_iterator it = virtServerRefs.find(fd);

    if (it != virtServerRefs.end()) {
        return (*it).second;
    }
    else
    {
        return notFoundVirtServer;
    }
}

void Server::setVirtServerRef(int fd, vsIt vs_it) {
    virtServerRefs[fd] = vs_it;
}

void Server::set_last_time(int fd)
{
    time_t now;
    time(&now);
    last_times[fd] = now;
}

void Server::run()
{
    init_server_sockets();
    while (1)
    {
        if (handTesting)
            usleep(1000000);
        do_timing();
        fill_fd_sets();
        do_select();
        do_send();
    }
}


void Server::check_timeout()
{
    time_t now;
    time(&now);
    for (std::list<int>::iterator it = client_fds_l.begin(); it != client_fds_l.end(); ++it)
    {
        double secs = difftime(now, last_times[*it]);

        if (secs > timeout) {
            lg.log(DEBUG, "Timeout for " + lg.str(*it) + "; diff: " + lg.str((int)secs) + "; Timeout: " + lg.str(timeout));
            handle_client_disconnect(it);
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
        lg.log(DEBUG, "Received " + lg.str((int)num_bytes_recv) + " bytes from fd " + lg.str(*fd_itr) + ": " + std::string(buffer, num_bytes_recv));
        requests[*fd_itr] += std::string(buffer, num_bytes_recv);
        set_last_time(*fd_itr);
    }
    if (num_bytes_recv == 0)
    {
        lg.log(INFO, "Got zero bytes == Peer has closed the connection gracefully");
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
            responces[*it] = Request(*this, *it, requests[*it]).process(); // <<<<<<<<<<<<< REQUEST <<<<<<<<<<<<<<<<
            lg.log(DEBUG, "DONE processing request from " + lg.str(*it) + ". Rescponce:\n" + responces[*it]);
            requests[*it] = "";
        }
        // Check if any responces are ready to be sent
        if (FD_ISSET(*it, &write_fd_set) && responces[*it].size() > 0)
        {
            lg.log(DEBUG, "Sending responce for " + lg.str(*it));
            send(*it, responces[*it].c_str(), responces[*it].size(), 0); // Maybe responce has to be sent in pieces
            set_last_time(*it);
            lg.log(INFO, "Sent " + lg.str((int)responces[*it].size()) + " bytes for client " + lg.str(*it));
            responces[*it] = "";
        }
    }
}


void Server::displayServer() const {
	std::cout << "Displaying Server variables:\n";
	std::cout << "Timeout: " << _timeout << std::endl;
	std::cout << "Max Clients: " << _maxClients << std::endl;
	std::cout << "Client Max Body Size: " << _client_max_body_size << std::endl;

	// Display VirtServer and Location objects using display() functions
	for (size_t i = 0; i < virtServers.size(); ++i) {
		virtServers[i].display();
	}

}
const std::vector<VirtServer> & Server::getVirtServers() const {
	return virtServers;
}