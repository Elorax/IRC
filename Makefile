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
				src/cmds/pong.cpp	\
				src/main.cpp

INCSDIR		=	-I./inc

CC			=	c++

CFLAGS		=	-Wall -Wextra -std=c++98 -I./inc -g3

RM			=	rm -rf

OBJSDIR		=	.objs

SRCSDIR		=	srcs

OBJS		=	$(addprefix $(OBJSDIR)/,$(notdir $(SRCS:.cpp=.o)))

DEPS		=	$(addprefix $(OBJSDIR)/,$(notdir $(SRCS:.cpp=.d)))

$(OBJSDIR)	:
				@mkdir -p $(OBJSDIR)

$(OBJSDIR)/%.o : %.cpp
				@$(CC) $(CFLAGS) -c $< -o $@ -MMD $(INCSDIR)

$(OBJSDIR)/%.o : src/%.cpp
				@printf "\033[1;33mGenerating ft_irc object... %-38.38s \r" $@
				@$(CC) $(CFLAGS) -c $< -o $@ -MMD $(INCSDIR)

$(OBJSDIR)/%.o : src/cmds/%.cpp
				@printf "\033[1;33mGenerating ft_irc object... %-38.38s \r" $@
				@$(CC) $(CFLAGS) -c $< -o $@ -MMD $(INCSDIR)

all			:	$(NAME)

$(NAME)		:	$(OBJSDIR) $(OBJS)
				@$(CC) $(CFLAGS) $(OBJS) -o $(NAME)
				@echo "\033[1;32mft_irc: Done!\033[0m"


clean		:
				@$(RM) $(OBJS) $(DEPS)
				@$(RM) $(OBJSDIR)
				@echo "\033[1;31mObject cleaned!\033[0m"

fclean		:	clean
				@$(RM) $(NAME)
				@echo "\033[1;31mProgram and object cleaned!\033[0m"

re			:	fclean all

.PHONY		:	clean fclean re all

-include $(DEPS)
