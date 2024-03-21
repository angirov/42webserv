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
        header_map::iterator it = headers.find("content-type");
        if (it != headers.end())
        {
            server.lg.log(DEBUG, "Request: CGI: request's contentType FOUND");
            setCgiEnvVar("CONTENT_TYPE",  *headers["content-type"].begin());
        }
        setCgiEnvVar("CONTENT_LENGTH", server.lg.str(body.size()));
    }

    setCgiEnvVar("SCRIPT_NAME",        extractFileName(resourcePath));
    setCgiEnvVar("REQUEST_URI",        url);
    setCgiEnvVar("SCRIPT_FILENAME",    resourcePath);
    setCgiEnvVar("PATH_INFO",          resourcePath);
    setCgiEnvVar("PATH_TRANSLATED",    resourcePath);
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

char ** Request::makeCgiArgv(std::string file_name)
{
    std::string cgi_exec_path = CGI_EXEC;
    cgi_exec_path = CGI_DIR + cgi_exec_path;
    file_name = CGI_DIR + file_name;
    char ** cgi_argv = (char **)calloc(3, sizeof(char *));
    cgi_argv[0] = strdup(cgi_exec_path.c_str());
    cgi_argv[1] = strdup(file_name.c_str());
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

std::string Request::process_CGI()
{
    // std::string file_name = "../../data/pycgi/hellopy/hello.py"; 
    std::string file_name = getDifference((*LocationIt).getRoute(), url);
    server.lg.log(DEBUG, "Request: CGI file_name: " + file_name);

    if (file_name.find("/") != std::string::npos) {
        server.lg.log(DEBUG, "Request: Only scripts immediately in the cgi-bin dir are allowed.");
        return process_get403();
    }

    server.lg.log(DEBUG, "Request: start processing CGI...");
    server.lg.log(DEBUG, "Request: CGI body: " + body);
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

        char ** cgi_argv = makeCgiArgv(file_name);
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
            std::cerr << "EXECVE failed: " << err << "\n";
            close(STDOUT_FILENO);
            close(STDIN_FILENO);
            exit(0);
        }
    }
    close(fdOut[WRITE_FD]);
    close(fdIn);


    // wait(NULL);
    int status, rc;
    int times = 1;
    int max_times = 7; // = 0.5 sec
    int toWait = 0;
    int waited = 0;
    while (times < max_times) {
        server.lg.log(DEBUG, "Request: CGI waitingc for the child ,,, " + server.lg.str(times) + " ...");
        toWait = pow(10, times);
        usleep(toWait); /* sleep 0.1 seconds */
        waited += toWait;
        rc = waitpid(-1, &status, WNOHANG);
        if (rc == 0) {
            times++;
            continue;
        }
        else if (rc < 0) {
            perror("waitpid");
            return "";
        } else {
            // Child process terminated
            server.lg.log(DEBUG, "Request: CGI waiting rc: " + server.lg.str(rc));
            std::stringstream ss;
            ss << WEXITSTATUS(status);
            if (WIFEXITED(status)) {
                server.lg.log(DEBUG, "Request: CGI: Child process exited with status: " + ss.str());
            } else {
                server.lg.log(DEBUG, "Request: CGI: Child process exited abnormally.");
            }

            break;
        }
    }

    server.lg.log(DEBUG, "Request: CGI: times: " + server.lg.str(times));
    server.lg.log(DEBUG, "Request: CGI: Waited " + server.lg.str(waited / 1000) + " ms.");

    if (times == max_times) {
        server.lg.log(DEBUG, "Request: CGI: Child process timed out.");
        kill(id, SIGTERM);
        wait(NULL);
        return process_cgi500();
    }

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
            server.lg.log(DEBUG, "Request: CGI read_ret " + server.lg.str(read_ret));
            break;
        }
    }
    std::string full_res;
    if (cgi_res.length() > 0) {
        full_res += "HTTP/1.1 200 OK (CGI)\r\n";
        full_res += "Content-Length: " + server.lg.str(getHTTPBodySize(cgi_res)) + "\r\n";
        full_res +=  cgi_res;
    }
    else {
        return process_cgi500();
    }

    return  full_res;
}

bool Request::isCgiExtention(std::string ext)
{
    server.lg.log(DEBUG, "Reqeust: checking extention: " + ext);
    if (LocationIt == (*VirtServIt).getLocations().end())
        return "";
    const Location &loc = *LocationIt;
    const std::vector<std::string> exts = loc.getCGIExtensions();
    std::vector<std::string>::const_iterator it;
    for (it = exts.begin(); it != exts.end(); ++it) {
        if (*it == ext) {
            server.lg.log(DEBUG, "Reqeust: found extention: " + ext);
            return true;
        }
    }
    server.lg.log(DEBUG, "Reqeust: NOT found extention: " + ext);
    return false;
}
