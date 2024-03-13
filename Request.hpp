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
#include <sys/wait.h>

#define READ_FD     0
#define WRITE_FD    1
#define CGI_BUFF_SIZE 10000
#define PY_EXEC "/usr/bin/python3"

#include "Server.hpp"
#include "conf/ConfigClass.hpp"
#include "utils.hpp"
#define UPLOAD_PATH "/home/wo/proj/42/42webserv/data/uploads/"
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
    StatusCode statusCode;

    std::vector<std::string> cgi_env;

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
    std::string getPath();
    bool isCgiExtention(std::string ext);
    std::string getMimeType(const std::string& extension);

    // public:
    Request(const Server &server, int fd, const std::string &request);
    void printServer() const;

    std::string process();
    std::string process_hard();
    std::string getStatusLine();
    std::string process_get200();
    std::string process_get200dir();
    std::string process_get301();
    std::string process_get301dir();
    std::string process_get403();
    std::string process_get404();
    std::string process_get405();
    std::string process_get500();
    std::string process_POST();
    std::string process_CGI();
    char ** makeCgiArgv();
    char ** makeCgiEnv();
    void setCgiEnvVar(std::string varName, std::string varVal);
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