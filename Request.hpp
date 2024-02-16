#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <string>
# include <iostream>
# include <sstream>

class Request {
public:
    std::string request;
    Request(const std::string & request) : request(request){

    }

    std::string process();
};

#endif