#ifndef PARSER_HPP
#define PARSER_HPP

#include "../conf/ConfigClass.hpp"

class Parser {
public:
	Parser(const std::string& filename);
	bool parseFile(Config& config);

private:
	std::string filename;
};

#endif // PARSER_HPP
