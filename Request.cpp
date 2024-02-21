#include "Request.hpp"

Request::Request(const Server & server, const std::string &request) : server(server), request(request)
{
}

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

void Request::parse_first() {
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


void Request::parse_header(std::string line) {
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

void Request::parse()
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

void Request::print_headers(std::stringstream & ss) {
    for (header_map::iterator it = headers.begin(); it != headers.end(); ++it) {
        ss << "Header: " << (*it).first << ";  values: [ " << std::endl;
        (*it).second; // str vec
        for (std::vector<std::string>::iterator val_it = (*it).second.begin(); val_it != (*it).second.end(); ++val_it) {
            ss << "    " << *val_it << std::endl;
        }
        ss << "  ]" << std::endl;
    }
}

void Request::print_request() {
    std::stringstream ss;
    ss << "method: " << method << std::endl;
    ss << "url: " << url << std::endl;
    ss << "httpVersion: " << httpVersion << std::endl;
    print_headers(ss);
    ss << "body: " << body << std::endl;
    std::cout << ss.str() << std::endl;
    printServer();
}

void Request::printServer() const {
    server.displayServer();
}