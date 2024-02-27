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
void createObjects(Config &config, const std::vector<std::tuple<int, std::vector<std::string>, std::vector<std::tuple<std::string, std::string, std::string, bool, std::vector<std::string>>>>>& serverConfigs)
{
	for (const auto& serverConfig : serverConfigs) {
		int port;
		std::vector<std::string> serverNames;
		std::vector<std::tuple<std::string, std::string, std::string, bool, std::vector<std::string>>> locationConfigs;

		std::tie(port, serverNames, locationConfigs) = serverConfig;

		VirtServer server(port, serverNames);

		for (const auto& locationConfig : locationConfigs) {
			std::string path, root, defaultPage;
			bool autoIndex;
			std::vector<std::string> methods;

			std::tie(path, root, defaultPage, autoIndex, methods) = locationConfig;

			Location location(path, root, defaultPage);
			location.setAutoIndex(autoIndex);
			for (const auto& method : methods) {
				location.addMethod(method);
			}

			server.addLocation(location);
		}

		config.addVirtServer(server);
	}
}
