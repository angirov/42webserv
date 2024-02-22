#include "utils.hpp"

Method resolveMethod(std::string const & word) {
    if(word == "GET" ) return MethodGET;
    if(word == "POST" ) return MethodPOST;
    if(word == "DELETE" ) return MethodDELETE;
    return MethodInvalid;
}

std::string toStr(Method method) {
    if(method == MethodGET ) return "GET";
    if(method == MethodPOST ) return "POST";
    if(method == MethodDELETE ) return "DELETE";
    return "";
}

HTTPVersion resolveHTTPVersion(std::string const & word) {
    if(word == "HTTP/1.0" ) return HTTPVer10;
    if(word == "HTTP/1.1" ) return HTTPVer11;
    return HTTPVerInvalid;
}

std::string toStr(HTTPVersion httpVersion) {
    if(httpVersion == HTTPVer10 ) return "HTTP/1.0";
    if(httpVersion == HTTPVer11 ) return "HTTP/1.1";
    return "";
}

StatusCode resolveStatusCode(std::string const & word) {
    if(word == "200" ) return StatusCode200;
    if(word == "404" ) return StatusCode404;
    return StatusCodeInvalid;
}

std::string toStr(StatusCode statusCode) {
    if(statusCode == StatusCode200 ) return "200";
    if(statusCode == StatusCode404 ) return "404";
    return "";
}

std::string strip(std::string input) {
    std::string::iterator first = input.begin();
    std::string::iterator last = input.end();

    for (std::string::iterator it = input.begin(); std::isspace(*it); ++it) {
        first++;
    }
    for (std::string::reverse_iterator it = input.rbegin(); std::isspace(*it); ++it) {
        last--;
    }
    return (first < last) ? std::string(first, last) : std::string();
}

std::string toLower(const std::string& input) {
    std::string result = input;
    for (std::size_t i = 0; i < result.length(); ++i) {
        result[i] = std::tolower(result[i]);
    }
    return result;
}

std::string toUpper(const std::string& input) {
    std::string result = input;
    for (std::size_t i = 0; i < result.length(); ++i) {
        result[i] = std::toupper(result[i]);
    }
    return result;
}