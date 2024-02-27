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
	std::vector<std::tuple<int, std::vector<std::string>, std::vector<std::tuple<std::string, std::string, std::string, bool, std::vector<std::string>>>>> serverConfigs = {
			{7777, {"whatever.com", "whatever.local"}, { {"/path0/", "/root0/", "index0.html", true, {"GET", "POST", "DELETE"}} }},
			{8080, {"Maximilian.com", "maximilian.local"}, { {"/path1/", "/root1/", "index1.html", true, {"GET", "POST"}}, {"/path2/", "/root2/", "index2.html", false, {"GET", "POST"}} }},
			{8080, {"Vladimir.LOCAL", "www.vladimir.org"}, { {"/path3/", "/root3/", "index3.html", true, {"GET", "POST"}} }}
	};

	Config config;
	createObjects(config, serverConfigs);
	Server server(config);
	server.displayServer();

	server.run();

	return 0;
}
