#include "Server.hpp"

std::list<int> argv2ports(int argc, char **argv) {
    std::list<int> ports_l;

    for (int i = 1; i < argc; ++i) {
        ports_l.push_back(atoi(argv[i]));
    }

    return ports_l;
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(0);
    }
    std::list<int> ports_l = argv2ports(argc, argv);
    Server server(ports_l);

    server.run();

    return 0;
}
