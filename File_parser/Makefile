SANITIZER	= -fsanitize=address -g3
CXX			= c++ -std=c++98 #$(SANITIZER)
CPPFLGES 	= -Wall -Wextra -Werror

NAME		= parse
FILES		= ./files/main.cpp \
			./files/utiles.cpp \
			./files/tokenizer.cpp \
			./files/collector.cpp \
			./files/errors.cpp \
			./files/validator.cpp

O_FILES 	= $(FILES:.cpp=.o)

all: $(NAME)

$(NAME) : $(O_FILES)
	ar -rc $(NAME) $(O_FILES)

clean:
	rm -rf $(O_FILES)

fclean: clean
	rm -rf $(NAME)

re: fclean $(NAME)