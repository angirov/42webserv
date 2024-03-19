COMPILER = c++
CFLAGS = -Wall -Werror -Wextra -std=c++98
SRCS	:=	main.cpp parser/Parser.cpp Server.cpp Request.cpp RequestCGI.cpp Logger.cpp utils.cpp conf/ConfigClass.cpp
SRCS_TEST	:=	test_main.cpp Server.cpp Request.cpp RequestCGI.cpp Logger.cpp utils.cpp conf/ConfigClass.cpp

all: client server

server:
	$(COMPILER) $(CFLAGS) ${SRCS} -o server

fclean:
	rm -f server

re: fclean all