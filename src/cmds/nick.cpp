#include "Server.hpp"

//Param: NICK <nickname>
void	Server::cmdNick( vecString& args, int fd ) {

	Client& client = *getClientByFD(fd);

	if (client.getPassword().empty())
		return (buildMsg(ERR_NOTREGISTERED, fd));

	if (args[1].empty())
		buildMsg(ERR_NONICKNAMEGIVEN, fd);

	else if (args.size() < 2)
		buildMsg(ERR_NEEDMOREPARAMS(args[0]), fd);

	else if (!isAvailNick(args[1]))
		buildMsg(ERR_NICKNAMEINUSE(client.getNickname(), args[1]), fd);

	else if (!isValidNick(args[1]))
		buildMsg(ERR_ERRONEUSNICKNAME(args[1]), fd);

	else {
		std::string nickMsg;
		buildMsg(NICKNOTICE(client.getNickname(), client.getUsername(), args[1]), fd);
		client.setNickname(args[1]);
	}
}