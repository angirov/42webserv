#include "Request.hpp"
#include "utils.hpp"

int main() {
    Request req("GET /index.html HTTP/1.1\r\n"
                "Host: webserv.42\r\n"
                "Connection: Keep-Alive\r\n"
                "RandomHeader: val1, val2, val3\r\n"
                "RandomHeader2: \"val with multiple words\", \"value, with, commas\"\r\n"
                "BODY BODY");

    req.parse();
    req.print_request();
    std::cout  << "$" << strip("\r  \t    hello   \n\r\n\r") << "$" << std::endl;
    std::cout  << "$" << strip("\r  \t       \n\r\n\r") << "$" << std::endl;
    std::cout  << "$" << strip("hello") << "$" << std::endl;
}