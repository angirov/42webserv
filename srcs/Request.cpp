#include "Request.hpp"

Request::Request(const Server &server, int fd, const std::string &request) : server(server), fd(fd), request(request)
{
    parse();
    print_request();

    // Find the Virtual Host - by default the first one
    VirtServIt = findHost();
    server.lg.log(DEBUG, "Request: Domain: " + domain);

    // Find the Location by route - if no route matches - the end of the vector returned
    LocationIt = findRoute();
    if (LocationIt != (*VirtServIt).getLocations().end())
    {
        server.lg.log(DEBUG, "Request: Route found: " + (*LocationIt).getRoute());
    }
    else
    {
        server.lg.log(DEBUG, "Request: Route NOT found for url: " + url);
        statusCode = StatusCode404;
        return;
    }

    if (methodOk())
        server.lg.log(DEBUG, "Request: Method: " + toStr(method) + " allowed");
    else
    {
        server.lg.log(DEBUG, "Request: Method: " + toStr(method) + " forbidden. set Status 405");
        statusCode = StatusCode405;
        return;
    }
	// TODO: CHECK IF REDIR is set for route, if NOT set response code to 3XX (_redirCode string from Configclass)
	// if strings !empty statusCode 3XX + return
    if (method == MethodGET)
    {
        checkForGET();
        return;
    }
    else if (method == MethodDELETE)
    {
        checkForDELETE();
        return;
    }
    else if (method == MethodPOST)
    {
        std::string uploadDir = (*LocationIt).getUploadDir();
        server.lg.log(DEBUG, "Request: checking uploaddir: " + uploadDir);
        if (isDirHasWritePermission(uploadDir)) {
            if (isCgiExtention(extractExtension(extractFileName(getPath())))) {
                statusCode = StatusCodeCGI;
                server.lg.log(DEBUG, "Request: reg file. set Status CGI");
                return;
            }
            server.lg.log(DEBUG, "Request: set Status POST");
            statusCode = StatusCodePOST;
            return;
        }
        else
        {
            server.lg.log(DEBUG, "Request: POST upload dir is bad - set 500");
            statusCode = StatusCodePost500;
            return;
        }
    }
    server.lg.log(DEBUG, "Request: constructor DONE only for GET, POST");
    return;
}

std::string Request::process_hard()
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

std::string readFileToString(const std::string &filename)
{
    std::ifstream file(filename.c_str());
    std::ostringstream oss;

    if (file)
    {
        oss << file.rdbuf();
        return oss.str();
    }
    else
    {
        std::cerr << "Error opening file: " << filename << std::endl;
        return ""; // Return an empty string if file cannot be opened
    }
}

std::string Request::getMimeType(const std::string &extension)
{
    if (extension == "html" || extension == "htm")
    {
        return "text/html";
    }
    else if (extension == "css")
    {
        return "text/css";
    }
    else if (extension == "txt")
    {
        return "text/txt";
    }
    else
    {
        // Default MIME type for unknown extensions
        return "application/octet-stream";
    }
}

std::string extractFileName(const std::string &fullPath);
std::string extractExtension(const std::string &fileName);

std::string Request::process_get200()
{
    std::string path = getPath();
    std::string res_body = readFileToString(path);
    std::string type = getMimeType(extractExtension(extractFileName(path)));

    std::string full_res;
    full_res += "HTTP/1.1 200 OK\r\n";
    full_res += "Content-Type: " + type + "\r\n";
    full_res += "Content-Length : " + server.lg.str((int)res_body.length()) + "\r\n";
    full_res += "\r\n";
    full_res += res_body;

    server.lg.log(DEBUG, "Request: Responce:\n " + full_res);

    return full_res;
}

