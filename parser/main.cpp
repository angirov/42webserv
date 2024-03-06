#include "Parser.hpp"

int main(int argc, char* argv[]) {
	if (argc != 2) {
		std::cerr << "Usage: " << argv[0] << " <config_file>" << std::endl;
		return 1;
	}

	std::string configFile = argv[1];
	Config config; // Create a Config object
	config.display();
	Parser parser(configFile); // Create a Parser object with the filename
	// Parse the config file
	if (!parser.parseFile(config)) {
		std::cerr << "Error: Failed to parse config file" << std::endl;
		return 1;
	}

	// Display the values of global settings
	std::cout << "Displaying Global Settings:" << std::endl;
	std::cout << "Timeout: " << config.getTimeout() << std::endl;
	std::cout << "Max Clients: " << config.getMaxClients() << std::endl;
	std::cout << "Client Max Body Size: " << config.getClientMaxBodySize() << std::endl;

	return 0;
}
