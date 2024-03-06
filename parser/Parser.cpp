#include "Parser.hpp"
#include <fstream>
#include <sstream>

Parser::Parser(const std::string& filename) : filename(filename) {}

bool Parser::parseFile(Config& config) {
	std::ifstream file(filename.c_str());
	if (!file.is_open()) {
		std::cerr << "Error: Unable to open file " << filename << std::endl;
		return false;
	}

	std::string line;
	while (std::getline(file, line)) {
	}

	file.close();
	return true;
}