std::string Request::process_get200dir()
{
    std::string res_body;

    res_body += "<html>\n<head><title>Index of " + url + "</title></head>\n"
                "<body>\n<h1>Index of " + url + "</h1><hr><pre><a href=\"../\">../</a>\n";

    DIR *dir; // DIR = A type representing a directory stream.
    struct dirent *ent;
    if ((dir = opendir(getPath().c_str())) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            std::string name(ent->d_name);
            if (ent->d_type == DT_REG) {
                res_body += "<a href=\"" + name + "\">" + name + "</a>\n";
            } else if (ent->d_type == DT_DIR) {
                if (strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0) {
                    res_body += "<a href=\"" + name + "/\">" + name + "/</a>\n";
                }
            }
        }
        closedir(dir);
    }

    res_body += "</pre><hr></body>\n</html>\n";

    std::string full_res;
    full_res += "HTTP/1.1 200 OK\r\n";
    full_res += "Content-Type: text/html\r\n";
    full_res += "Content-Length : " + server.lg.str((int)res_body.length()) + "\r\n";
    full_res += "\r\n";
    full_res += res_body;

    return full_res;
}

std::string Request::process_get301dir()
{
    std::string res_body; // = "<!DOCTYPE html>"
    res_body += "<html>\n<head><title>301 Moved Permanently</title></head>\n<body>";
    res_body += "<center><h1>301 Moved Permanently</h1></center>";
    res_body += "<hr><center>The Best WebServ42</center>\n</body>\n</html>";
    std::string full_res;
    full_res += "HTTP/1.1 301 Moved Permanently\r\n";
    full_res += "Content-Type: text/html\r\n";
    full_res += "Content-Length : " + server.lg.str((int)res_body.length()) + "\r\n";
    full_res += "Location: " + url + "/\r\n";
    full_res += "\r\n";
    full_res += res_body;

    return full_res;
}

std::string Request::process_get301()
{
    return "under construction process_get301";
}

std::string Request::process_get403()
{
    return "under construction process_get403";
}

std::string Request::process_get404()
{
    return "HTTP/1.1 404 OK\r\nContent-Length: 0\r\n\r\n";
}

std::string Request::process_get405()
{
    return "under construction process_get405";
}

std::string Request::process_get500()
{
    return "under construction process_get500";
}

std::string Request::process_post500()
{
    return "under construction process_post500";
}

std::string Request::process_cgi500()
{
    return "HTTP/1.1 500 Failed (CGI)\r\nContent-Length: 0\r\n\r\n";
}

std::string Request::process_POST()
{
    std::string upload_path = appendIfNotEndsWith((*LocationIt).getUploadDir(), '/');
    server.lg.log(DEBUG, "Request: upload_path: " + upload_path);
    std::string file_name = getDifference((*LocationIt).getRoute(), url);
    if (file_name == "" || file_name == "/")
    {
        server.lg.log(DEBUG, "Request: no name submitted. Generateing name...");
        file_name = "upload_" + generateTimeStamp();
    }
    server.lg.log(DEBUG, "Request: file_name: " + file_name);
    std::string filepath = upload_path + file_name;
    writeStringToBinaryFile(body, filepath);
    return "HTTP/1.1 201 OK\r\n\r\n";
    // todo: the rest of the post responce
}

std::string Request::process_DELETE()
{
    std::string filename = getPath();

    // Attempt to delete the file
    if (std::remove(filename.c_str()) != 0)
    {
        server.lg.log(ERROR, "Error deleting file");
        return "HTTP/1.1 500 Internal Server Error\r\n\r\n";
    }
    else
    {
        server.lg.log(INFO, "File successfully deleted");
    }

    return "HTTP/1.1 204 No Content\r\n\r\n";
    // todo: the rest of the post responce
}

