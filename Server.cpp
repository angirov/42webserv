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

void Server::cout_list(std::list<int> l)
{
    std::ostream_iterator<int> out_it(std::cout, ", ");
    for (std::list<int>::iterator it = l.begin(); it != l.end(); ++it)
    {
        *out_it++ = *it;
    }
    std::cout << "]" << std::endl;
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
        std::cout << "Adding fd " << connfd << " to the list. New list: [";
        cout_list(client_fds_l);
        printf("Connection from %s:%d\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
    }
}

void Server::run()
{
    init_server_sockets(ports_l);
    while (1)
    {
        accept_new_conn(*server_socket_fds_l.begin());

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
                    std::cout << "Got zero bytes == Peer has closed the connection gracefully\n";
                    close(*it);
                    std::cout << "Removing fd " << *it << " from the list. ";
                    it = client_fds_l.erase(it);
                    FD_CLR(*it, &fds_listen);
                    std::cout << "New list: [";
                    cout_list(client_fds_l);
                    continue;
                }
                if (num_bytes_recv == -1)
                {
                    perror("recv");
                }
            }
        }

        // Check if request is ready to be processed
        for (std::list<int>::iterator it = client_fds_l.begin(); it != client_fds_l.end(); ++it)
        {
            if (!FD_ISSET(*it, &fds_listen_ret) && FD_ISSET(*it, &fds_listen))
            {
                if (requests[*it].size() > 0)
                {
                    std::string res;
                    res += "RESPONCE: your request was:\n";
                    res += "============================\n";
                    res += requests[*it];
                    res += "============================\n";
                    res += "Thank you for using our server!";

                    std::stringstream ss;
                    ss << http200chunked
                       << std::hex << res.length() << "\r\n"
                       << res << "\r\n0\r\n\r\n";

                    std::string to_send = ss.str();
                    send(*it, to_send.c_str(), to_send.size(), 0);
                    requests[*it] = "";

                    std::cout << ">>> DEBUG: sent to the client " << *it << " : " << res << std::endl;
                }
            }
        }
    }
}
