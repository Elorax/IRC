#include "Server.hpp"

//Param: NICK <nickname>

void	Server::cmdNick( vecString& args, int fd ) {

	if (getClientByFD(fd)->getPassword().empty() || getClientByFD(fd)->getUsername().empty())
		return (buildMsg(ERR_NOTREGISTERED, fd));

	//std::cout << "cmdNick, args[1] = >" << args[1] << "<" << std::endl;
	if (args[1].empty())
		buildMsg(ERR_NONICKNAMEGIVEN, fd);

	else if (args.size() != 2)
		buildMsg(ERR_NEEDMOREPARAMS(args[0]), fd);
	
	else if (!isAvailNick(args[1]))
		buildMsg(ERR_NICKNAMEINUSE(args[1]), fd);

	else if (!isValidNick(args[1]))
		{
			std::cout << "le nick est pas bon kevin" << std::endl;
			buildMsg(ERR_ERRONEUSNICKNAME(args[1]), fd);
		}

	else
		getClientByFD(fd)->setNickname(args[1]);
}