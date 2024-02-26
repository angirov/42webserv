#include "ConfigClass.hpp"

// Config class implementation with global settings
Config::Config() {
	_timeout = 0;
	_maxClients = 0;
	_client_max_body_size = 0;
}

// Copy Constructor
Config::Config(const Config &other) {
	_timeout = other._timeout;
	_maxClients = other._maxClients;
	_client_max_body_size = other._client_max_body_size;
	virtServers = other.virtServers; // Copy the vector of VirtServer objects
}

// Setter and Getter functions for Config class
void Config::setTimeout(int timeout) {
	_timeout = timeout;
}

int Config::getTimeout() const {
	return _timeout;
}

void Config::setMaxClients(int maxClients) {
	_maxClients = maxClients;
}

int Config::getMaxClients() const {
	return _maxClients;
}

void Config::setClientMaxBodySize(int maxBodySize) {
	_client_max_body_size = maxBodySize;
}

int Config::getClientMaxBodySize() const {
	return _client_max_body_size;
}

void Config::addVirtServer(const VirtServer &virtServer) {
	virtServers.push_back(virtServer); // Add a VirtServer object to the vector
}

// display Config implementation
void Config::display() const {
	std::cout << "Displaying Config variables:" << std::endl;
	std::cout << "Timeout: " << getTimeout() << std::endl;
	std::cout << "Max Clients: " << getMaxClients() << std::endl;
	std::cout << "Client Max Body Size: " << getClientMaxBodySize() << std::endl;

	// Display VirtServer objects
	const std::vector<VirtServer>& servers = getVirtServers();
	for (size_t i = 0; i < servers.size(); ++i) {
		servers[i].display();
	}
}

const std::vector<VirtServer>& Config::getVirtServers() const {
	return virtServers; // Return the vector of VirtServer objects
}

// Constructor
VirtServer::VirtServer(int port, const std::vector<std::string> &serverNames) {
	_port = port;
	_serverNames = serverNames;
}

// Copy Constructor
VirtServer::VirtServer(const VirtServer &other) {
	_port = other._port;
	_serverNames = other._serverNames;
	_errorPages = other._errorPages;
	locations = other.locations;
}

// Setter and Getter functions for VirtServer class
void VirtServer::setPort(int port) {
	_port = port;
}

int VirtServer::getPort() const {
	return _port;
}

void VirtServer::setServerNames(const std::vector<std::string> &serverNames) {
	_serverNames = serverNames;
}

const std::vector<std::string> & VirtServer::getServerNames() const {
	return _serverNames;
}

void VirtServer::addErrorPage(int errorCode, const std::string &errorPage) {
	_errorPages[errorCode] = errorPage;
}

const std::string &VirtServer::getErrorPage(int errorCode) const {
	std::map<int, std::string>::const_iterator it = _errorPages.find(errorCode);
	if (it != _errorPages.end()) {
		return it->second;
	}
	static const std::string emptyString = "";
	return emptyString;
}

void VirtServer::addLocation(const Location &location) {
	locations.push_back(location);
}

const std::vector<Location> &VirtServer::getLocations() const {
	return locations;
}

// Location class implementation
// Constructor
Location::Location(const std::string &route, const std::string &locationRoot, const std::string &locationIndex) {
	_locationRoot = locationRoot;
	_locationIndex = locationIndex;
	_autoIndex = false;
	_route = route;
}

// Copy Constructor
Location::Location(const Location &other) {
	_returnRedir = other._returnRedir;
	_route = other._route;
	_locationRoot = other._locationRoot;
	_locationIndex = other._locationIndex;
	_methods = other._methods;
	_cgiExtension = other._cgiExtension;
	_uploadDir = other._uploadDir;
	_autoIndex = other._autoIndex;
}

// Setter and Getter functions for Location class
void Location::setLocationRoot(const std::string &locationRoot) {
	_locationRoot = locationRoot;
}

const std::string& Location::getLocationRoot() const {
	return _locationRoot;
}

void Location::setLocationIndex(const std::string &locationIndex) {
	_locationIndex = locationIndex;
}

const std::string& Location::getLocationIndex() const {
	return _locationIndex;
}

void Location::setAutoIndex(bool autoIndex) {
	_autoIndex = autoIndex;
}

bool Location::getAutoIndex() const {
	return _autoIndex;
}

void Location::setRoute(const std::string &route) {
	_route = route;
}

const std::string &Location::getRoute() const {
	return _route;
}

void Location::addMethod(const std::string &method) {
	_methods.push_back(method);
}

const std::vector<std::string> &Location::getMethods() const {
	return _methods;
}

void Location::addCGIExtension(const std::string &cgiExtension) {
	_cgiExtension.push_back(cgiExtension);
}

const std::vector<std::string> &Location::getCGIExtensions() const {
	return _cgiExtension;
}

void Location::setUploadDir(const std::string &uploadDir) {
	_uploadDir = uploadDir;
}

const std::string& Location::getUploadDir() const {
	return _uploadDir;
}

void Location::addReturnRedir(int errorCode, const std::string &redirectUrl) {
	_returnRedir[errorCode] = redirectUrl;
}

const std::string &Location::getReturnRedir(int errorCode) const {
	std::map<int, std::string>::const_iterator it = _returnRedir.find(errorCode);
	if (it != _returnRedir.end()) {
		return it->second;
	}
	static const std::string emptyString = "";
	return emptyString;
}

void VirtServer::display() const {
	// Display VirtServer variables
	std::cout << "\nVirtServer Variables:\n";
	std::cout << "Port: " << getPort() << std::endl;
	std::cout << "Server Names: ";
	const std::vector<std::string>& serverNames = getServerNames();
	for (size_t i = 0; i < serverNames.size(); ++i) {
		std::cout << serverNames[i];
		if (i != serverNames.size() - 1) {
			std::cout << ", ";
		}
	}
	std::cout << std::endl;

	// Display error pages
	std::cout << "\nError Pages:\n";
	std::cout << "404 Error Page: " << getErrorPage(404) << std::endl;

	// Display locations
	for (size_t i = 0; i < locations.size(); ++i) {
		locations[i].display();
	}
}

void Location::display() const {
	// Display Location variables
	std::cout << "\nLocation Variables:\n";
	std::cout << "Route: " << getRoute() << std::endl;
	std::cout << "Location Root: " << getLocationRoot() << std::endl;
	std::cout << "Location Index: " << getLocationIndex() << std::endl;
	std::cout << "Methods: ";
	const std::vector<std::string>& methods = getMethods();
	for (size_t i = 0; i < methods.size(); ++i) {
		std::cout << methods[i];
		if (i != methods.size() - 1) {
			std::cout << ", ";
		}
	}
	std::cout << std::endl;
	std::cout << "Auto Index: " << (getAutoIndex() ? "On" : "Off") << std::endl;
	std::cout << "CGI Extensions: ";
	const std::vector<std::string>& cgiExtensions = getCGIExtensions();
	for (size_t i = 0; i < cgiExtensions.size(); ++i) {
		std::cout << cgiExtensions[i];
		if (i != cgiExtensions.size() - 1) {
			std::cout << ", ";
		}
	}
	std::cout << std::endl;
	std::cout << "Upload Dir: " << getUploadDir() << std::endl;
}
