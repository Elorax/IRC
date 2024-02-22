#include "Server.hpp"

//Param: NICK <nickname>

void	Server::cmdNick( vecString& args, int fd ) {

	if (getClientByFD(fd)->getPassword().empty())
		return (buildMsg(ERR_NOTREGISTERED, fd));

	//std::cout << "cmdNick, args[1] = >" << args[1] << "<" << std::endl;
	if (args[1].empty())
		buildMsg(ERR_NONICKNAMEGIVEN, fd);

	else if (args.size() < 2)
		buildMsg(ERR_NEEDMOREPARAMS(args[0]), fd);
	
	else if (!isAvailNick(args[1]))
		buildMsg(ERR_NICKNAMEINUSE(getClientByFD(fd)->getNickname(), args[1]), fd);

	else if (!isValidNick(args[1]))
		{
			std::cout << "DEBUG: le nick est pas bon kevin" << std::endl;
			buildMsg(ERR_ERRONEUSNICKNAME(args[1]), fd);
		}

	else
	{
		std::string reponse;
		//! CETTE REPONSE EST RECONNUE PAR IRSSI, S'EN INSPIRER POur LA SUITE
		//! AVEC OU SANS ~ LA REPONSE EST CORRECTE ET COMPRISE
		
		reponse = ":" + getClientByFD(fd)->getNickname() + "!~" + getClientByFD(fd)->getUsername() + "@" + _name + " NICK " + args[1] + "\r\n";
		buildMsg(reponse, fd);
		getClientByFD(fd)->setNickname(args[1]);
		//! reponse informative a mettre pour que irssi interprete et qu il effectue le changement de nick
	}
}