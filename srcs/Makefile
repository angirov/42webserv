# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: mwagner <mwagner@student.42wolfsburg.de>   +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/03/20 14:32:07 by mwagner           #+#    #+#              #
#    Updated: 2024/03/20 14:32:10 by mwagner          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #


COMPILER = c++
CFLAGS = -Wall -Werror -Wextra -std=c++98
SRCS := \
    main.cpp \
    parser/Parser.cpp \
    Server.cpp \
    Request.cpp \
    RequestCGI.cpp \
    Logger.cpp \
    utils.cpp \
    conf/ConfigClass.cpp
NAME := server

# Color codes
GREEN := \033[0;32m
RED := \033[0;31m
NC := \033[0m

.PHONY: all clean fclean re

all: $(NAME)

$(NAME): $(SRCS:.cpp=.o)
	@echo -e "$(GREEN)Creating $(NAME)$(NC)"
	@$(COMPILER) $(CFLAGS) $^ -o $@

%.o: %.cpp
	@echo -e "$(GREEN)Compiling $<$(NC)"
	@$(COMPILER) $(CFLAGS) -c $< -o $@

clean:
	@echo -e "$(RED)Deleting object files$(NC)"
	@rm -f $(SRCS:.cpp=.o)

fclean: clean
	@echo -e "$(RED)Deleting $(NAME)$(NC)"
	@rm -f $(NAME)

re: fclean all
