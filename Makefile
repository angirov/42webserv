COMPILER = g++
CFLAGS = -Wall -Werror -Wextra -std=c++98

all: client hello_server

client: client.cpp
	$(COMPILER) $(CFLAGS) client.cpp -o client

hello_server: server_sellect.cpp
	$(COMPILER) $(CFLAGS) server_sellect.cpp -o hello_server

clean:
	rm -f client hello_server

re: clean all