#include "Request.hpp"

Request::Request(const Server &server, int fd, const std::string &request) : server(server), fd(fd), request(request)
{
    parse();

    // Find the Virtual Host - by default the first one
    VirtServIt = findHost();
    server.lg.log(DEBUG,"Request: Domain: " + domain);

    // Find the Location by route - if no route matches - the end of the vector returned
    LocationIt = findRoute();
    if (LocationIt != (*VirtServIt).getLocations().end()){
        server.lg.log(DEBUG,"Request: Route found: " + (*LocationIt).getRoute());
    }
    else {
        server.lg.log(DEBUG,"Request: Route NOT found for url: " + url);
        // set 404
        return;
    }


    if (methodOk()) 
        server.lg.log(DEBUG,"Request: Method: " + toStr(method) + " allowed" );
    else {
        server.lg.log(DEBUG,"Request: Method: " + toStr(method) + " forbidden. set Status 405" );
        // set 405;
        return;
    }

    if (method == MethodGET){
        checkForGET();
        return;
    }
    server.lg.log(DEBUG,"Request: constructor DONE only for GET" );
    return;
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
    server.lg.log(DEBUG,"Request:\n" + ss.str());

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
    server.lg.log(DEBUG,"Request: request header found: " + hostVal.str());
    std::string domain;
    std::getline(hostVal, domain, ':');
    return domain;
}

const vsIt Request::findHost()
{
    // we get list of VServers that listen to the server socket 
    // to which the client fd belongs
    const std::vector<vsIt> vs_vec = server.clientFd2vsIt(fd);
    server.lg.log(DEBUG,"Request: client fd: " + server.lg.str(fd));
    server.lg.log(DEBUG,"Request: server fd: " + server.lg.str(server.getClientRef(fd)));
    server.lg.log(DEBUG,"Request: port: " + server.lg.str(server.getPortRef(server.getClientRef(fd))));

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
            server.lg.log(DEBUG,"Request: checking domain: " + *name_it);
            if (toLower(*name_it) == toLower(headerDomain))
            {
                domain = headerDomain;
                server.lg.log(DEBUG,"Request: domain found: " + headerDomain);
                return server_iter;
            }
        }
    }
    domain = "";
    return *vs_vec.begin(); // default server is the first one
}

// std::string findLongestMatchingRoute(const std::string& routeToFind, const VirtServer& virtServer) {
// 	const std::vector<Location>& locations = virtServer.getLocations();
// 	std::string longestMatch;
//     locIt loc_it = virtServer.getLocations().begin();

// 	for (size_t i = 0; i < locations.size(); ++i) { // return location iterator inside longest matching route
// 		const std::string& route = locations[i].getRoute();
// 		if (routeToFind.find(route) == 0 && route.length() > longestMatch.length()) {
// 			longestMatch = route;
// 		}
// 	}

// 	if (longestMatch.empty()) {
// 		return "Error: No matching route found"; // Return error message or empty string?
// 	}

// 	return longestMatch; // Return the longest matching route
// }


locIt Request::findRoute()
{
    /* unction compares URL (string) with all routes of a server (vector of strings) 
    and finds the longest match (url starts with the route) 
    and returns the iterator of location vector. 
    if the location does not match any route, 
    the end of the location vector is returned, 
    which then can be checked if any route has found */

    locIt loc_it_longestroute = (*VirtServIt).getLocations().end(); // todo check if != end()

	const VirtServer & vs = *VirtServIt;
    std::string longestMatch;
	// Find the location iterator corresponding to the longest matching route
	for (locIt loc_it = vs.getLocations().begin(); loc_it != vs.getLocations().end(); ++loc_it) {
		if (url_match_root(url, (*loc_it).getRoute())) {
			if ((*loc_it).getRoute() > longestMatch) {
                longestMatch = (*loc_it).getRoute();
                loc_it_longestroute = loc_it;
            };
		}
	}
	return loc_it_longestroute;
}

bool Request::methodOk() {

    if (LocationIt != (*VirtServIt).getLocations().end()){
        const std::vector<Method>& methodVec = (*LocationIt).getMethods();
        std::vector<Method>::const_iterator method_iter;
        for (method_iter = methodVec.begin(); method_iter != methodVec.end(); ++method_iter) {
            if (*method_iter == method) {
                return true;
            }
        }
    }
    return false;
}

bool Request::checkForGET() {
    // assuming GET method, functionS that check
    // if the resource can be found  in the location root
    // if it is accessible for the server process.
    // if it is a directory, we check if listing is alowed for this location
    // Expected Errors: file or dir is not found -> 404.

    if (LocationIt == (*VirtServIt).getLocations().end())
        return false;
    const Location& loc = *LocationIt;
    std::string path = loc.getLocationRoot();
    truncateIfEndsWith(path, '/');

    struct stat st;
    path += url;
    server.lg.log(DEBUG,"Request: resourceAvailable: checking path: " + path);

    if (stat(path.c_str(), &st) != 0) {
        server.lg.log(DEBUG, "Request: Error accessing path (does NOT exist?): " + std::string(strerror(errno))); // = file does not exist
        // set 404 
        server.lg.log(DEBUG, "Request: set Status 404"); // = file does not exist
        return false;
    }
    if (!hasReadPermission(path)) {
        // set 500
        server.lg.log(DEBUG, "Request: Cannot read existing file. set Status 500"); // = file does not exist
        return false;
    }

    if (S_ISDIR(st.st_mode)) {
        if ((*LocationIt).getAutoIndex() ){
            // set 200
            server.lg.log(DEBUG, "Request: dir can be indexed. set Status 200"); // = file does not exist
            return true;
        } else {
            // set 403
            server.lg.log(DEBUG, "Request: dir CANNOT be incexed. set Status 403"); // = file does not exist
            return false;
        }
    } 
    else if (S_ISREG(st.st_mode)) {
        // set 200;
        server.lg.log(DEBUG, "Request: reg file. set Status 200"); // = file does not exist
        return true;
    }
    else {
        // set 404;
        server.lg.log(DEBUG, "Request: path is NEITHER reg file or dir. set Status 404"); // = file does not exist
        return false;
    }
}