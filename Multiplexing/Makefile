SANITIZER	= -fsanitize=address -g3
CXX			= c++ -std=c++98
CPPFLAGS 	= -Wall -Wextra -Werror

NAME		= Multi

FILES		= ./srcs/Client.cpp \
			  ./srcs/Multi.cpp \
			  ./srcs/Server.cpp \
			  ./srcs/Request.cpp \
			

O_FILES 	= $(FILES:.cpp=.o)

all: $(NAME)
	

$(NAME) : $(O_FILES)
	ar -rc $(NAME) $(O_FILES)

makeparse:
	make -C ../File_parser

clean:
	rm -rf $(O_FILES)

fclean: clean
	rm -rf $(NAME)

re: fclean $(NAME)