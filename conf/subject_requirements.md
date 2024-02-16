## Configuration file:

• Choose the port and host of each "server"

• Set up the server_names or not.

• The first server for a host:port will be the default for this host:port (that means
it will answer to all the requests that don’t belong to another server).

• Setup default error pages:

    • I guess we should orientate ourselves on the html default eror pages.

• Limit client body size.

• Setup routes with one or multiple of the following rules/configuration (routes won't
be using regexp):

    • Define a list of accepted HTTP methods for the route.
    • Define a HTTP redirection.
    • Define a directory or a file from where the file should be searched
    • Turn on or off directory listing.
    • Set a default file to answer if the request is a directory.
    • Execute CGI based on certain file extension (for example .php).
    • Make it work with POST and GET methods.
    • Make the route able to accept uploaded files and configure where they should be saved.
      Because you won’t call the CGI directly, use the full path as PATH_INFO:

    ∗ Just remember that, for chunked request, your server needs to unchunkit, the CGI will expect EOF as end of the body.
    ∗ Same things for the output of the CGI. If no content_length is returned from the CGI, EOF will mark the end of the returned data.
    ∗ Your program should call the CGI with the file requested as first argument.
    ∗ The CGI should be run in the correct directory for relative path file access.
    ∗ Your server should work with one CGI (php-CGI, Python, and so forth)

You must provide some configuration files and default basic files to test and demonstrate every feature works during evaluation.
