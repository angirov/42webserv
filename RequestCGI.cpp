#include "Request.hpp"

void Request::setCgiEnvVar(std::string varName, std::string varVal) {
    cgi_env.push_back(varName + "=" + varVal);
}

std::string headerVals(std::vector<std::string> headerValVec) {
    std::string res;
    for (std::vector<std::string>::iterator it = headerValVec.begin(); it != headerValVec.end(); ++it)
    {
        if (it != headerValVec.begin()) 
            res += ",";
        res += *it;
    }
    return res;
}

char ** Request::makeCgiEnv()
{
/////////////////////////////////////////////////////////////////////////////////////
    setCgiEnvVar("GATEWAY_INTERFACE",  "CGI/1.1");
    setCgiEnvVar("SERVER_SOFTWARE",    "42webserv");

    setCgiEnvVar("QUERY_STRING",       queryString);
    setCgiEnvVar("REQUEST_METHOD",     toStr(method));

    if(method == MethodPOST)
    {
        setCgiEnvVar("CONTENT_TYPE",   *headers["content-type"].begin());
        setCgiEnvVar("CONTENT_LENGTH", server.lg.str((int)body.size()));
    }

    setCgiEnvVar("SCRIPT_NAME",        extractFileName(getPath()));
    setCgiEnvVar("REQUEST_URI",        url);
    setCgiEnvVar("SCRIPT_FILENAME",    getPath());
    setCgiEnvVar("PATH_INFO",          getPath());
    setCgiEnvVar("PATH_TRANSLATED",    getPath());
    setCgiEnvVar("SERVER_NAME",        domain);
    setCgiEnvVar("SERVER_PORT",        server.lg.str(server.getPortRef(server.getClientRef(fd))));
    setCgiEnvVar("SERVER_PROTOCOL",    "HTTP/1.1");
    setCgiEnvVar("REDIRECT_STATUS",    "200");

    for (header_map::iterator it = headers.begin(); it != headers.end(); ++it)
    {
        std::string name = it->first;
        std::transform(name.begin(), name.end(), name.begin(), ::toupper);
        std::string key = "HTTP_" + name;
        setCgiEnvVar(key, headerVals(it->second));
    }

    char ** env_arr = (char **)calloc(cgi_env.size() + 1, sizeof(char *));
    std::size_t i = 0;
    while (i < cgi_env.size()) {
        env_arr[i] = strdup(cgi_env[i].c_str());
        i++;
    }
    env_arr[i] = NULL;
    return env_arr;
}

char ** Request::makeCgiArgv()
{
    char ** cgi_argv = (char **)calloc(3, sizeof(char *));
    cgi_argv[0] = strdup(PY_EXEC);
    cgi_argv[1] = strdup(getPath().c_str());
    cgi_argv[2] = NULL;

    return cgi_argv;
}

void freeCharPtrArr(char ** arr) {
    if (arr) {
        for(int i=0; arr[i]; i++) {
            free(arr[i]);
        }
        free(arr);
    }
}
size_t getHTTPBodySize(const std::string& httpResponse) {
    // Find the position of the double CRLF ("\r\n\r\n") that separates the header and body
    size_t bodyStart = httpResponse.find("\r\n\r\n");

    // If double CRLF is not found, return 0 (no body)
    if (bodyStart == std::string::npos)
        return 0;

    // Calculate the size of the body by subtracting the body start position
    // from the total length of the HTTP response
    return httpResponse.size() - (bodyStart + 4); // 4 is the length of "\r\n\r\n"
}

std::string Request::process_CGI()
{
    server.lg.log(DEBUG, "Request: start processing CGI...");
    server.lg.log(DEBUG, "CGI: body: " + body);
    int fdOut[2];
    pipe(fdOut);

    FILE	*filedIn = tmpfile();
    int	fdIn = fileno(filedIn);
    write(fdIn, body.c_str(), body.size());
    lseek(fdIn, 0, SEEK_SET);

    int id = fork();
    if (id == 0)
    {
        std::cerr << "Child is starting\n";
        close(fdOut[READ_FD]);

        char ** cgi_argv = makeCgiArgv();
        char ** env_arr = makeCgiEnv();

        dup2(fdOut[WRITE_FD], STDOUT_FILENO);
        dup2(fdIn, STDIN_FILENO);

        close(fdOut[WRITE_FD]);
        close(fdIn);

        int err = execve(cgi_argv[0], cgi_argv, env_arr);

        if (err == -1)
        {
            freeCharPtrArr(cgi_argv);
            freeCharPtrArr(env_arr);
            write(STDERR_FILENO, "EXECVE failed\n", 14);
            close(STDOUT_FILENO);
            close(STDIN_FILENO);
            exit(0);
        }
    }
    close(fdOut[WRITE_FD]);
    close(fdIn);
    wait(NULL);
    char buff[CGI_BUFF_SIZE];
    std::string cgi_res;
    int read_ret;
    while (1)
    {
        memset(buff, 0, CGI_BUFF_SIZE);
        read_ret = read(fdOut[READ_FD], buff, CGI_BUFF_SIZE);
        if (read_ret > 0)
        {
            cgi_res += std::string(buff, read_ret);
            server.lg.log(DEBUG, "Request: reading CGI return (" + server.lg.str(read_ret) + "): " + cgi_res);
        }
        else
        {
            server.lg.log(DEBUG, "Request: read_ret " + server.lg.str(read_ret));
            break;
        }
    }
    std::string full_res;
    if (cgi_res.length() > 0) {
    full_res += "HTTP/1.1 200 OK (CGI)\r\n";
    full_res += "Content-Length: " + server.lg.str((int)getHTTPBodySize(cgi_res)) + "\r\n";
    full_res +=  cgi_res;
    }
    else {
        return process_cgi500();
    }

    return  full_res;
}

bool Request::isCgiExtention(std::string ext)
{
    if (LocationIt == (*VirtServIt).getLocations().end())
        return "";
    const Location &loc = *LocationIt;
    const std::vector<std::string> exts = loc.getCGIExtensions();
    std::vector<std::string>::const_iterator it;
    for (it = exts.begin(); it != exts.end(); ++it) {
        if (*it == ext) {
            return true;
        }
    }
    return false;
}
