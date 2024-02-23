#include "Request.hpp"

Request::Request(const Server &server, int fd, const std::string &request) : server(server), fd(fd), request(request)
{
    parse();

    VirtServIt = findHost();
    std::cout << "===================" << std::endl;
    std::cout << "Domain: " << domain << std::endl;

    LocationIt = findRoute();
    std::cout << "Route: " << route << std::endl;
    std::cout << "===================" << std::endl;

    if (methodOk()) std::cout << "$$$$ Method: " << toStr(method) << " allowed" << std::endl;
    else std::cout << "$$$$ Method: " << toStr(method) << " forbidden" << std::endl;

    if (resourceAvailable()) std::cout << "$$$$ Resource: " << resourcePath << " available" << std::endl;
    else std::cout << "$$$$ Resource: " << resourcePath << " NOT available" << std::endl;
}

std::string Request::process()
{
    // parse();
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

void Request::print_headers(std::stringstream &ss)
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

std::string Request::getRequestHostHeader() const
{
    // the first and HOPEFULLY the only header of the request
    const std::vector<std::string> hostVals = getHeaderVals("host");
    std::stringstream hostVal(*hostVals.begin());
    std::cout << "#### request header found: " << hostVal.str() << std::endl;
    std::string domain;
    std::getline(hostVal, domain, ':');
    return domain;
}

const vsIt Request::findHost()
{ // Does not make sense at all
    // we get list of VServers that listen to the server socket to which the client fd belongs
    std::cout << "#### client fd: " << fd << std::endl;
    std::cout << "#### server fd: " << server.getClientRef(fd) << std::endl;
    std::cout << "#### port: " << server.getPortRef(server.getClientRef(fd)) << std::endl;
    const std::vector<vsIt> vs_vec = server.clientFd2vsIt(fd);

    std::string headerDomain = getRequestHostHeader();

    // loop over the VServers and again over their names
    std::vector<vsIt>::const_iterator vs_it;
    for (vs_it = vs_vec.begin(); vs_it != vs_vec.end(); ++vs_it)
    {
        const vsIt server_iter = *vs_it;
        const std::vector<std::string> names = (*server_iter).getServerNames();
        std::vector<std::string>::const_iterator name_it;
        for (name_it = names.begin(); name_it != names.end(); ++name_it)
        {
            std::cout << "#### checking domain: " << *name_it << std::endl;
            if (toLower(*name_it) == toLower(headerDomain))
            {
                domain = headerDomain;
                std::cout << "######## domain found: " << headerDomain << std::endl;
                return server_iter;
            }
        }
    }
    domain = "";
    return *vs_vec.begin(); // default server is the first one
}

const locIt Request::findRoute()
{
    // url
    const VirtServer & vs = *VirtServIt;
    const std::vector<Location> locs = vs.getLocations();
    // magic ...
    
    const locIt loc_it = locs.begin();
    route = (*loc_it).getRoute();
    // resourcePath =  extracts the name of the resource and sets it for this request
    return loc_it; // default???
}

bool Request::methodOk() {

    // magic ...

    if (method == MethodGET)
        return true;
    else
        return false;
}

bool Request::resourceAvailable() {
    // assuming GET method, functionS that check 
    // if the resource can be found  in the location root 
    // if it is accessible for the server process. 
    // if it is a regular file, it should be read, 
    // if it is a directory, we check if listing is alowed for this location 
    // and if yes it should be listed. 
    // Expected Errors: file or dir is not found -> 404. 
    // what should we do if the dir is not allowed to be listed?

    const Location& loc = *LocationIt;
    // magic ... find resourcePath in the filesystem
    //     ...

    if (loc.getAutoIndex())
        return true;
    else
        return false;
}
