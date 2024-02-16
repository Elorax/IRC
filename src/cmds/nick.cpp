#include "Server.hpp"

//Param: NICK <nickname>

void	Server::cmdNick( std::vector<std::string>& args, int fd ) {

	if (args.size() != 1)
		buildMsg(ERR_NEEDMOREPARAMS, fd);

	else if (args[0].empty())
		buildMsg(ERR_NONICKNAMEGIVEN, fd);

	else if (!isAvailNick(args[0]))
		buildMsg(ERR_NICKNAMEINUSE, fd);

	else if (!isValidNick(args[0]))
		buildMsg(ERR_ERRONEUSNICKNAME, fd);

	else
		getClientByFD(fd)->setNickName(args[0]);
}