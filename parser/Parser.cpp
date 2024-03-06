#include "Parser.hpp"

bool Parser::hasSyntaxErrors() const {
	std::ifstream file(filename.c_str());
	if (!file.is_open()) {
		std::cerr << "Error: Unable to open file " << filename << std::endl;
		return true;
	}

	std::string line;
	while (std::getline(file, line)) {
		// Trim leading and trailing whitespace from the line
		size_t start = 0;
		while (start < line.size() && std::isspace(line[start])) {
			++start;
		}
		size_t end = line.size();
		while (end > start && std::isspace(line[end - 1])) {
			--end;
		}
		std::string trimmedLine = line.substr(start, end - start);

		// Check if the line contains key-value pairs
		size_t colonPos = trimmedLine.find(':');
		if (colonPos != std::string::npos) {
			// If the line does not end with a semicolon, it is a syntax error
			if (trimmedLine[trimmedLine.size() - 1] != ';') {
				std::cerr << "Syntax Error: Setting value not closed with a semicolon: " << trimmedLine << std::endl;
				file.close();
				return true;
			}
		}
	}

	file.close();
	return false;
}


bool Parser::parseFile(Config& config) {
	std::ifstream file(filename.c_str());
	if (!file.is_open()) {
		std::cerr << "Error: Unable to open file " << filename << std::endl;
		return false;
	}

	std::string line;
	while (std::getline(file, line)) {
		// Parse global settings
		if (!parseGlobalSettings(line, config)) {
			std::cerr << "Error: Failed to parse line: " << line << std::endl;
			file.close();
			return false;
		}
	}

	file.close();
	return true;
}

bool Parser::parseGlobalSettings(const std::string& line, Config& config) {
	// Trim leading and trailing whitespace from the line
	size_t start = 0;
	while (start < line.size() && std::isspace(line[start])) {
		++start;
	}
	size_t end = line.size();
	while (end > start && std::isspace(line[end - 1])) {
		--end;
	}
	std::string trimmedLine = line.substr(start, end - start);

	// Check if the line contains global settings
	size_t colonPos = trimmedLine.find(':');
	if (colonPos != std::string::npos) {
		// Extract setting name and value
		std::string settingName = trimmedLine.substr(0, colonPos);
		std::string settingValue = trimmedLine.substr(colonPos + 1, trimmedLine.size() - colonPos - 2); // Exclude colon and semicolon

		// Trim leading and trailing whitespace from setting value
		start = 0;
		while (start < settingValue.size() && std::isspace(settingValue[start])) {
			++start;
		}
		end = settingValue.size();
		while (end > start && std::isspace(settingValue[end - 1])) {
			--end;
		}
		settingValue = settingValue.substr(start, end - start);

		// Set the parsed value in the Config object
		if (settingName == "timeout") {
			int timeout;
			std::istringstream iss(settingValue);
			if (!(iss >> timeout)) {
				std::cerr << "Error: Invalid value for timeout setting: " << settingValue << std::endl;
				return false;
			}
			config.setTimeout(timeout);
		} else if (settingName == "max_clients") {
			int maxClients;
			std::istringstream iss(settingValue);
			if (!(iss >> maxClients)) {
				std::cerr << "Error: Invalid value for max_clients setting: " << settingValue << std::endl;
				return false;
			}
			config.setMaxClients(maxClients);
		} else if (settingName == "client_max_body_size") {
			int clientMaxBodySize;
			std::istringstream iss(settingValue);
			if (!(iss >> clientMaxBodySize)) {
				std::cerr << "Error: Invalid value for client_max_body_size setting: " << settingValue << std::endl;
				return false;
			}
			config.setClientMaxBodySize(clientMaxBodySize);
		}

		return true; // Successfully parsed global settings
	}

	return false; // Line does not contain global settings
}
