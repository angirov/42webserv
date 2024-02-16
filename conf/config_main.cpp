#include <iostream>
#include "ConfigClass.hpp"

int main() {
	// Create a Config object
	Config config;

	// Set values using setter functions
	config.setTimeout(50);
	config.setMaxClients(50);
	config.setPort(5001);
	config.setServerName("localhost");
	config.setErrorPage("/404.html");
	config.setLocationRoot("/var/www/html");
	config.setLocationIndex("index.html");
	std::vector<std::string> methods;
	methods.push_back("GET");
	methods.push_back("POST");
	methods.push_back("DELETE");
	config.setMethods(methods);
	config.setCGI(".php");
	config.setUploadDir("upload");

	// Display values using getter functions
	std::cout << "Timeout: " << config.getTimeout() << std::endl;
	std::cout << "Max Clients: " << config.getMaxClients() << std::endl;
	std::cout << "Port: " << config.getPort() << std::endl;
	std::cout << "Server Name: " << config.getServerName() << std::endl;
	std::cout << "Error Page: " << config.getErrorPage() << std::endl;
	std::cout << "Location Root: " << config.getLocationRoot() << std::endl;
	std::cout << "Location Index: " << config.getLocationIndex() << std::endl;
	std::vector<std::string> retrievedMethods = config.getMethods();
	std::cout << "Methods:";
	for (size_t i = 0; i < retrievedMethods.size(); ++i) {
		std::cout << " " << retrievedMethods[i];
	}
	std::cout << std::endl;
	std::cout << "CGI: " << config.getCGI() << std::endl;
	std::cout << "Upload Directory: " << config.getUploadDir() << std::endl;

	return 0;
}
