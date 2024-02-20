#include "ConfigClass.hpp"
#include <iostream>

void setValues(Config &config, VirtServer &server, Location &location) {
	// Set Config variables
	config.setTimeout(50);
	config.setMaxClients(50);
	config.setClientMaxBodySize(1000);

	// Set VirtServer variables
	server.setPort(5001);
	server.setServerName("localhost");
	server.addErrorPage(404, "/404.html");

	// Set Location variables
	location.setRoute("/");
	location.setLocationRoot("/var/www/html");
	location.setLocationIndex("index.html");
	location.addMethod("GET");
	location.addMethod("POST");
	location.addMethod("DELETE");
	location.setAutoIndex(false);
	location.addCGIExtension(".php");
	location.setUploadDir("/upload");
}

void displayValues(const Config &config, const VirtServer &server, const Location &location) {
	// Display Config variables
	std::cout << "Config Variables:\n";
	std::cout << "Timeout: " << config.getTimeout() << std::endl;
	std::cout << "Max Clients: " << config.getMaxClients() << std::endl;
	std::cout << "Client Max Body Size: " << config.getClientMaxBodySize() << " bytes" << std::endl;

	// Display VirtServer variables
	std::cout << "\nVirtServer Variables:\n";
	std::cout << "Port: " << server.getPort() << std::endl;
	std::cout << "Server Name: " << server.getServerName() << std::endl;

	// Display error pages
	std::cout << "\nError Pages:\n";
	std::cout << "404 Error Page: " << server.getErrorPage(404) << std::endl;

	// Display Location variables
	std::cout << "\nLocation Variables:\n";
	std::cout << "Route: " << location.getRoute() << std::endl;
	std::cout << "Location Root: " << location.getLocationRoot() << std::endl;
	std::cout << "Location Index: " << location.getLocationIndex() << std::endl;
	std::cout << "Methods: ";
	const std::vector<std::string>& methods = location.getMethods();
	for (size_t i = 0; i < methods.size(); ++i) {
		std::cout << methods[i];
		if (i != methods.size() - 1) {
			std::cout << ", ";
		}
	}
	std::cout << std::endl;
	std::cout << "Auto Index: " << (location.getAutoIndex() ? "On" : "Off") << std::endl;
	std::cout << "CGI Extensions: ";
	const std::vector<std::string>& cgiExtensions = location.getCGIExtensions();
	for (size_t i = 0; i < cgiExtensions.size(); ++i) {
		std::cout << cgiExtensions[i];
		if (i != cgiExtensions.size() - 1) {
			std::cout << ", ";
		}
	}
	std::cout << std::endl;
	std::cout << "Upload Dir: " << location.getUploadDir() << std::endl;
}

int main() {
	// Create objects
	Config config;
	VirtServer server(0, "");
	Location location("", "", "");

	// Set values
	setValues(config, server, location);

	// Display values
	displayValues(config, server, location);

	return 0;
}