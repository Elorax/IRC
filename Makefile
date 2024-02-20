NAME		=	ircserv

SRCS		=	src/Channel.cpp		\
				src/Client.cpp		\
				src/Message.cpp		\
				src/Server.cpp		\
				src/ServerRun.cpp	\
				src/cmds/invite.cpp	\
				src/cmds/join.cpp	\
				src/cmds/kick.cpp	\
				src/cmds/mode.cpp	\
				src/cmds/nick.cpp	\
				src/cmds/notice.cpp	\
				src/cmds/part.cpp	\
				src/cmds/pass.cpp	\
				src/cmds/privmsg.cpp\
				src/cmds/quit.cpp	\
				src/cmds/topic.cpp	\
				src/cmds/user.cpp	\
				src/cmds/who.cpp	\
				main.cpp			\

INCS		=	-I./inc


CC			=	c++

CFLAGS		=	-Wall -Wextra -std=c++98 -I./inc -g3

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