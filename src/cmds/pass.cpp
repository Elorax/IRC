#include "Server.hpp"

//Param: <password>
//Must be done first when a client try to register

void	Server::cmdPass( std::vector<std::string>& args, int fd ) {

	if(args.empty())
		buildMsg(ERR_NEEDMOREPARAMS, fd);

	else if (!getClientByFD(fd)->getPassword().empty())
		buildMsg(ERR_ALREADYREGISTRED, fd);

	else if (getClientByFD(fd)->getPassword() != args[0])
		buildMsg(ERR_PASSWDMISMATCH, fd);

	else
		getClientByFD(fd)->setPassword(args[0]);
}