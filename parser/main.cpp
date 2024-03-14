#include "Parser.hpp"

int main(int argc, char* argv[]) {
	std::string configFile;
	if (argc == 2) {
		configFile = argv[1];
	} else {
		configFile = "../conf/default.conf"; // Default configuration file path
	}

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

	config.display();
	return 0;
}
