#include <iostream>
#include <vector>
#include "ConfigClass.hpp"

int main() {
	// Create Config object
	Config config;

	// Set Config parameters
	config.setTimeout(50);
	config.setMaxClients(50);
	config.setClientMaxBodySize(10);

	// Create VirtServer object
	VirtServer server(5001, std::vector<std::string>{"localhost"});

	// Add error page to server
	server.setErrorPage(404, "/404.html");

	// Create first Location object
	Location location1("/", "/var/www/html", "index.html");
	location1.addMethod("GET");
	location1.addMethod("POST");
	location1.addMethod("DELETE");
	location1.setAutoIndex(false);
	location1.addCGIExtension(".php");
	location1.setUploadDir("upload");

	// Create second Location object
	Location location2("/intra", "", "");
	location2.addMethod("GET");
	location2.addMethod("POST");
	location2.addMethod("DELETE");
	location2.setReturnRedir(301, "https://profile.intra.42.fr/users/mwagner");
	location2.setAutoIndex(false);

	// Add locations to the server
	server.addLocation(location1);
	server.addLocation(location2);

	// Add server to the config
	config.addVirtServer(server);

	// Display Config
	config.display();

	return 0;
}
