#include "Server.hpp"

//Param: NICK <nickname>

void	Server::cmdNick( vecString& args, int fd ) {

	Client& client = *getClientByFD(fd);

	if (client.getPassword().empty())
		return (buildMsg(ERR_NOTREGISTERED, fd));

	//std::cout << "cmdNick, args[1] = >" << args[1] << "<" << std::endl;
	if (args[1].empty())
		buildMsg(ERR_NONICKNAMEGIVEN, fd);

	else if (args.size() < 2)
		buildMsg(ERR_NEEDMOREPARAMS(args[0]), fd);
	
	else if (!isAvailNick(args[1]))
		buildMsg(ERR_NICKNAMEINUSE(client.getNickname(), args[1]), fd);

	else if (!isValidNick(args[1]))
		{
			std::cout << "DEBUG: le nick est pas bon kevin" << std::endl;
			buildMsg(ERR_ERRONEUSNICKNAME(args[1]), fd);
		}

	else
	{
		std::string nickMsg;
		//! CETTE reply EST RECONNUE PAR IRSSI, S'EN INSPIRER POur LA SUITE
		//! AVEC OU SANS ~ LA reply EST CORRECTE ET COMPRISE
		buildMsg(NICKNOTICE(client.getNickname(), client.getUsername(), args[1]), fd);
		client.setNickname(args[1]);
		//! reply informative a mettre pour que irssi interprete et qu il effectue le changement de nick
	}
}