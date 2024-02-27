#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <iostream>
#include <sstream>
#include <iterator>
#include <cctype> // for strip
#include <sys/stat.h>
#include <cstring> // for strerror
#include <unistd.h> // for access()
#include <cerrno>
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
    // StatusCode201,
    StatusCode301,
    // StatusCode400, // not understand
    StatusCode403,
    StatusCode404,
    StatusCode405,
    StatusCode500,
};

enum ContentType
{
    ContentInvalid,
    ContentPlain,
    ContentHTML,
    ContentCSS
};

Method resolveMethod(std::string const & word);
HTTPVersion resolveHTTPVersion(std::string const & word);
StatusCode resolveStatusCode(std::string const & word);

std::string toStr(Method method);
std::string toStr(HTTPVersion httpVersion);
std::string toStr(StatusCode statusCode);

std::string strip(std::string input);
std::string toLower(const std::string& input);
std::string toUpper(const std::string& input);

void truncateIfEndsWith(std::string& str, char c);
bool url_match_root(std::string str, std::string pattern);
bool isValidDirectory(const std::string& path);
bool hasReadPermission(const std::string& path);

#endif