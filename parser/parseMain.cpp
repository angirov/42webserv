#include <iostream>
#include <fstream>
#include "Parser.hpp" // Include your parser header file here

int main(int argc, char* argv[]) {
	if (argc != 2) {
		std::cerr << "Usage: " << argv[0] << " <config_file>" << std::endl;
		return 1;
	}

	std::ifstream configFile(argv[1]);
	if (!configFile.is_open()) {
		std::cerr << "Error: Unable to open file " << argv[1] << std::endl;
		return 1;
	}

	VirtServer virtServer(0, std::vector<std::string>()); // Create a VirtServer object

	Parser parser(argv[1]); // Create a Parser object with the filename

	// Parse location blocks until the end of the file is reached or an error occurs
	while (parser.parseLocationBlock(virtServer, configFile)) {
		// Check for errors during parsing
		if (configFile.eof() || configFile.fail()) {
			break; // Exit the loop if end of file or parsing error occurs
		}
	}

	configFile.close();

	// Display the contents of the VirtServer object
	std::cout << "Displaying VirtServer Contents:" << std::endl;
	virtServer.display();

	return 0;
}
