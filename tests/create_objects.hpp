#pragma once
#include <iostream>
#include "../conf/ConfigClass.hpp"

// Function to create objects
void createObjects(Config &config)
{
	// Create VirtServer objects
	std::vector<std::string> serverNames1;
	serverNames1.push_back("Maximilian.com");
	serverNames1.push_back("maximilian.local");
	VirtServer server1(8080, serverNames1);

	std::vector<std::string> serverNames2;
	serverNames2.push_back("Vladimir.LOCAL");
	serverNames2.push_back("www.vladimir.org");
	VirtServer server2(8081, serverNames2);

	// Create Location objects for server1
	Location location1("/blog/pizza/neapolitan/", "/root/", "index.html");
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

	Location location3("/path3/", "/root3/", "index3.html");
	location3.setAutoIndex(false);
	location3.addMethod("POST");
	location3.addCGIExtension(".php");
	location3.setUploadDir("/files/");
	location3.addReturnRedir(303, "redirect3.html");

	// Add locations to respective servers
	server1.addLocation(location1);
	server2.addLocation(location2);
	server2.addLocation(location3);

	// Add VirtServer objects to the Config
	config.addVirtServer(server1);
	config.addVirtServer(server2);
	config.setTimeout(50);
	config.setMaxClients(50);
	config.setClientMaxBodySize(1000);
}

// Function to display config variables
void displayConfig(const Config &config)
{
	std::cout << "Displaying Config variables:\n";
	std::cout << "Timeout: " << config.getTimeout() << std::endl;
	std::cout << "Max Clients: " << config.getMaxClients() << std::endl;
	std::cout << "Client Max Body Size: " << config.getClientMaxBodySize() << std::endl;
}