std::string Request::process()
{
    std::string res;
    if (statusCode == StatusCode200) res = process_get200();
    if (statusCode == StatusCode200dir) res = process_get200dir();
    if (statusCode == StatusCode301) res = process_get301();
    if (statusCode == StatusCode403) res = process_get403();
    if (statusCode == StatusCode404) res = process_get404();
    if (statusCode == StatusCode405) res = process_get405();
    if (statusCode == StatusCode500) res = process_get500();
    if (statusCode == StatusCodePOST) res = process_POST();
    if (statusCode == StatusCodePost500) res = process_post500();
    if (statusCode == StatusCodeCGI) res = process_CGI();
    if (statusCode == StatusCodeDELETE) res = process_DELETE();
    return res;
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
    url = word;
    size_t queryStart = url.find('?');
    if (queryStart != std::string::npos) {
        queryString = url.substr(queryStart + 1);
        url = url.substr(0, queryStart);
    }

    iss >> word;
    httpVersion = resolveHTTPVersion(word);
}

void Request::parse_header(const std::string &line)
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
    ss << "============== body: ==============\n"
       << body << std::endl;
    server.lg.log(DEBUG, "Prining parsed request:\n" + ss.str());
    server.lg.log(DEBUG, "\n============== DONE prining rarsed request ==============\n");
    // printServer();
}

void Request::printServer() const
{
    server.displayServer();
}

const std::vector<std::string> &Request::getHeaderVals(std::string const &key) const
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
    if (hostVals == notFoundStrVec) {
        server.lg.log(ERROR, "Could not find host header.");
        return "";
    }
    std::stringstream hostVal(*hostVals.begin());
    server.lg.log(DEBUG, "Request: request header found: " + hostVal.str());
    std::string domain;
    std::getline(hostVal, domain, ':');
    return domain;
}

