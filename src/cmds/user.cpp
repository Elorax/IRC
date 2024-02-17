#include "Server.hpp"

void    Server::cmdUser( vecString& args, int fd)
{
    if (args.size() < 4)
        buildMsg(ERR_NEEDMOREPARAMS, fd);

	else if (!getClientByFD(fd)->getUsername().empty())
		buildMsg(ERR_ALREADYREGISTRED, fd);

	else
	{
		getClientByFD(fd)->setUsername(args[0]);
		getClientByFD(fd)->setRealname(args[3]);//setrealname, realname can have spaces
		getClientByFD(fd)->setHostname("Placeholder");
	}
}