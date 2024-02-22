#pragma once
#include <iostream>
#include "ConfigClass.hpp"

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
