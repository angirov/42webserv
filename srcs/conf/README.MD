# NGINX CONFIG

## LOADING AN NGINX server config
- a default config and default symbolic link are located in "/etc/nginx" (DEB/UBUNTU) in
  - /sites-available
  - /sites-enabled

When you create a symbolic link from a configuration file in sites-available to sites-enabled, you effectively enable that configuration.
NGINX will then read and use that configuration file when serving requests.


- "sudo nginx -t -c"
  - to check if conf file is ok
- make sure you have root permission
- Add an entry in your /etc/hosts file to map your.customdomain to localhost:

https://docs.nginx.com/nginx/admin-guide/web-server/web-server/

https://nginx.org/en/docs/switches.html

check switches for syntax check.

No detailed syntax check documentation.
but generally:

# Nginx Configuration Syntax Checks

When checking the syntax of an Nginx configuration file using `nginx -t`, the command primarily validates various aspects of the configuration to ensure it is correct and free of errors. While there isn't an exhaustive list of checks performed, understanding key areas of configuration can provide insights into what the syntax check validates.

Below are some key areas of Nginx configuration and where to find more information about them:

1. **Core Module Directives**: These directives define the core behavior of the Nginx server. Documentation for core functionality directives can be found in the [Core Functionality](https://nginx.org/en/docs/http/ngx_http_core_module.html) section of the Nginx documentation.

2. **HTTP Module Directives**: HTTP module directives configure HTTP server behavior, including server blocks, location blocks, and various HTTP-related settings. Documentation for HTTP core module directives is available in the [HTTP Core Module](https://nginx.org/en/docs/http/ngx_http_core_module.html) section.

3. **Server Blocks and Virtual Hosts**: Server blocks (virtual hosts) define how Nginx responds to requests for different domains or IP addresses. Information on configuring server blocks can be found in the [Server](https://nginx.org/en/docs/http/ngx_http_core_module.html#server) section of the documentation.

4. **Location Blocks**: Location blocks configure how Nginx handles requests for specific URI paths. Details on configuring location blocks are available in the [Location](https://nginx.org/en/docs/http/ngx_http_core_module.html#location) section.

5. **SSL Configuration**: If SSL/TLS is used, ensuring correct SSL configuration is crucial. Information on SSL/TLS configuration can be found in the [SSL Module](https://nginx.org/en/docs/http/ngx_http_ssl_module.html) section of the documentation.

By understanding these areas of configuration and consulting the Nginx documentation, users can gain insights into what the syntax check validates when running `nginx -t`. Additionally, error messages produced by the syntax check provide helpful feedback if there are issues with the configuration.



WILL DO testing on syntax check and create a bulletin list for own syntax checks to implement for our config parsing.