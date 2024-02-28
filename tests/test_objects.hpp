#include <iostream>
#include <vector>
#include "../conf/ConfigClass.hpp"


// Function to display config variables
void displayConfig(const Config &config)
{
	std::cout << "Displaying Config variables:\n";
	std::cout << "Timeout: " << config.getTimeout() << std::endl;
	std::cout << "Max Clients: " << config.getMaxClients() << std::endl;
	std::cout << "Client Max Body Size: " << config.getClientMaxBodySize() << std::endl;
}

// Function to create objects with configurable parameters
void createObjects(Config &config, const std::vector<std::tuple<int, std::vector<std::string>, std::map<int, std::string>, std::vector<std::tuple<std::string, std::string, std::string, bool, std::vector<std::string>, std::map<int, std::string>, std::vector<std::string>, std::string>>>>& serverConfigs)
{
	for (const auto& serverConfig : serverConfigs) {
		int port;
		std::vector<std::string> serverNames;
		std::map<int, std::string> errorPages;
		std::vector<std::tuple<std::string, std::string, std::string, bool, std::vector<std::string>, std::map<int, std::string>, std::vector<std::string>, std::string>> locationConfigs;

		std::tie(port, serverNames, errorPages, locationConfigs) = serverConfig;

		VirtServer server(port, serverNames);

		// Set error pages for the server
		for (const auto& errorPage : errorPages) {
			server.setErrorPage(errorPage.first, errorPage.second);
		}

		for (const auto& locationConfig : locationConfigs) {
			std::string path, root, defaultPage, uploadDir;
			bool autoIndex;
			std::vector<std::string> methods, cgiExtensions;
			std::map<int, std::string> returnRedir;

			std::tie(path, root, defaultPage, autoIndex, methods, returnRedir, cgiExtensions, uploadDir) = locationConfig;

			Location location(path, root, defaultPage);
			location.setAutoIndex(autoIndex);
			for (const auto& method : methods) {
				location.addMethod(method);
			}

			// Set additional location variables
			for (const auto& redirect : returnRedir) {
				location.setReturnRedir(redirect.first, redirect.second);
			}
			for (const auto& cgiExtension : cgiExtensions) {
				location.addCGIExtension(cgiExtension);
			}
			location.setUploadDir(uploadDir);

			server.addLocation(location);
		}

		config.addVirtServer(server);
	}
}


