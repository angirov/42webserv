#include "Request.hpp"

Request::Request(const Server & server, int fd, const std::string &request) : server(server), fd(fd), request(request)
{
    parse();
    VirtServIt = findHost();
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

void Request::parse_first_line()
{
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

void Request::parse_header(std::string line)
{
    std::stringstream lineStream(line);
    std::string key;
    std::getline(lineStream, key, ':');
    // check key?

    std::string value;
    std::list<std::string> values_l;
    while (std::getline(lineStream, value, ','))
    { // it is a mess: https://stackoverflow.com/questions/29549299
        values_l.push_back(strip(value));
    }
    // while (lineStream >> value) {
    //     values_l.push_back(strip(value));
    // }
    std::vector<std::string> values(values_l.begin(), values_l.end());

    headers.insert(header_map::value_type(strip(toLower(key)), values)); // the values should not be overwritten -> check if key is there
}

void Request::parse()
{
    parse_first_line();
    std::string header_line;
    while (std::getline(request, header_line) && header_line != "\r")
    { // is there a beter way to deal with it ????????????????????????????
        parse_header(header_line);
    }
    std::stringstream rest;
    rest << request.rdbuf();
    body = rest.str();
}

void Request::print_headers(std::stringstream & ss)
{
    for (header_map::iterator it = headers.begin(); it != headers.end(); ++it)
{
        ss << "Header: " << (*it).first << ";  values: [ " << std::endl;
        (*it).second; // str vec
        for (std::vector<std::string>::iterator val_it = (*it).second.begin(); val_it != (*it).second.end(); ++val_it)
        {
            ss << "    " << *val_it << std::endl;
        }
        ss << "  ]" << std::endl;
    }
}

void Request::print_request()
{
    std::stringstream ss;
    ss << "fd: " << fd << std::endl;
    ss << "resourcePath: " << resourcePath << std::endl;
    ss << "method: " << method << std::endl;
    ss << "url: " << url << std::endl;
    ss << "httpVersion: " << httpVersion << std::endl;
    print_headers(ss);
    ss << "body: " << body << std::endl;
    std::cout << ss.str() << std::endl;
    std::cout  << "Host name: " << (*VirtServIt).getServerName() << std::endl;
    // printServer();
}

void Request::printServer() const
{
    server.displayServer();
}

const std::vector<std::string> &Request::getHeaderVals(std::string const key) const
{

    header_map::const_iterator it = headers.find(toLower(key));

    if (it != headers.end())
    {
        return (*it).second;
    }
    else
    {
        // Return a special value indicating object not found
        return notFoundStrVec;
    }
}

vsIt Request::findHost() {
    const std::vector<VirtServer> & vs = server.getVirtServers();

    const std::vector<std::string> hostVals = getHeaderVals("host");
    if (hostVals == notFoundStrVec) {
        return vs.begin();
    }
    std::string hostVal = *hostVals.begin(); // the first and HOPEFULLY the only header

    if (hostVal.length() > 0) {
        for (vsIt it = vs.begin(); it != vs.end(); ++it) {
            std::cout << "getServerName " << (*it).getServerName() << std::endl;
            if (toLower((*it).getServerName()) == toLower(hostVal)) {
                return it;
            }
        }
    }
    return vs.begin();
}
