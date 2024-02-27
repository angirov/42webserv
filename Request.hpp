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
#include "conf/ConfigClass.hpp"
#include "utils.hpp"

typedef std::map<std::string, std::vector<std::string> > header_map;

struct Server;

struct Request
{
    // private:
public:
    // Have to be initialized in the constructor?
    const Server &server; // server->virtServers[0].locations[0]
    int fd;
    vsIt VirtServIt;
    locIt LocationIt;

    // Connection
    bool connectionKeepAlive;

    std::istringstream request;

    Method method;
    std::string url; // should be a class??? query params?
    std::string resourcePath;
    HTTPVersion httpVersion;
    header_map headers; // conditions GET (changed since last request ), obligatory host header for virtual hosting, Connection: Keep-Alive default for http11 otherwise "close"
    std::string body;

    std::string domain;
    std::string route;

    void parse_first_line();
    void parse_header(const std::string& line);
    void parse();

    const std::vector<std::string> &getHeaderVals(std::string const &key) const;
    const vsIt findHost();
    locIt findRoute() const;
    bool methodOk() const;
    bool checkForGET();
    std::string getRequestHostHeader() const;
    void print_headers(std::stringstream &ss);
    void print_request();

    // public:
    Request(const Server &server, int fd, const std::string &request);
    void printServer() const;

    std::string process();
};

class Response
{
    HTTPVersion httpVersion;
    StatusCode statusCode; // with descriptions hardcoded
    header_map headers;
};

#endif

// http11:
// consistent connections
// mandatory server headers
// better caching options
//