const vsIt Request::findHost()
{
    // we get list of VServers that listen to the server socket
    // to which the client fd belongs
    const std::vector<vsIt> vs_vec = server.clientFd2vsIt(fd);
    server.lg.log(DEBUG, "Request: client fd: " + server.lg.str(fd));
    server.lg.log(DEBUG, "Request: server fd: " + server.lg.str(server.getClientRef(fd)));
    server.lg.log(DEBUG, "Request: port: " + server.lg.str(server.getPortRef(server.getClientRef(fd))));

    std::string headerDomain = getRequestHostHeader();

    if (headerDomain.length() > 0)
    {
    // loop over the VServers and again over their names
        std::vector<vsIt>::const_iterator vs_it;
        for (vs_it = vs_vec.begin(); vs_it != vs_vec.end(); ++vs_it)
        {
            const vsIt server_iter = *vs_it;
            const std::vector<std::string> names = (*server_iter).getServerNames();
            std::vector<std::string>::const_iterator name_it;
            for (name_it = names.begin(); name_it != names.end(); ++name_it)
            {
                server.lg.log(DEBUG, "Request: checking domain: " + *name_it);
                if (toLower(*name_it) == toLower(headerDomain))
                {
                    domain = headerDomain;
                    server.lg.log(DEBUG, "Request: domain found: " + headerDomain);
                    return server_iter;
                }
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

locIt Request::findRoute() const
{
    /* unction compares URL (string) with all routes of a server (vector of strings)
    and finds the longest match (url starts with the route)
    and returns the iterator of location vector.
    if the location does not match any route,
    the end of the location vector is returned,
    which then can be checked if any route has found */

    locIt loc_it_longestroute = (*VirtServIt).getLocations().end(); // todo check if != end()

    const VirtServer &vs = *VirtServIt;
    std::string longestMatch;
    // Find the location iterator corresponding to the longest matching route
    for (locIt loc_it = vs.getLocations().begin(); loc_it != vs.getLocations().end(); ++loc_it)
    {
        if (url_match_root(url, (*loc_it).getRoute()))
        {
            if ((*loc_it).getRoute() > longestMatch)
            {
                longestMatch = (*loc_it).getRoute();
                loc_it_longestroute = loc_it;
            };
        }
    }
    return loc_it_longestroute;
}

bool Request::methodOk() const
{

    if (LocationIt != (*VirtServIt).getLocations().end())
    {
        const std::vector<Method> &methodVec = (*LocationIt).getMethods();
        std::vector<Method>::const_iterator method_iter;
        for (method_iter = methodVec.begin(); method_iter != methodVec.end(); ++method_iter)
        {
            if (*method_iter == method)
            {
                return true;
            }
        }
    }
    return false;
}

std::string Request::getPath()
{

    if (LocationIt == (*VirtServIt).getLocations().end())
        return "";
    const Location &loc = *LocationIt;
    std::string path = loc.getLocationRoot();
    truncateIfEndsWith(path, '/');

    path += url;
    return path;
}

bool Request::checkForGET()
{
    // assuming GET method, functionS that check
    // if the resource can be found  in the location root
    // if it is accessible for the server process.
    // if it is a directory, we check if listing is alowed for this location
    // Expected Errors: file or dir is not found -> 404.

    std::string path = getPath();
    if (path.length() == 0)
        return false;
    server.lg.log(DEBUG, "Request: resourceAvailable: checking path: " + path);

    struct stat st = {};
    if (stat(path.c_str(), &st) != 0)
    {
        server.lg.log(DEBUG, "Request: Error accessing path (does NOT exist?): " + std::string(strerror(errno))); // = file does not exist
        statusCode = StatusCode404;
        server.lg.log(DEBUG, "Request: set Status 404");
        return false;
    }

    if (!hasReadPermission(path))
    {
        statusCode = StatusCode500;
        server.lg.log(DEBUG, "Request: Cannot read existing file. set Status 500");
        return false;
    }

    if (S_ISDIR(st.st_mode))
    {
        if ((*LocationIt).getAutoIndex())
        {
            if (url[url.length() - 1] == '/')
            { // if last char of url == /
                statusCode = StatusCode200dir;
                server.lg.log(DEBUG, "Request: dir can be indexed. set Status 200");
                return true;
            }
            else
            {
                statusCode = StatusCode301dir;
                server.lg.log(DEBUG, "Request: dir can be indexed but needs a trailing / set Status 301dir");
                return true;
            }
        }
        else
        {
            statusCode = StatusCode403;
            server.lg.log(DEBUG, "Request: dir CANNOT be incexed. set Status 403");
            return false;
        }
    }
    else if (S_ISREG(st.st_mode))
    {
        if (isCgiExtention(extractExtension(extractFileName(path)))) {
            statusCode = StatusCodeCGI;
            server.lg.log(DEBUG, "Request: reg file. set Status CGI");
        }
        else {
            statusCode = StatusCode200;
            server.lg.log(DEBUG, "Request: reg file. set Status 200");
        }
        return true;
    }
    else
    {
        statusCode = StatusCode404;
        server.lg.log(DEBUG, "Request: path is NEITHER reg file or dir. set Status 404");
        return false;
    }
}

bool Request::checkForDELETE()
{
    std::string path = getPath();
    if (path.length() == 0)
        return false;
    server.lg.log(DEBUG, "Request: DELETE: resourceAvailable: checking path: " + path);

    struct stat st = {};
    if (stat(path.c_str(), &st) != 0)
    {
        server.lg.log(DEBUG, "Request: DELETE: Error accessing path (does NOT exist?): " + std::string(strerror(errno))); // = file does not exist
        statusCode = StatusCode404;
        server.lg.log(DEBUG, "Request: set Status 404");
        return false;
    }

    if (S_ISDIR(st.st_mode))
    {
        statusCode = StatusCode500;
        server.lg.log(DEBUG, "Request: DELETE: is directory. set Status 500");
        return false;
    }

    if (!hasWritePermission(path))
    {
        statusCode = StatusCode500;
        server.lg.log(DEBUG, "Request: DELETE: Cannot write existing file. set Status 500");
        return false;
    }

    if (S_ISREG(st.st_mode))
    {
        server.lg.log(DEBUG, "Request: set Status DELETE");
        statusCode = StatusCodeDELETE;
        return true;
    }
    else
    {
        statusCode = StatusCode404;
        server.lg.log(DEBUG, "Request: path is NEITHER reg file or dir. set Status 404");
        return false;
    }
}
