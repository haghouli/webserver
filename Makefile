
SANITIZER	= -fsanitize=address -g3
CXX			= c++ -std=c++98 #$(SANITIZER)
CPPFLGES 	= -Wall -Wextra -Werror

NAME		= webserver


all: makeparse makeMulti
	$(CXX) main.cpp ./File_parser/parse ./Multiplexing/Multi -o $(NAME)

makeparse:
	make -C ./File_parser

makeMulti:
	make -C ./Multiplexing

fclean:
	make fclean -C ./File_parser 
	make fclean -C ./Multiplexing
	rm -rf $(NAME)

re: fclean all
