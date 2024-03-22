#include <vector>
#include <string>

struct Redir {

};

struct LocationConfig {
    std::vector<std::string> methods;
    Redir redir;
    std::string root_dir;
    bool dir_listing_allowed;
    std::string default_file;
    std::string cgi_extention;
    
};

struct ServerConfig {
    int port;
    std::string ipaddr;
    std::vector<std::string> server_names;
    std::vector<LocationConfig> locations;
};

struct Configs {
    std::vector<ServerConfig> servers;
    int default_server = 0;
    int client_body_size_limit = 10000;
};