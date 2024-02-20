#ifndef CONFIGCLASS_HPP
#define CONFIGCLASS_HPP

#include <string>
#include <vector>
#include <map>

class Config {
public:
	Config();

	// Setter functions
	void setTimeout(int timeout);
	void setMaxClients(int maxClients);
	void setPort(int port);
	void setServerName(const std::string& serverName);
	void setErrorPage(const std::string& errorPage);
	void setLocationRoot(const std::string& locationRoot);
	void setLocationIndex(const std::string& locationIndex);
	void setMethods(const std::vector<std::string>& methods);
	void setCGI(const std::string& cgi);
	void setUploadDir(const std::string& uploadDir);

	// Getter functions
	int getTimeout() const;
	int getMaxClients() const;
	int getPort() const;
	std::string getServerName() const;
	std::string getErrorPage() const;
	std::string getLocationRoot() const;
	std::string getLocationIndex() const;
	std::vector<std::string> getMethods() const;
	std::string getCGI() const;
	std::string getUploadDir() const;

private:
	int _timeout;
	int _maxClients;
	std::vector<VirtServer> _virtServers;
	int client_max_body_size;
};

class VirtServer {
	int _port;
	std::string _serverName;
	std::map<int, std::string> _errorPages;
	std::vector<Location> locations;
};

class Location {
	std::map<int, std::string> _returnRedir; 
	std::string _route;
	std::string _locationRoot;
	std::string _locationIndex;
	std::vector<std::string> _methods;
	std::vector<std::string> _cgiExtension;
	std::string _uploadDir;
	bool _autoIndex;
}

#endif /* CONFIGCLASS_HPP */
