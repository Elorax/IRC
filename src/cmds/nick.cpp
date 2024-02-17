#include "Server.hpp"

//Param: NICK <nickname>

void	Server::cmdNick( vecString& args, int fd ) {

	if (args[0].empty())
		buildMsg(ERR_NONICKNAMEGIVEN, fd);

	else if (args.size() != 1)
		buildMsg(ERR_NEEDMOREPARAMS, fd);
		
	else if (!isAvailNick(args[0]))
		buildMsg(ERR_NICKNAMEINUSE, fd);

	else if (!isValidNick(args[0]))
		buildMsg(ERR_ERRONEUSNICKNAME, fd);

	else
		getClientByFD(fd)->setNickname(args[0]);
}