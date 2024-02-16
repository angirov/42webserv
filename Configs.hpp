#include <vector>
#include <string>

class Redir {

};

class LocationConfig {
    std::vector<std::string> methods;
    Redir redir;
    std::string root_dir;
    bool dir_listing_allowed;
    std::string default_file;
    std::string cgi_extention;
    
};

class ServerConfig {
    int port;
    std::string ipaddr;
    std::vector<std::string> server_names;
    std::vector<LocationConfig> locations;
};

class Configs {
    std::vector<ServerConfig> servers;
    int default_server = 0;
    int client_body_size_limit = 10000;
};