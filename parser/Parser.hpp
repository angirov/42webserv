#ifndef PARSER_HPP
#define PARSER_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cctype>
#include "../conf/ConfigClass.hpp"

class Parser {
public:
	Parser(const std::string& filename) : filename(filename) {}

	bool hasSyntaxErrors() const;
	bool parseFile(Config& config);

private:
	std::string filename;

	bool parseGlobalSettings(const std::string& line, Config& config);
	bool parseServerBlock(Config& config, std::ifstream& file);
};

#endif // PARSER_HPP
