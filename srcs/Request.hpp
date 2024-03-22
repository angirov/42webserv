/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mwagner <mwagner@student.42wolfsburg.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/22 12:27:23 by mwagner           #+#    #+#             */
/*   Updated: 2024/03/22 12:27:48 by mwagner          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
#include <cmath>
#include <csignal>

#define READ_FD     0
#define WRITE_FD    1
#define CGI_BUFF_SIZE 10000
#define CGI_DIR "/webserv/cgi-bin/"
#define CGI_EXEC "python3"

#include "Server.hpp"
#include "conf/ConfigClass.hpp"
#include "utils.hpp"
#define UPLOAD_PATH "/webserv/data/uploads/"
typedef std::map<std::string, std::vector<std::string> > header_map;

struct Server;

struct Request
{
    // private:
public:
    bool sizeExceeded;
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
    std::string queryString;
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
    bool checkForPOST();
    bool checkForGET();
    bool checkForDELETE();
    std::string getRequestHostHeader() const;
    void print_headers(std::stringstream &ss);
    void print_request();
    std::string getPath();
    bool isCgiExtention(std::string ext);
    std::string getMimeType(const std::string& extension);
	std::string generate_error_page(const std::string& error_page_path);

    // public:
    Request(const Server &server, int fd, const std::string &request, bool sizeExceeded);
    void printServer() const;

	bool checkForRedirection();

    std::string process();
    bool process_dir();
    std::string process_hard();
    std::string getStatusLine();
    std::string process_get200();
    std::string process_get200dir();
    std::string process_get301();
    std::string process_get301dir();
	std::string process_redirection() const;
    std::string process_get403();
    std::string process_get404();
    std::string process_get405();
    std::string process_get413();
    std::string process_get500();
    std::string process_POST();
    std::string process_DELETE();
    std::string process_post500();
    std::string process_CGI();
    std::string process_cgi500();
    char ** makeCgiArgv(std::string file_name);
    char ** makeCgiEnv();
    void setCgiEnvVar(std::string varName, std::string varVal);
};

#endif
