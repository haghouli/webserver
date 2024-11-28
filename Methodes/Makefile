NAME = Req

CXX = c++
SANITIZER	= -fsanitize=address -g3

CPPFLAGS = -Wall -Wextra -Werror -std=c++98

SRCS = 	srcs/Post.cpp\
		srcs/delete.cpp\
		srcs/get.cpp\
		srcs/utilsPost.cpp\
		srcs/parceChuncked.cpp\
		srcs/postBoundary.cpp\
		srcs/boundaryUtils.cpp


HDRS =	includes/Post.hpp\
		includes/delete.hpp\
		includes/get.hpp

OBJCS = $(SRCS:.cpp=.o)

all : $(NAME) 

$(NAME) : $(OBJCS) $(HDRS)
	ar -rc $(NAME) $(OBJCS) 

clean :
	rm -fr $(OBJCS)

fclean : clean
	rm -fr $(NAME)

re : fclean re
