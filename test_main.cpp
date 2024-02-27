#include "Server.hpp"
#include "Logger.hpp"
#include "conf/ConfigClass.hpp"

#include "tests/test_objects.hpp"

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
	if (argc != 2)
	{
		fprintf(stderr, "usage: %s <port>\n", argv[0]);
		exit(0);
	}

	// Define configurations for virtual servers
	// ORDER
	// int: Port number.
	// std::vector<std::string>: Vector of server names.

	// Vector of tuples representing location configurations.
	// You can add multiple Locations at once, see Server maximilian.local

	// std::string: Route.
	// std::string: Location root.
	// std::string: Location index.
	// bool: Auto index.
	// std::vector<std::string>: Vector of HTTP methods.
	std::vector<std::tuple<int, std::vector<std::string>, std::vector<std::tuple<std::string, std::string, std::string, bool, std::vector<std::string>>>>> serverConfigs = {
			{7777, {"allmethods.local"}, { {"/methods/", "/data/root0/", "index.html", true, {"GET", "POST", "DELETE"}} }},
			{8080, {"Maximilian.com", "maximilian.local"}, { {"/blog/pizza/", "/data/root1/", "index.html", true, {"GET"}}, {"/path2/", "/root2/upload/", "upload.html", false, {"POST"}} }},
			{8080, {"Vladimir.LOCAL", "www.vladimir.org"}, { {"/travel/destination/", "/data/root3/", "index.html", false, {"GET", "POST"}} }}
	};

	Config config;
	createObjects(config, serverConfigs);
	Server server(config);
	server.displayServer();

	server.run();

	return 0;
}
