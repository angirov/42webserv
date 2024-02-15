#include "Server.hpp"
#include "Logger.hpp"

std::list<int> argv2ports(int argc, char **argv)
{
    std::list<int> ports_l;

    for (int i = 1; i < argc; ++i)
    {
        ports_l.push_back(atoi(argv[i]));
    }

    return ports_l;
}

int test_logger() {
    Logger logger(INFO, "logfile.txt");
    logger.log(DEBUG, "This is a debug message.");
    logger.log(INFO, "This is an info message.");
    logger.log(WARNING, "This is a warning message.");
    logger.log(ERROR, "This is an error message.");

    // Redirecting output to stdout
    Logger loggerToStdout(INFO, "", std::cout);
    loggerToStdout.log(INFO, "This message goes to stdout.");

    return 0;
}


int main(int argc, char **argv)
{
    test_logger();
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
