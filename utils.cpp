#include "utils.hpp"

Method resolveMethod(std::string const & word) {
    if(word == "GET" ) return MethodGET;
    if(word == "POST" ) return MethodPOST;
    if(word == "DELETE" ) return MethodDELETE;
    return MethodInvalid;
}

HTTPVersion resolveHTTPVersion(std::string const & word) {
    if(word == "HTTP/1.0" ) return HTTPVer10;
    if(word == "HTTP/1.1" ) return HTTPVer11;
    return HTTPVerInvalid;
}

StatusCode resolveStatusCode(std::string const & word) {
    if(word == "200" ) return StatusCode200;
    if(word == "404" ) return StatusCode404;
    return StatusCodeInvalid;
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