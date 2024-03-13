#include "Parser.hpp"

// Helper function to trim leading and trailing whitespace from a string
std::string Parser::trim(const std::string& str) {
	size_t start = 0, end = str.length();
	while (start < end && std::isspace(str[start])) {
		++start;
	}
	while (end > start && std::isspace(str[end - 1])) {
		--end;
	}
	return str.substr(start, end - start);
}

// Helper function isNumeric
bool Parser::isNumeric(const std::string& str) {
	for (size_t i = 0; i < str.length(); ++i) {
		if (!isdigit(str[i])) {
			return false;
		}
	}
	return true;
}
// Helper function splitString
void Parser::splitString(const std::string& input, char delimiter, std::vector<std::string>& tokens) const {
	std::string::size_type start = 0;
	std::string::size_type end = input.find(delimiter);

	while (end != std::string::npos) {
		tokens.push_back(input.substr(start, end - start));
		start = end + 1;
		end = input.find(delimiter, start);
	}

	tokens.push_back(input.substr(start));
}


bool Parser::isValidServerNameFormat(const std::string& value) {
	std::istringstream iss(value);
	std::string serverName;
	while (std::getline(iss, serverName, ',')) {
		if (serverName.empty()) {
			// Empty server name found
			std::cerr << "Syntax Error: Empty server name found" << std::endl;
			return false;
		}
	}
	return true;
}

bool Parser::hasDuplicateSettings() const {
	std::ifstream file(filename.c_str());

	std::map<std::string, int> globalSettingsCount;
	std::map<std::string, int> serverSettingsCount;
	bool inServerBlock = false;

	std::string line;
	while (std::getline(file, line)) {
		if (line.find("<server>") != std::string::npos) {
			inServerBlock = true;
			serverSettingsCount.clear(); // Clear the settings count for each new server block
		} else if (line.find("</server>") != std::string::npos) {
			inServerBlock = false;
		} else {
			size_t colonPos = line.find(':');
			if (colonPos != std::string::npos) {
				std::string key = line.substr(0, colonPos);
				if (inServerBlock) {
					serverSettingsCount[key]++;
					if (serverSettingsCount[key] > 1) {
						std::cerr << "Syntax Error: Duplicate setting found within server block: " << key << std::endl;
						file.close();
						return true;
					}
				} else {
					globalSettingsCount[key]++;
					if (globalSettingsCount[key] > 1) {
						std::cerr << "Syntax Error: Duplicate global setting found: " << key << std::endl;
						file.close();
						return true;
					}
				}
			}
		}
	}
	file.close();
	return false;
}

bool Parser::hasMissingSemicolons() const {
	std::ifstream file(filename.c_str());

	std::string line;
	while (std::getline(file, line)) {
		size_t colonPos = line.find(':');
		if (colonPos != std::string::npos) {
			if (line[line.size() - 1] != ';') {
				std::cerr << "Syntax Error: Missing semicolon at the end of line: " << line << std::endl;
				file.close();
				return true;
			}
		}
	}

	file.close();
	return false;
}

bool Parser::hasIncorrectServerBlocks(std::ifstream& file) {

	bool inServerBlock = false;

	std::string line;
	while (std::getline(file, line)) {
		if (line.find("<server>") != std::string::npos) {
			if (inServerBlock) {
				std::cerr << "Syntax Error: Nested server blocks are not allowed" << std::endl;
				file.close();
				return true;
			}
			inServerBlock = true;
		} else if (line.find("</server>") != std::string::npos) {
			if (!inServerBlock) {
				std::cerr << "Syntax Error: Missing opening <server> tag" << std::endl;
				file.close();
				return true;
			}
			inServerBlock = false;
		}
	}

	if (inServerBlock) {
		std::cerr << "Syntax Error: Missing closing </server> tag" << std::endl;
		file.close();
		return true;
	}

	file.close();
	return false;
}

bool Parser::hasSyntaxErrors() {
	std::ifstream file(filename.c_str());
	if (!file.is_open()) {
		std::cerr << "SyntaxError: Unable to open file " << filename << std::endl;
		return true;
	}
	bool syntaxErrors =
			// hasDuplicateSettings() ||
			hasMissingSemicolons() ||
			hasWrongGlobalSettings(file) ||
			hasIncorrectServerBlocks(file);
	std::cout << "Finished checking server settings" << std::endl;
	file.close();
	return syntaxErrors;
}

