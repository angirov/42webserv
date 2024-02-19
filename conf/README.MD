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

check switches for syntax check