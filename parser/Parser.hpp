#ifndef PARSER_HPP
#define PARSER_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cctype>
#include <vector>
#include <set>
#include "../conf/ConfigClass.hpp"

class Parser {
public:
	explicit Parser(const std::string& filename) : filename(filename) {}

	static bool parseGlobalSettings(const std::string& line, Config& config);
	static bool parseServerBlock(Config& config, std::ifstream& file);
	static bool parseLocationBlock(VirtServer& virtServer, std::ifstream& file);

	bool hasSyntaxErrors();
	bool parseFile(Config& config);

	static std::string trim(const std::string& str);

private:
	std::string filename;

	bool hasDuplicateGlobalSettings() const;
	bool hasInvalidPorts() const;
	bool hasDuplicateServerNames() const;
	static bool hasWrongGlobalSettings(std::ifstream& file);
	static bool hasIncorrectServerBlocks(std::ifstream& file);

};

#endif // PARSER_HPP