bool Parser::hasWrongGlobalSettings(std::ifstream& file) {
	const char* allowedGlobalSettings[] = {"timeout", "max_clients", "client_max_body_size"};
	bool inServerBlock = false;

	std::string line;
	while (std::getline(file, line)) {
		if (line.find("<server>") != std::string::npos) {
			inServerBlock = true;
			continue;
		} else if (line.find("</server>") != std::string::npos) {
			inServerBlock = false;
			continue;
		}

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
			std::string key = trimmedLine.substr(0, colonPos);
			if (!inServerBlock && !key.empty()) {
				bool isKnown = false;
				for (size_t i = 0; i < sizeof(allowedGlobalSettings) / sizeof(allowedGlobalSettings[0]); ++i) {
					if (key == allowedGlobalSettings[i]) {
						isKnown = true;
						break;
					}
				}
				if (!isKnown) {
					std::cerr << "Syntax Error: Unknown global setting found in line: " << line << ", key: " << key << std::endl;
					return true;
				}

				// Extract the value part of the key-value pair
				std::string value = trimmedLine.substr(colonPos + 1);

				// Trim the value if it ends with a semicolon
				if (!value.empty() && value[value.size() - 1] == ';') {
					value = trim(value.substr(0, value.size() - 1)); // Remove the semicolon before trimming
				}

				// Check if the value is numerical
				if (!value.empty() && !isNumeric(value)) {
					std::cerr << "Syntax Error: Invalid value for " << key << ": " << value << ". Value must be numerical." << std::endl;
					return true;
				}
			}
		}
	}

	return false;
}

bool Parser::parseFile(Config& config) {
	std::ifstream file(filename.c_str());
	if (!file.is_open()) {
		std::cerr << "Error: Unable to open file " << filename << std::endl;
		return false;
	}

	std::string line;
	int lineNumber = 0;
	while (std::getline(file, line)) {
		++lineNumber;
		// Check if the line contains the start of a server block
		if (line.find("<server>") != std::string::npos) {
			// Parse the server block
			if (!parseServerBlock(config, file)) {
				std::cerr << "Error: Failed to parse server block at line " << lineNumber << std::endl;
				file.close();
				return false;
			}
		} else {
			// Parse global settings
			if (!parseGlobalSettings(line, config)) {
				std::cerr << "Error: Failed to parse line " << lineNumber << ": " << line << std::endl;
				file.close();
				return false;
			}
		}
	}
	file.close();
	return true;
}

