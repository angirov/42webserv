#ifndef CONFIGCLASS_HPP
#define CONFIGCLASS_HPP

#include <string>
#include <vector>
#include <map>

class Location;
class VirtServer;

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

private:
	int _timeout;
	int _maxClients;
	int _client_max_body_size;
};

class VirtServer {
public:
	VirtServer(int port, const std::string &serverName);
	VirtServer(const VirtServer &other); // Copy constructor

	// Setter and Getter functions for VirtServer class
	void setPort(int port);
	int getPort() const;

	void setServerName(const std::string &serverName);
	const std::string & getServerName() const;

	void addErrorPage(int errorCode, const std::string &errorPage);
	const std::string & getErrorPage(int errorCode) const;

	void addLocation(const Location &location);
	const std::vector<Location> & getLocations() const;

private:
	int _port;
	std::string _serverName;
	std::map<int, std::string> _errorPages;
	std::vector<Location> locations;
};

class Location {
public:
	Location(const std::string &route, const std::string &locationRoot, const std::string &locationIndex);
	Location(const Location &other); // Copy constructor

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
	const std::vector<std::string> & getMethods() const;

	void addCGIExtension(const std::string &cgiExtension);
	const std::vector<std::string> & getCGIExtensions() const;

	void setUploadDir(const std::string &uploadDir);
	const std::string & getUploadDir() const;

	void addReturnRedir(int errorCode, const std::string &redirectUrl);
	const std::string & getReturnRedir(int errorCode) const;

private:
	std::map<int, std::string> _returnRedir;
	std::string _route;
	std::string _locationRoot;
	std::string _locationIndex;
	std::vector<std::string> _methods;
	std::vector<std::string> _cgiExtension;
	std::string _uploadDir;
	bool _autoIndex;
};

#endif /* CONFIGCLASS_HPP */
