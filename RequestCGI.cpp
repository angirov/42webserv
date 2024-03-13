#include "Request.hpp"

char ** Request::makeCgiEnv()
{
        std::vector<std::string> cgi_env;
        cgi_env.push_back("TEST0=val0");
        cgi_env.push_back("TEST1=val1");
        cgi_env.push_back("TEST2=val2");

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

std::string Request::process_CGI()
{
    server.lg.log(DEBUG, "Request: start processing CGI...");
    int fd[2];
    pipe(fd);

    int id = fork();
    if (id == 0)
    {
        char ** cgi_argv = makeCgiArgv();
        char ** env_arr = makeCgiEnv();

        close(fd[READ_FD]);
        std::cerr << "Child is starting\n";
        dup2(fd[WRITE_FD], STDOUT_FILENO);
        close(fd[WRITE_FD]);

        int err = execve(cgi_argv[1], cgi_argv, env_arr);

        if (err == -1)
        {
            freeCharPtrArr(cgi_argv);
            freeCharPtrArr(env_arr);
            write(STDERR_FILENO, "EXECVE failed\n", 14);
            close(STDOUT_FILENO);
            exit(0);
        }
    }
    close(fd[WRITE_FD]);
    wait(NULL);
    char buff[CGI_BUFF_SIZE];
    std::string str;
    int read_ret;
    while (1)
    {
        memset(buff, 0, CGI_BUFF_SIZE);
        read_ret = read(fd[READ_FD], buff, CGI_BUFF_SIZE);
        if (read_ret > 0)
        {
            str += std::string(buff, read_ret);
            server.lg.log(DEBUG, "Request: reading CGI return (" + server.lg.str(read_ret) + "): " + str);
        }
        else
        {
            server.lg.log(DEBUG, "Request: read_ret " + server.lg.str(read_ret));
            break;
        }
    }
    std::cout << ">>> " << str << std::endl;
    return "HTTP/1.1 200 OK (CGI)\r\n" + str;
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
