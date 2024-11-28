SANITIZER	= -fsanitize=address -g3
CXX			= c++ -std=c++98
CPPFLAGS	= -Wall -Wextra -Werror

NAME		= webserv

FILES = Methodes/srcs/boundaryUtils.cpp\
  		Methodes/srcs/delete.cpp\
		Methodes/srcs/get.cpp\
    	Methodes/srcs/parceChuncked.cpp\
	  	Methodes/srcs/postBoundary.cpp\
	    Methodes/srcs/Post.cpp\
		Methodes/srcs/utilsPost.cpp\
		cgi/cgi.cpp\
		File_parser/files/collector.cpp\
  		File_parser/files/errors.cpp\
		File_parser/files/main.cpp\
		File_parser/files/response.cpp\
		File_parser/files/tokenizer.cpp\
		File_parser/files/utiles.cpp\
		File_parser/files/validator.cpp\
		Multiplexing/srcs/Client.cpp\
		Multiplexing/srcs/Multi.cpp\
		Multiplexing/srcs/Request.cpp\
		Multiplexing/srcs/Server.cpp\
		main.cpp


HEADERS = Methodes/includes/delete.hpp\
		Methodes/includes/get.hpp\
	    Methodes/includes/Post.hpp\
		cgi/cgi.hpp\
		File_parser/includes/collector.hpp\
  		File_parser/includes/errors.hpp\
		File_parser/includes/response.hpp\
		File_parser/includes/tokenizer.hpp\
		File_parser/includes/utiles.hpp\
		File_parser/includes/validator.hpp\
		Multiplexing/includes/Multi.hpp

OBJ_FILES 	= $(FILES:.cpp=.o)

all : $(NAME)

$(NAME) : $(OBJ_FILES) $(HEADERS)
	$(CXX) $(CPPFLAGS) $(OBJ_FILES) -o $(NAME)

clean:
	rm -rf $(OBJ_FILES)

fclean: clean
	rm -rf $(NAME)

re: fclean all