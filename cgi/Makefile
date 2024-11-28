NAME 		= cgi

CXX 		= c++
SANITIZER	= -fsanitize=address -g3

CPPFLAGS 	= -Wall -Wextra -Werror -std=c++98

SRCS 		= 	./cgi.cpp


HDRS 		=	./cgi.hpp

OBJCS 		= $(SRCS:.cpp=.o)

all : $(NAME) 

$(NAME) : $(OBJCS) $(HDRS)
	ar -rc $(NAME) $(OBJCS) 

clean :
	rm -fr $(OBJCS)

fclean : clean
	rm -fr $(NAME)
