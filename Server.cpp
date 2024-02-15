#include "Server.hpp"

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
        printf("Server listening on port %d...\n", *it);
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
            printf("Checked server socket... No connection attempt ...\n");
            // sleep(1);
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
        FD_SET(connfd, &fds_listen);
        requests[connfd] = "";

        // The rest is just for debugging
        // if (FD_ISSET(connfd, &fds_listen)) {
        //     std::cout << "DEBUG: Checked if ISSET for the new client socket: TRUE\n";
        // } else {
        //     std::cout << "DEBUG: Checked if ISSET for the new client socket: FALSE\n";
        // };
        std::cout << "Adding fd " << connfd << " to the list. New list " << cout_list(client_fds_l) << std::endl;
        printf("Connection from %s:%d\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
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

void Server::do_select() {
    std::cout << ">>> DEBUG: before select" << std::endl;
    fds_listen_ret = fds_listen;
    int ret;
    ret = select(1024, &fds_listen_ret, NULL, NULL, &tv); // TODO: max has to be calculated
    std::cout << ">>> DEBUG: after select: returned: " << ret << std::endl;

    if (ret > 0)
    {
        for (std::list<int>::iterator it = client_fds_l.begin(); it != client_fds_l.end(); ++it)
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
                handle_client_disconnect(it);
                continue;
            }
            if (num_bytes_recv == -1)
            {
                perror("recv");
            }
        }
    }
}

void Server::handle_client_disconnect(std::list<int>::iterator & fd_ptr) {
    std::cout << "Got zero bytes == Peer has closed the connection gracefully\n";
    close(*fd_ptr);
    std::cout << "Removing fd " << *fd_ptr << " from the list. ";
    fd_ptr = client_fds_l.erase(fd_ptr);
    FD_CLR(*fd_ptr, &fds_listen);
    std::cout << "New list: " << cout_list(client_fds_l) << std::endl;
}

void Server::do_send() {
    // Check if request is ready to be processed
    for (std::list<int>::iterator it = client_fds_l.begin(); it != client_fds_l.end(); ++it)
    {
        if (!FD_ISSET(*it, &fds_listen_ret) && FD_ISSET(*it, &fds_listen))
        {
            if (requests[*it].size() > 0)
            {
                std::string responce = process_request(requests[*it]);
                send(*it, responce.c_str(), responce.size(), 0);
                requests[*it] = "";

                std::cout << ">>> DEBUG: sent to the client " << *it << " :\n" << responce << std::endl;
            }
        }
    }
}

std::string Server::process_request(const std::string & request) {
    std::string res;
    res += "RESPONCE: your request was:\n";
    res += "============================\n";
    res += request;
    res += "============================\n";
    res += "Thank you for using our server!";

    std::stringstream ss;
    ss << http200chunked
        << std::hex << res.length() << "\r\n"
        << res << "\r\n0\r\n\r\n";

    return ss.str();
}
