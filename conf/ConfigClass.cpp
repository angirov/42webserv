#include "ConfigClass.hpp"

Config::Config() {
	// Initialize default values
	_timeout = 0;
	_maxClients = 0;
	_port = 0;
}

// Setter functions
void Config::setTimeout(int timeout) {
	this->_timeout = timeout;
}

void Config::setMaxClients(int maxClients) {
	this->_maxClients = maxClients;
}

void Config::setPort(int port) {
	this->_port = port;
}

void Config::setServerName(const std::string& serverName) {
	this->_serverName = serverName;
}

void Config::setErrorPage(const std::string& errorPage) {
	this->_errorPage = errorPage;
}

void Config::setLocationRoot(const std::string& locationRoot) {
	this->_locationRoot = locationRoot;
}

void Config::setLocationIndex(const std::string& locationIndex) {
	this->_locationIndex = locationIndex;
}

void Config::setMethods(const std::vector<std::string>& methods) {
	this->_methods = methods;
}

void Config::setCGI(const std::string& cgi) {
	this->_cgi = cgi;
}

void Config::setUploadDir(const std::string& uploadDir) {
	this->_uploadDir = uploadDir;
}

// Getter functions
int Config::getTimeout() const {
	return _timeout;
}

int Config::getMaxClients() const {
	return _maxClients;
}

int Config::getPort() const {
	return _port;
}

std::string Config::getServerName() const {
	return _serverName;
}

std::string Config::getErrorPage() const {
	return _errorPage;
}

std::string Config::getLocationRoot() const {
	return _locationRoot;
}

std::string Config::getLocationIndex() const {
	return _locationIndex;
}

std::vector<std::string> Config::getMethods() const {
	return _methods;
}

std::string Config::getCGI() const {
	return _cgi;
}

std::string Config::getUploadDir() const {
	return _uploadDir;
}
