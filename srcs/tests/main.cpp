/*
// Function to create objects
void createObjects(Config &config) {
	// Create VirtServer objects
	std::vector<std::string> serverNames1;
	serverNames1.push_back("Maximilian.com");
	serverNames1.push_back("www.maximilian.com");
	VirtServer server1(8080, serverNames1);

	std::vector<std::string> serverNames2;
	serverNames2.push_back("Vladimir.org");
	serverNames2.push_back("www.vladimir.org");
	VirtServer server2(9090, serverNames2);

	// Create Location objects for server
	Location location1("/blog/pizza/french", "/root1/", "index1.html");
	location1.setAutoIndex(true);
	location1.addMethod("GET");
	location1.addCGIExtension(".cgi");
	location1.setUploadDir("/uploads/");
	location1.addReturnRedir(301, "redirect1.html");

	Location location2("/blog/pizza/neapolitan/", "/root2/", "index2.html");
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

std::string findLongestMatchingRoute(const std::string& routeToFind, const VirtServer& virtServer) {
	const std::vector<Location>& locations = virtServer.getLocations();
	std::string longestMatch;

	for (size_t i = 0; i < locations.size(); ++i) {
		const std::string& route = locations[i].getRoute();
		if (routeToFind.find(route) == 0 && route.length() > longestMatch.length()) {
			longestMatch = route;
		}
	}

	return longestMatch;
}

int main() {
	Config config;
	createObjects(config);

	// Display VirtServer and Location objects using display() functions
	std::vector<VirtServer> virtServers = config.getVirtServers();
	for (size_t i = 0; i < virtServers.size(); ++i) {
		const VirtServer& server = virtServers[i];
		std::cout << "Virtual Server: ";
		const std::vector<std::string>& serverNames = server.getServerNames();
		for (size_t j = 0; j < serverNames.size(); ++j) {
			std::cout << serverNames[j] << " ";
		}
		std::cout << std::endl;

		const std::vector<Location>& locations = server.getLocations();
		for (size_t j = 0; j < locations.size(); ++j) {
			const Location& location = locations[j];
			std::cout << "  Route: " << location.getRoute() << std::endl;
		}

		std::string longestMatch = findLongestMatchingRoute("/blog/pizza/neapolitan/index.html", server);
		if (!longestMatch.empty()) {
			std::cout << "Longest matching route: " << longestMatch << std::endl;
		} else {
			std::cout << "No matching route found." << std::endl;
		}
	}
	return 0;
}
*/

#include <iostream>
#include "../conf/ConfigClass.hpp"

// Function to create objects
void createObjects(Config &config) {
	// Create VirtServer objects
	std::vector<std::string> serverNames1;
	serverNames1.push_back("Maximilian.com");
	serverNames1.push_back("www.maximilian.com");
	VirtServer server1(8080, serverNames1);

	std::vector<std::string> serverNames2;
	serverNames2.push_back("Vladimir.org");
	serverNames2.push_back("www.vladimir.org");
	VirtServer server2(9090, serverNames2);

	// Create Location objects for server
	Location location1("/blog/pizza/french", "/root1/", "index1.html");
	location1.setAutoIndex(true);
	location1.addMethod("GET");
	location1.addCGIExtension(".cgi");
	location1.setUploadDir("/uploads/");
	location1.addReturnRedir(301, "redirect1.html");

	Location location2("/blog/pizza/neapolitan/", "/root2/", "index2.html");
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

std::string findLongestMatchingRoute(const std::string& routeToFind, const VirtServer& virtServer) {
	const std::vector<Location>& locations = virtServer.getLocations();
	std::string longestMatch = ""; // Initialize the longest match

	for (size_t i = 0; i < locations.size(); ++i) {
		const std::string& route = locations[i].getRoute();
		if (routeToFind.find(route) == 0 && route.length() > longestMatch.length()) {
			longestMatch = route;
		}
	}

	if (longestMatch.empty()) {
		return "Error: No matching route found"; // Return error message
	}

	return longestMatch; // Return the longest matching route
}

int main() {
	Config config;
	createObjects(config);

	// Display VirtServer and Location objects using display() functions
	std::vector<VirtServer> virtServers = config.getVirtServers();
	for (size_t i = 0; i < virtServers.size(); ++i) {
		const VirtServer& server = virtServers[i];
		std::cout << "Virtual Server: ";
		const std::vector<std::string>& serverNames = server.getServerNames();
		for (size_t j = 0; j < serverNames.size(); ++j) {
			std::cout << serverNames[j] << " ";
		}
		std::cout << std::endl;

		const std::vector<Location>& locations = server.getLocations();
		for (size_t j = 0; j < locations.size(); ++j) {
			const Location& location = locations[j];
			std::cout << "  Route: " << location.getRoute() << std::endl;
		}

		std::string longestMatch = findLongestMatchingRoute("/blog/pizza/neapolitan/index.html", server);
		if (!longestMatch.empty()) {
			std::cout << "Longest matching route: " << longestMatch << std::endl;
		} else {
			std::cout << "No matching route found." << std::endl;
		}
	}

	return 0;
}