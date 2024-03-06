#include "Parser.hpp"
#include "../conf/ConfigClass.hpp"

int main() {
	Config config; // Create a Config object
	Parser parser("config.txt"); // Create a Parser object with the filename
	if (parser.parseFile(config)) {
		// If Parsing successful, you can use the filled Config object here
		config.display(); // display the filled Config object
	}

	return 0;
}
