#include <iostream>
#include <vector>
#include <tuple>
#include <map>
#include "conf/ConfigClass.hpp"
#include "tests/test_objects.hpp"

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		fprintf(stderr, "usage: %s <port>\n", argv[0]);
		exit(0);
	}

	std::vector<std::tuple<int, // Port number
			std::vector<std::string>, // Vector of server names
			std::map<int, std::string>, // Error pages
			std::vector<std::tuple<std::string, std::string, std::string, bool, std::vector<std::string>, std::map<int, std::string>, std::vector<std::string>, std::string>>>> serverConfigs; // Vector of Location configurations

	// Fill the serverConfigs vector with information for four virtual servers

	// VirtServer 1
	std::vector<std::string> server1_names = {"allmethods.local"};
	std::map<int, std::string> server1_errorPages = {{401, "/404.html"}};
	std::vector<std::tuple<std::string, std::string, std::string, bool, std::vector<std::string>, std::map<int, std::string>, std::vector<std::string>, std::string>> server1_locations = {
			std::make_tuple("/methods/", // Route
							"/data/root0/", // Location root
							"index.html", // Location index
							true, // Auto index
							std::vector<std::string>{"GET", "POST", "DELETE"}, // Vector of HTTP methods
							std::map<int, std::string>{}, // Redirects for error codes
							std::vector<std::string>{}, // Vector of CGI extensions
							"") // Upload directory
	};
	serverConfigs.emplace_back(7777, server1_names, server1_errorPages, server1_locations);

	// VirtServer 2
	std::vector<std::string> server2_names = {"Maximilian.com", "maximilian.local"};
	std::map<int, std::string> server2_errorPages; // No error pages for server 2
	std::vector<std::tuple<std::string, std::string, std::string, bool, std::vector<std::string>, std::map<int, std::string>, std::vector<std::string>, std::string>> server2_locations = {
			std::make_tuple("/blog/pizza/", // Route
							"/data/root1/", // Location root
							"index.html", // Location index
							true, // Auto index
							std::vector<std::string>{"GET"}, // Vector of HTTP methods
							std::map<int, std::string>{}, // Redirects for error codes
							std::vector<std::string>{"php", "cgi"}, // Vector of CGI extensions
							"/uploads") // Upload directory
	};
	serverConfigs.emplace_back(8080, server2_names, server2_errorPages, server2_locations);

	// VirtServer 3
	std::vector<std::string> server3_names = {"Vladimir.LOCAL", "www.vladimir.org"};
	std::map<int, std::string> server3_errorPages; // No error pages for server 3
	std::vector<std::tuple<std::string, std::string, std::string, bool, std::vector<std::string>, std::map<int, std::string>, std::vector<std::string>, std::string>> server3_locations = {
			std::make_tuple("/travel/destination/", // Route
							"/data/root3/", // Location root
							"index.html", // Location index
							false, // Auto index
							std::vector<std::string>{"GET", "POST"}, // Vector of HTTP methods
							std::map<int, std::string>{}, // Redirects for error codes
							std::vector<std::string>{}, // Vector of CGI extensions
							"") // Upload directory
	};
	serverConfigs.emplace_back(8080, server3_names, server3_errorPages, server3_locations);

	// VirtServer 4
	std::vector<std::string> server4_names = {"redirectserver"};
	std::map<int, std::string> server4_errorPages; // No error pages for server 4
	std::vector<std::tuple<std::string, std::string, std::string, bool, std::vector<std::string>, std::map<int, std::string>, std::vector<std::string>, std::string>> server4_locations = {
			std::make_tuple("/", // Route
							"/var/www/html/", // Location root
							"index.html", // Location index
							true, // Auto index
							std::vector<std::string>{"GET", "POST", "DELETE"}, // Vector of HTTP methods
							std::map<int, std::string>{}, // Redirects for error codes
							std::vector<std::string>{"php"}, // Vector of CGI extensions
							"/upload"), // Upload directory
			std::make_tuple("/intra", // Route
							"", // Location root
							"", // Location index
							false, // Auto index
							std::vector<std::string>{"GET", "POST", "DELETE"}, // Vector of HTTP methods
							std::map<int, std::string>{{301, "https://profile.intra.42.fr/users/mwagner"}}, // Redirects for error codes
							std::vector<std::string>{}, // Vector of CGI extensions
							"") // Upload directory
	};
	serverConfigs.emplace_back(5050, server4_names, server4_errorPages, server4_locations);

	// Create a Config object and fill it using the createObjects function
	Config config;
	createObjects(config, serverConfigs);
	config.display();

	// Additional code to run the server
	// Server server(config);
	// server.run();

	return 0;
}