COMPILER = g++
CFLAGS = -Wall -Werror -Wextra -std=c++98
TESTFLAGS = -Wall -Werror -Wextra
SRCS	:=	main.cpp Server.cpp Request.cpp Logger.cpp utils.cpp conf/ConfigClass.cpp
SRCS_TEST	:=	test_main.cpp Server.cpp Request.cpp Logger.cpp utils.cpp conf/ConfigClass.cpp

all: client server

client: client.cpp
	$(COMPILER) $(CFLAGS) client.cpp -o client

server:
	$(COMPILER) $(CFLAGS) ${SRCS} -o server

servertest:
	$(COMPILER) $(TESTFLAGS) ${SRCS_TEST} -o servertest

clean:
	rm -f server

fclean: clean
	rm -f client
	rm -f servertest

test: re
	bash test.sh

re: clean all