#include "Parser.hpp"

int main(int argc, char* argv[]) {
	std::string configFile;
	if (argc == 2) {
		configFile = argv[1];
	} else {
		configFile = "../conf/default.conf"; // Default configuration file path
	}

	Config config; // Create a Config object
	Parser parser(configFile); // Create a Parser object with the filename

	// Perform syntax check
	if (parser.hasSyntaxErrors()) {
		std::cerr << "Error: Syntax errors found in config file" << std::endl;
		return 1;
	}

	// Parse the config file
	if (!parser.parseFile(config)) {
		std::cerr << "Error: Failed to parse config file" << std::endl;
		return 1;
	}

	// Display the global settings
	std::cout << "Displaying Global Settings:" << std::endl;
	std::cout << "Timeout: " << config.getTimeout() << std::endl;
	std::cout << "Max Clients: " << config.getMaxClients() << std::endl;
	std::cout << "Client Max Body Size: " << config.getClientMaxBodySize() << std::endl;

	// Display the parsed virtual servers
	std::cout << "Displaying Virtual Server Settings:" << std::endl;
	const std::vector<VirtServer>& virtServers = config.getVirtServers();
	for (size_t i = 0; i < virtServers.size(); ++i) {
		std::cout << "Virtual Server " << i + 1 << ":\n";
		std::cout << "Port: " << virtServers[i].getPort() << std::endl;
		std::cout << "Server Names: ";
		const std::vector<std::string>& serverNames = virtServers[i].getServerNames();
		for (size_t j = 0; j < serverNames.size(); ++j) {
			std::cout << serverNames[j];
			if (j != serverNames.size() - 1) {
				std::cout << ", ";
			}
		}
		std::cout << std::endl;

		// Display error pages
		std::cout << "Error Pages:" << std::endl;
		for (int errorCode = 400; errorCode < 600; ++errorCode) {
			const std::string& errorPage = virtServers[i].getErrorPage(errorCode);
			if (!errorPage.empty()) {
				std::cout << "Error " << errorCode << ": " << errorPage << std::endl;
			}
		}

		// Display locations
		std::cout << "Locations:" << std::endl;
		const std::vector<Location>& locations = virtServers[i].getLocations();
		for (size_t k = 0; k < locations.size(); ++k) {
			std::cout << "Location " << k + 1 << ":\n";
			std::cout << "Route: " << locations[k].getRoute() << std::endl;
			std::cout << "Root: " << locations[k].getLocationRoot() << std::endl;
			std::cout << "Index: " << locations[k].getLocationIndex() << std::endl;
			std::cout << "Auto Index: " << (locations[k].getAutoIndex() ? "On" : "Off") << std::endl;

			// Display methods
			std::cout << "Methods: ";
			const std::vector<Method>& methods = locations[k].getMethods();
			for (size_t m = 0; m < methods.size(); ++m) {
				std::cout << toStr(methods[m]);
				if (m != methods.size() - 1) {
					std::cout << ", ";
				}
			}
			std::cout << std::endl;

			// Display CGI extensions
			std::cout << "CGI Extensions: ";
			const std::vector<std::string>& cgiExtensions = locations[k].getCGIExtensions();
			for (size_t n = 0; n < cgiExtensions.size(); ++n) {
				std::cout << cgiExtensions[n];
				if (n != cgiExtensions.size() - 1) {
					std::cout << ", ";
				}
			}
			std::cout << std::endl;

			// Display upload directory
			std::cout << "Upload Directory: " << locations[k].getUploadDir() << std::endl;

			// Display error redirects
			for (int errorCode = 400; errorCode < 600; ++errorCode) {
				const std::string& redirectUrl = locations[k].getReturnRedir(errorCode);
				if (!redirectUrl.empty()) {
					std::cout << "Error " << errorCode << " Redirect: " << redirectUrl << std::endl;
				}
			}

			std::cout << std::endl;
		}
	}

	return 0;
}
