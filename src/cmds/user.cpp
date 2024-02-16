#include "Server.hpp"

void    Server::cmdUser( std::vector<std::string>& args, int fd)
{
    if (args.size() < 4)
        buildMsg(ERR_NEEDMOREPARAMS, fd);

	else if (!getClientByFD(fd)->getUserName().empty())
		buildMsg(ERR_ALREADYREGISTRED, fd);

	else
		getClientByFD(fd)->setUserName(args[0]);//setrealname, realname can have spaces
}