#include "Parser.hpp"

// Helper function to trim leading and trailing whitespace from a string
std::string Parser::trim(const std::string& str) {
	size_t start = str.find_first_not_of(" \t\r\n");
	size_t end = str.find_last_not_of(" \t\r\n");
	if (start == std::string::npos || end == std::string::npos) {
		return "";
	}
	return str.substr(start, end - start + 1);
}

bool Parser::hasDuplicateGlobalSettings() const {
	std::ifstream file(filename.c_str());

	std::map<std::string, int> globalSettingsCount;

	std::string line;
	bool inServerBlock = false;
	while (std::getline(file, line)) {
		if (line.find("<server>") != std::string::npos) {
			inServerBlock = true;
		} else if (line.find("</server>") != std::string::npos) {
			inServerBlock = false;
		} else if (!inServerBlock) { // Check if outside server block
			size_t colonPos = line.find(':');
			if (colonPos != std::string::npos) {
				std::string key = line.substr(0, colonPos);
				globalSettingsCount[key]++;
				if (globalSettingsCount[key] > 1) {
					std::cerr << "Syntax Error: Duplicate global setting found: " << key << std::endl;
					file.close();
					return true;
				}
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

// Check for duplicate ports and validate port range
bool Parser::hasInvalidPorts() const {
	std::set<int> ports; // Set to store unique ports

	std::ifstream file(filename.c_str());
	std::string line;
	while (std::getline(file, line)) {
		std::string trimmedLine = trim(line);
		std::size_t colonPos = trimmedLine.find(':');
		if (colonPos != std::string::npos) {
			std::string key = trim(trimmedLine.substr(0, colonPos));
			if (key == "listen") {
				std::string value = trim(trimmedLine.substr(colonPos + 1));
				int port = atoi(value.c_str());
				if (port <= 0 || port > 65535) {
					std::cerr << "Syntax Error: Invalid or unset port number: " << port << std::endl;
					file.close();
					return true;
				}
				if (ports.find(port) != ports.end()) {
					std::cerr << "Syntax Error: Duplicate port found: " << port << std::endl;
					file.close();
					return true;
				}
				ports.insert(port);
			}
		}
	}
	file.close();
	return false;
}

bool Parser::hasDuplicateServerNames() const {
	std::map<std::string, int> serverNamesCount;

	std::ifstream file(filename.c_str());

	std::string line;
	while (std::getline(file, line)) {
		size_t colonPos = line.find(':');
		if (colonPos != std::string::npos) {
			std::string key = trim(line.substr(0, colonPos));
			if (key == "server_name") {
				std::string value = trim(line.substr(colonPos + 1));
				std::istringstream iss(value);
				std::string serverName;
				while (std::getline(iss, serverName, ',')) {
					serverNamesCount[trim(serverName)]++;
				}
			}
		}
	}
	file.close();

	for (std::map<std::string, int>::const_iterator it = serverNamesCount.begin(); it != serverNamesCount.end(); ++it) {
		if (it->second > 1) {
			std::cerr << "Syntax Error: Duplicate server name found: " << it->first << std::endl;
			return true;
		}
	}
	return false;
}

bool Parser::hasSyntaxErrors() {
	std::ifstream file(filename.c_str());
	if (!file.is_open()) {
		std::cerr << "SyntaxError: Unable to open file " << filename << std::endl;
		return true;
	}
	bool syntaxErrors =
			hasWrongGlobalSettings(file) ||
			hasInvalidPorts() ||
			hasDuplicateServerNames() ||
			hasDuplicateGlobalSettings() ||
			hasIncorrectServerBlocks(file);
	// std::cout << "Finished checking server settings" << std::endl;
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
				int numericValue;
				sscanf(value.c_str(), "%d", &numericValue);
				// Validate the numeric value based on the setting
				if (key == "timeout" && (numericValue < 0 || numericValue > 60)) {
					std::cerr << "Syntax Error: Timeout value out of range in line: " << line << std::endl;
					return true;
				} else if (key == "max_clients" && (numericValue < 0 || numericValue > 1000)) {
					std::cerr << "Syntax Error: Max clients value out of range in line: " << line << std::endl;
					return true;
				} else if (key == "client_max_body_size" && (numericValue < 0 || numericValue > 1000000)) {
					std::cerr << "Syntax Error: Client max body size value out of range in line: " << line << std::endl;
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
	int port;
	std::vector<std::string> serverNames;
	std::string errorPage;

	bool foundListen = false;

	VirtServer virtServer; // Create an empty VirtServer object to store parsed server data

	while (std::getline(file, line)) {
		line = trim(line);

		if (line.find("</server>") != std::string::npos) {
			// Check if Port is set
			if (!foundListen) {
				std::cerr << "Error: Missing Port in server block" << std::endl;
				return false;
			}
			virtServer.setPort(port);
			virtServer.setServerNames(serverNames);
			virtServer.setErrorPage(errorPage);
			// Add the filled VirtServer object to the Config object
			config.addVirtServer(virtServer);
			return true;
		}
		// Check if the line contains the start of a location block
		if (line.find("<location>") != std::string::npos) {
			// std::cout << "Encountered location block" << std::endl;
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
			} else if (key == "error_page") {
				errorPage = value;
				}
			}
		}
	std::cerr << "Error: Server block not properly terminated" << std::endl;
	return false;
}


bool Parser::parseLocationBlock(VirtServer& virtServer, std::ifstream& file) {
	std::string line;
	std::string route;
	std::string locationRoot;
	std::string locationIndex;
	std::vector<std::string> methods;
	std::string returnURL;
	std::string returnCode;
	bool autoIndex = false;
	std::vector<std::string> cgiExtensions;
	std::string uploadDir;

	while (std::getline(file, line)) {
		// Trim leading and trailing whitespace from the line
		line = trim(line);

		// Check if we reached the end of the location block
		if (line.find("</location>") != std::string::npos) {
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

			if (key == "route") {
				route = value;
			} else if (key == "root") {
				locationRoot = value;
			} else if (key == "index") {
				locationIndex = value;
			} else if (key == "methods") {
				std::istringstream iss(value);
				std::string method;
				while (std::getline(iss, method, ',')) {
					methods.push_back(trim(method));
				}
			} else if (key == "return") {
				returnURL = value;
			} else if (key == "returnCode") {
				returnCode = value;
			} else if (key == "autoindex") {
				// std::cout << "Key: " << key << ", Value: " << value << std::endl;
				if (value == "on") {
					// std::cout << "Auto Index is ON" << std::endl;
					autoIndex = true;
				} else if (value == "off") {
					// std::cout << "Auto Index is OFF" << std::endl;
					autoIndex = false;
				} else {
					std::cerr << "Invalid value for autoindex: " << value << std::endl;
				}
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
	if (
	route.empty() &&
	locationRoot.empty() &&
	locationIndex.empty() &&
	methods.empty() &&
	returnURL.empty() &&
	returnCode.empty() &&
	cgiExtensions.empty() &&
	uploadDir.empty())
	{
		return false;
	}

	// Create a Location object with parsed values
	Location location(route, locationRoot, locationIndex);
	std::cout << "Route: " << route << std::endl;
	std::cout << "Root: " << locationRoot << std::endl;
	std::cout << "Index: " << locationIndex << std::endl;
	for (size_t i = 0; i < methods.size(); ++i) {
		std::cout << "Method: " << methods[i] << std::endl;
		location.addMethod(methods[i]);
	}
	std::cout << "Auto Index: " << (autoIndex ? "On" : "Off") << std::endl;
	location.setAutoIndex(autoIndex);
	std::cout << "returnURL: " << returnURL << std::endl;
	std::cout << "returnCode: " << returnCode << std::endl;
	for (size_t i = 0; i < cgiExtensions.size(); ++i) {
		std::cout << "CGI Extension: " << cgiExtensions[i] << std::endl;
		location.addCGIExtension(cgiExtensions[i]);
	}
	std::cout << "Upload Directory: " << uploadDir << std::endl;
	location.setUploadDir(uploadDir);

	// Add the filled Location object to the VirtServer
	virtServer.addLocation(location);

	return true;
}
