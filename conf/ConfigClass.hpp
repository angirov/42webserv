#ifndef CONFIGCLASS_HPP
#define CONFIGCLASS_HPP

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include "../utils.hpp"

class Location;
class VirtServer;
typedef std::vector<VirtServer>::const_iterator vsIt;
typedef std::vector<Location>::const_iterator locIt;

static const vsIt notFoundVirtServer;
static const std::vector<vsIt> notFoundVirtServerVec;
static const std::vector<std::string> notFoundStrVec;

class Config {
public:
	Config();
	Config(const Config &other); // Copy constructor

	// Setter and Getter functions for Config class
	void setTimeout(int timeout);
	int getTimeout() const;

	void setMaxClients(int maxClients);
	int getMaxClients() const;

	void setClientMaxBodySize(int maxBodySize);
	int getClientMaxBodySize() const;

	void addVirtServer(const VirtServer &virtServer); // Function to add a VirtServer object
	const std::vector<VirtServer> &getVirtServers() const; // Function to get the vector of VirtServer objects

	// Display method
	void display() const;

private:
	int _timeout;
	int _maxClients;
	int _client_max_body_size;
	std::vector<VirtServer> virtServers; // Vector to store VirtServer objects
};

class VirtServer {
public:
	VirtServer(int port, const std::vector<std::string> &serverNames);
	// Copy constructor
	VirtServer(const VirtServer &other);

	// Setter and Getter functions for VirtServer class
	void setPort(int port);
	int getPort() const;

	void setServerNames(const std::vector<std::string> &serverNames);
	const std::vector<std::string> & getServerNames() const;

	void setErrorPage(int errorCode, const std::string &errorPage);
	const std::string & getErrorPage(int errorCode) const;

	void addLocation(const Location &location);
	const std::vector<Location> & getLocations() const;

	void display() const;

private:
	int _port;
	std::vector<std::string> _serverNames;
	std::map<int, std::string> _errorPages;
	std::vector<Location> locations;
};

class Location {
public:
	Location(const std::string &route, const std::string &locationRoot, const std::string &locationIndex);
	// Copy constructor
	Location(const Location &other);

	// Setter and Getter functions for Location class
	void setRoute(const std::string &route);
	const std::string & getRoute() const;

	void setLocationRoot(const std::string &locationRoot);
	const std::string & getLocationRoot() const;

	void setLocationIndex(const std::string &locationIndex);
	const std::string & getLocationIndex() const;

	void setAutoIndex(bool autoIndex);
	bool getAutoIndex() const;

	void addMethod(const std::string &method);
	const std::vector<Method> & getMethods() const;

	void addCGIExtension(const std::string &cgiExtension);
	const std::vector<std::string> & getCGIExtensions() const;

	void setUploadDir(const std::string &uploadDir);
	const std::string & getUploadDir() const;

	void setReturnRedir(int errorCode, const std::string &redirectUrl);
	const std::string& getReturnRedir(int errorCode) const;

	void display() const;

private:
	std::map<int, std::string> _returnRedir;
	std::string _route;
	std::string _locationRoot;
	std::string _locationIndex;
	std::vector<Method> _methods;
	std::vector<std::string> _cgiExtension;
	std::string _uploadDir;
	bool _autoIndex;
};

#endif /* CONFIGCLASS_HPP */
