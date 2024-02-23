#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <iostream>
#include <sstream>
#include <iterator>
#include <cctype> // for strip
#include "conf/ConfigClass.hpp"

enum Method
{
    MethodInvalid,
    MethodGET,
    MethodPOST,
    MethodDELETE
};

enum HTTPVersion
{
    HTTPVerInvalid,
    HTTPVer10,
    HTTPVer11
};

enum StatusCode
{
    StatusCodeInvalid,
    StatusCode200,
    StatusCode404
};

typedef std::vector<VirtServer>::const_iterator vsIt;
typedef std::vector<Location>::const_iterator locIt;

static const vsIt notFoundVirtServer;
static const std::vector<vsIt> notFoundVirtServerVec;
static const std::vector<std::string> notFoundStrVec;

Method resolveMethod(std::string const & word);
HTTPVersion resolveHTTPVersion(std::string const & word);
StatusCode resolveStatusCode(std::string const & word);

std::string toStr(Method method);
std::string toStr(HTTPVersion httpVersion);
std::string toStr(StatusCode statusCode);

std::string strip(std::string input);
std::string toLower(const std::string& input);
std::string toUpper(const std::string& input);

#endif