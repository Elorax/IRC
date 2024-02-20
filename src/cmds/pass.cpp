#include "Server.hpp"

//Param: <password>
//Must be done first when a client try to register

void	Server::cmdPass( vecString& args, int fd ) {

	std::cout << "args[1] : " << args[1] << std::endl << "key : " << _chanKey << std::endl;
	if(args.size() == 1)
		buildMsg(ERR_NEEDMOREPARAMS, fd);

	else if (!getClientByFD(fd)->getPassword().empty())
		buildMsg(ERR_ALREADYREGISTRED, fd);

	else if (args[1] != _chanKey)
		buildMsg(ERR_PASSWDMISMATCH, fd);

	else
		getClientByFD(fd)->setPassword(args[0]);
}