bool Parser::parseGlobalSettings(const std::string& line, Config& config) {
	// Skip empty lines
	if (line.empty()) {
		return true;
	}

	// Trim leading and trailing whitespace from the line
	std::string trimmedLine = trim(line);

	// Check if the line contains global settings
	size_t colonPos = trimmedLine.find(':');
	if (colonPos != std::string::npos) {
		// Extract setting name and value
		std::string settingName = trimmedLine.substr(0, colonPos);
		std::string settingValue = trimmedLine.substr(colonPos + 1);

		// Trim leading and trailing whitespace from setting value
		settingValue = trim(settingValue);

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


bool Parser::parseServerBlock(Config& config, std::ifstream& file) {
	std::string line;
	int port = 0;
	std::vector<std::string> serverNames;
	std::map<int, std::string> errorPages;

	bool foundListen = false;
	bool foundServerName = false;
	bool foundErrorPage = false;

	VirtServer virtServer; // Create an empty VirtServer object to store parsed server data

	while (std::getline(file, line)) {
		line = trim(line);

		if (line.find("</server>") != std::string::npos) {
			// Add the filled VirtServer object to the Config object
			if (!foundListen || !foundServerName || !foundErrorPage) {
				std::cerr << "Error: Missing essential variables in server block" << std::endl;
				return false;
			}

			virtServer.setPort(port);
			virtServer.setServerNames(serverNames);
			for (std::map<int, std::string>::const_iterator it = errorPages.begin(); it != errorPages.end(); ++it) {
				virtServer.setErrorPage(it->first, it->second);
			}
			config.addVirtServer(virtServer); // Add the filled VirtServer object to the Config object

			return true;
		}

		// Check if the line contains the start of a location block
		if (line.find("<location>") != std::string::npos) {
			std::cout << "Encountered location block, parsing..." << std::endl;
			// Parse the location block and add it to the current VirtServer object
			if (!parseLocationBlock(virtServer, file)) {
				return false;
			}
		}

		size_t colonPos = line.find(':');
		if (colonPos != std::string::npos) {
			std::string key = trim(line.substr(0, colonPos));
			std::string value = trim(line.substr(colonPos + 1));

			if (key == "listen") {
				port = atoi(value.c_str());
				foundListen = true;
			} else if (key == "server_name") {
				std::istringstream iss(value);
				std::string serverName;
				while (std::getline(iss, serverName, ',')) {
					serverNames.push_back(trim(serverName));
				}
				foundServerName = true;
			} else if (key == "error_page") {
				size_t commaPos = value.find(',');
				if (commaPos != std::string::npos) {
					int errorCode = atoi(value.substr(0, commaPos).c_str());
					std::string pageURL = value.substr(commaPos + 1);
					errorPages[errorCode] = pageURL;
				}
				foundErrorPage = true;
			}
		}
	}

	std::cerr << "Error: Server block not properly terminated" << std::endl;
	return false; // Server block not properly terminated
}


bool Parser::parseLocationBlock(VirtServer& virtServer, std::ifstream& file) {
	std::string line;
	std::string route;
	std::string locationRoot;
	std::string locationIndex;
	std::vector<std::string> methods;
	std::map<int, std::string> returnRedir;
	bool autoIndex = false;
	std::vector<std::string> cgiExtensions;
	std::string uploadDir;

	std::cout << "Parsing location block..." << std::endl;

	while (std::getline(file, line)) {
		// Trim leading and trailing whitespace from the line
		line = trim(line);

		// Check if we reached the end of the location block
		if (line.find("</location>") != std::string::npos) {
			std::cout << "End of location block" << std::endl;
			break; // End of location block, break out of loop
		}

		// Parse key-value pairs inside the location block
		size_t colonPos = line.find(':');
		if (colonPos != std::string::npos) {
			std::string key = line.substr(0, colonPos);
			std::string value = line.substr(colonPos + 1);

			// Trim leading and trailing whitespace from the key and value
			key = trim(key);
			value = trim(value);

			// Check the key and set the corresponding value
			if (key == "route") {
				route = value;
			} else if (key == "root") {
				locationRoot = value;
			} else if (key == "index") {
				locationIndex = value;
			} else if (key == "methods") {
				// Remove any trailing semicolon from the value
				if (!value.empty() && value[value.size() - 1] == ';') {
					value.erase(value.size() - 1);
				}
				// Split the value by commas and add each method to the methods vector
				std::istringstream iss(value);
				std::string method;
				while (std::getline(iss, method, ',')) {
					methods.push_back(trim(method));
				}
			} else if (key == "return") {
				// Parse the return value for error codes and redirect URLs
				// Assuming format: errorCode,redirectUrl;
				size_t commaPos = value.find(',');
				if (commaPos != std::string::npos) {
					int errorCode = atoi(value.substr(0, commaPos).c_str());
					std::string redirectUrl = value.substr(commaPos + 1);
					returnRedir[errorCode] = redirectUrl;
				}
			} else if (key == "autoindex") {
				autoIndex = (value == "on") ? true : false;
			} else if (key == "cgi") {
				// Split the value by commas and add each CGI extension to the cgiExtensions vector
				std::istringstream iss(value);
				std::string cgiExtension;
				while (std::getline(iss, cgiExtension, ',')) {
					cgiExtensions.push_back(trim(cgiExtension));
				}
			} else if (key == "uploadDir") {
				uploadDir = value;
			}
		}
	}

	// Check if any key-value pairs were parsed
	if (route.empty() && locationRoot.empty() && locationIndex.empty() && methods.empty() && returnRedir.empty() && cgiExtensions.empty() && uploadDir.empty()) {
		// No valid key-value pairs were parsed, indicating the end of the location block or an empty location block
		return false;
	}

	// Create a Location object with parsed values
	Location location(route, locationRoot, locationIndex);
	for (size_t i = 0; i < methods.size(); ++i) {
		location.addMethod(methods[i]);
	}
	std::map<int, std::string>::const_iterator it;
	for (it = returnRedir.begin(); it != returnRedir.end(); ++it) {
		location.setReturnRedir(it->first, it->second);
	}
	for (size_t i = 0; i < cgiExtensions.size(); ++i) {
		location.addCGIExtension(cgiExtensions[i]);
	}
	location.setUploadDir(uploadDir);
	location.setAutoIndex(autoIndex);

	// Add the filled Location object to the VirtServer
	virtServer.addLocation(location);

	return true;
}



