#include "Server.hpp"

//Param: <password>
//Must be done first when a client try to register

void	Server::cmdPass( vecString& args, int fd ) {

	//std::cout << "args[1] : " << args[1] << std::endl << "key : " << _chanKey << std::endl;
	std::cout << "DEBUG: password = >" + _chanKey + "<" << std::endl;
	if (!getClientByFD(fd)->getPassword().empty())
		return (buildMsg(ERR_ALREADYREGISTRED, fd));

	if(args.size() != 2)
		return (buildMsg(ERR_NEEDMOREPARAMS(args[0]), fd));

	else if (args[1] != _chanKey)
		buildMsg(ERR_PASSWDMISMATCH, fd);

	else
		getClientByFD(fd)->setPassword(args[1]);
}
