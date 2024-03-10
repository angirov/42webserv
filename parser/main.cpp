#include "Parser.hpp"

int main(int argc, char* argv[]) {
	if (argc != 2) {
		std::cerr << "Usage: " << argv[0] << " <config_file>" << std::endl;
		return 1;
	}

	std::string configFile = argv[1];
	Config config; // Create a Config object
	Parser parser(configFile); // Create a Parser object with the filename

	// Perform syntax check
	if (parser.hasSyntaxErrors()) {
		std::cerr << "Error: Syntax errors found in config file" << std::endl;
		return 1;
	}

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

	// Display the parsed virtual servers
	const std::vector<VirtServer>& virtServers = config.getVirtServers();
	for (size_t i = 0; i < virtServers.size(); ++i) {
		std::cout << "Virtual Server " << i + 1 << ":\n";
		std::cout << "Port: " << virtServers[i].getPort() << std::endl;
		std::cout << "Server Names: ";
		const std::vector<std::string>& serverNames = virtServers[i].getServerNames();
		for (size_t j = 0; j < serverNames.size(); ++j) {
			std::cout << serverNames[j];
			if (j != serverNames.size() - 1) {
				std::cout << ", ";
			}
		}
		std::cout << std::endl;
	}

	return 0;
}
