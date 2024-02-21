#include "Server.hpp"
#include "Logger.hpp"
#include "conf/ConfigClass.hpp"

// Function to create objects
void createObjects(Config &config) {
    config.setTimeout(5);
    config.setMaxClients(10);
    config.setClientMaxBodySize(10000);

	// Create VirtServer objects
	VirtServer server1(5001, "Maximilian.com");
	VirtServer server2(9090, "Vladimir.org");

	// Create Location objects for server1
	Location location1("/path1/", "/root1/", "index1.html");
	location1.setAutoIndex(true);
	location1.addMethod("GET");
	location1.addCGIExtension(".cgi");
	location1.setUploadDir("/uploads/");
	location1.addReturnRedir(301, "redirect1.html");

	Location location2("/path2/", "/root2/", "index2.html");
	location2.setAutoIndex(false);
	location2.addMethod("POST");
	location2.addCGIExtension(".php");
	location2.setUploadDir("/files/");
	location2.addReturnRedir(302, "redirect2.html");

	// Add locations to respective servers
	server1.addLocation(location1);
	server2.addLocation(location2);

	// Add VirtServer objects to the Config
	config.addVirtServer(server1);
	config.addVirtServer(server2);
}

// Function to display config variables
void displayConfig(const Config &config) {
	std::cout << "Displaying Config variables:\n";
	std::cout << "Timeout: " << config.getTimeout() << std::endl;
	std::cout << "Max Clients: " << config.getMaxClients() << std::endl;
	std::cout << "Client Max Body Size: " << config.getClientMaxBodySize() << std::endl;
}

std::list<int> argv2ports(int argc, char **argv)
{
    std::list<int> ports_l;

    for (int i = 1; i < argc; ++i)
    {
        ports_l.push_back(atoi(argv[i]));
    }

    return ports_l;
}

int test_logger()
{
    char *env;
    std::string loglevel = "INFO";
    if ((env = std::getenv("WEBSERV_LOGLEVEL")))
    {
        loglevel = env;
    }
    Logger logger(loglevel, "logfile3.txt");

    if (env)
        logger.log(INFO, "Value of WEBSERV_LOGLEVEL is " + std::string(env));
    else
        logger.log(INFO, "Value of WEBSERV_LOGLEVEL is NOT FOUND");
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

    // test_logger();
    if (argc != 2)
    {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(0);
    }


    // std::list<int> ports_l = argv2ports(argc, argv);
    // Server server(ports_l);


	Config config;
	createObjects(config);
    Server server(config);
    server.displayServer();
    
    // Request req(server, "GET /index.html HTTP/1.1\r\n"
    //             "Host: webserv.42\r\n"
    //             "Connection: Keep-Alive\r\n"
    //             "RandomHeader: val1, val2, val3\r\n"
    //             "RandomHeader2: \"val with multiple words\", \"value, with, commas\"\r\n"
    //             "BODY BODY");

    // req.parse();
    // req.print_request();

    server.run();

    return 0;
}
