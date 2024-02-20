#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <iostream>
#include <sstream>
#include <iterator>
#include <cctype> // for strip

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

Method resolveMethod(std::string const & word);
HTTPVersion resolveHTTPVersion(std::string const & word);
StatusCode resolveStatusCode(std::string const & word);

std::string strip(std::string input);

#endif