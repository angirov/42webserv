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

#include "utils.hpp"

typedef std::map<std::string, std::vector<std::string> > header_map;

struct Request
{
// private:
public:
    // Server
    static std::string default_host;
    // Connetion
    bool connectionKeepAlive;

    std::istringstream  request;

    Method       method;
    std::string  url; // should be a class??? query params?
    HTTPVersion  httpVersion;
    header_map   headers; // conditions GET (changed since last request ), obligatory host header for virtual hosting, Connection: Keep-Alive default for http11 otherwise "close"
    std::string  body;

    void parse_first() {
        std::string first_line;
        std::getline(request, first_line);
        std::istringstream iss(first_line);
        std::string word;
        iss >> word;
        method = resolveMethod(word);

        iss >> word;
        url = word; ////////////////////////////// need a url handler?????????????????????????????????????????????????
        iss >> word;
        httpVersion = resolveHTTPVersion(word);
    }
    

    void parse_header(std::string line) {
        std::stringstream lineStream(line);
        std::string key;
        std::getline(lineStream, key, ':');
        // check key?

        std::string value;
        std::list<std::string> values_l;
        while (std::getline(lineStream, value, ',')) { // it is a mess: https://stackoverflow.com/questions/29549299
            values_l.push_back(strip(value));
        }
        // while (lineStream >> value) {
        //     values_l.push_back(strip(value));
        // }
        std::vector<std::string> values(values_l.begin(), values_l.end());

        headers.insert(header_map::value_type(strip(key), values)); // the values should not be overwritten -> check if key is there
    }

    void parse()
    {
        parse_first();
        std::string header_line;
        while (std::getline(request, header_line) && header_line != "\r") { // is there a beter way to deal with it ????????????????????????????
                parse_header(header_line);
        }
        std::stringstream rest;
        rest << request.rdbuf();
        body = rest.str();
    }

    void print_headers(std::stringstream & ss) {
        for (header_map::iterator it = headers.begin(); it != headers.end(); ++it) {
            ss << "Header: " << (*it).first << ";  values: [ " << std::endl;
            (*it).second; // str vec
            for (std::vector<std::string>::iterator val_it = (*it).second.begin(); val_it != (*it).second.end(); ++val_it) {
                ss << "    " << *val_it << std::endl;
            }
            ss << "  ]" << std::endl;
        }
    }

    void print_request() {
        std::stringstream ss;
        ss << "method: " << method << std::endl;
        ss << "url: " << url << std::endl;
        ss << "httpVersion: " << httpVersion << std::endl;
        print_headers(ss);
        ss << "body: " << body << std::endl;
        std::cout << ss.str() << std::endl;

    }


// public:
    Request(const std::string &request) : request(request)
    {
    }

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