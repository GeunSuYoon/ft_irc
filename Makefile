# Names
NAME	= ircserv

# Source Files and Object Files, Header
# SRC		= ./src/main.cpp					\
# 		  ./src/engine/ft_irc_channel.cpp	\
# 		  ./src/engine/ft_irc_channel.cpp
SRC		= $(shell find . -name "*.cpp")

OBJ		= $(SRC:.cpp=.o)
HEADER	= -I./includes -I./src

# Compiler and Flags
CC		= c++
CFLAG	= -g -Wall -Wextra -Werror -std=c++98

# Rules
all 	: $(NAME)

$(NAME) : $(OBJ)
	$(CC) $(CFLAG) $(HEADER) -o $@ $^

%.o		: %.cpp
	@$(CC) $(CFLAG) $(HEADER) -c $< -o $@

fclean 	: clean
	@rm -f $(NAME)

clean 	:
	@rm -f $(OBJ)

re 		: fclean all

.PHONY : all fclean clean re