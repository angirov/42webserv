COMPILER = g++
CFLAGS = -Wall -Werror -Wextra -std=c++98
SRCS	:=	main.cpp Server.cpp Request.cpp Logger.cpp utils.cpp conf/ConfigClass.cpp
HDRS	:=	Server.hpp Request.hpp Logger.hpp Request.hpp utils.hpp conf/ConfigClass.hpp
all: client server

client: client.cpp
	$(COMPILER) $(CFLAGS) client.cpp -o client

server: ${HDRS}
	$(COMPILER) $(CFLAGS) ${SRCS} -o server

clean:
	rm -f server

fclean: clean
	rm -f client

test: re
	bash test.sh

re: clean all