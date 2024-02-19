#include "Request.hpp"

std::string Request::process()
{
    parse();
    print_request();
    std::string http200chunked = "HTTP/1.1 200 OK\r\n"
                                 "Content-Type: text/plain\r\n"
                                 "Transfer-Encoding: chunked\r\n"
                                 "\r\n";
    std::string res;
    res += "RESPONCE: your request was:\n";
    res += "============================\n";
    res += request.str();
    res += "============================\n";
    res += "Thank you for using our server!";

    std::stringstream ss;
    ss << http200chunked
       << std::hex << res.length() << "\r\n"
       << res << "\r\n0\r\n\r\n";

    return ss.str();
}