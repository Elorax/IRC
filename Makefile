NAME		=	ircserv

SRCS		=	src/Channel.cpp	\
				src/Client.cpp	\
				src/Message.cpp	\
				src/Server.cpp	\
				main.cpp		\
				

INCS		=	inc/Channel.hpp	\
				inc/Client.hpp	\
				inc/IRC.hpp		\
				inc/Message.hpp	\
				inc/Server.hpp	\


CC			=	c++

CFLAGS		=	-Wall -Werror -Wextra -std=c++98 -g3

RM			=	rm -f

OBJ			=	$(SRCS:.cpp=.o)

all			:	$(NAME)

%.o : %.cpp
				$(CC) $(CFLAGS) -o $@ -c $< 

$(NAME)		:	$(OBJ)
				$(CC) $(CFLAGS) $(OBJ) -o $(NAME) 
		
clean		:	
				$(RM) $(OBJ)

fclean		:	clean
				$(RM) $(NAME)

re			:	fclean all

.PHONY		:	clean fclean re all