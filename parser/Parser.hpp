#ifndef PARSER_HPP
#define PARSER_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cctype>
#include <vector>
#include "../conf/ConfigClass.hpp"

class Parser {
public:
	Parser(const std::string& filename) : filename(filename) {}

	bool hasSyntaxErrors();
	bool parseFile(Config& config);

private:
	std::string filename;

	bool parseGlobalSettings(const std::string& line, Config& config);
	bool parseServerBlock(Config& config, std::ifstream& file);

	bool hasDuplicateSettings() const;
	bool hasMissingSemicolons() const;
	bool hasWrongGlobalSettings(std::ifstream& file);
	bool hasIncorrectServerBlocks(std::ifstream& file);

	bool isValidServerNameFormat(const std::string& value) ;

	std::string trim(const std::string& str);
	bool isNumeric(const std::string& str);
};

#endif // PARSER_HPP
