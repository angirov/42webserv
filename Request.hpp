#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <string>
#include <iostream>
#include <sstream>
#include <map>
#include <vector>
#include <list>
#include <iterator>
#include <cctype> // for strip

#include "Server.hpp"
#include "utils.hpp"

typedef std::map<std::string, std::vector<std::string> > header_map;

class Server;

struct Request
{
    // private:
public:
    // Have to be initialized in the constructor?
    const Server &server; // server->virtServers[0].locations[0]
    int fd;
    vsIt VirtServIt;
    locIt LocationIt;

    // Connetion
    bool connectionKeepAlive;

    std::istringstream request;

    Method method;
    std::string url; // should be a class??? query params?
    std::string resourcePath;
    HTTPVersion httpVersion;
    header_map headers; // conditions GET (changed since last request ), obligatory host header for virtual hosting, Connection: Keep-Alive default for http11 otherwise "close"
    std::string body;

    void parse_first_line();
    void parse_header(std::string line);
    void parse();

    const std::vector<std::string> &getHeaderVals(std::string const key) const;
    vsIt findHost();

    void print_headers(std::stringstream &ss);
    void print_request();

    // public:
    Request(const Server &server, int fd, const std::string &request);
    void printServer() const;

    std::string process();
};

class Responce
{
    HTTPVersion httpVersion;
    StatusCode statusCode; // with descriptions hardcoded
    header_map headers;
};

#endif

// http11:
// consistent connetions
// mandatory server headers
// bettter caching options
//