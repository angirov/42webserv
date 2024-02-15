COMPILER = g++
CFLAGS = -Wall -Werror -Wextra

all: client hello_server

client: multiconnect/client.cpp
	$(COMPILER) $(CFLAGS) multiconnect/client.cpp -o client

hello_server: multiconnect/server_http_hello_world.cpp
	$(COMPILER) $(CFLAGS) multiconnect/server_http_hello_world.cpp -o hello_server

clean:
	rm -f client hello_server

re: clean all