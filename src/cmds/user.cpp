#include "Server.hpp"


//VERIFIER QUE PASS A ETE CORRECTEMENT EXECUTE : _chanKey n'est PAS VIDE
//Sinon user non registered sur le serveur et interdit d'accès
//
void    Server::cmdUser( vecString& args, int fd )
{
	if (getClientByFD(fd)->getPassword().empty())
		return (buildMsg(ERR_NOTREGISTERED, fd));

    if (args.size() < 4)
        buildMsg(ERR_NEEDMOREPARAMS(args[0]), fd);

	else if (!getClientByFD(fd)->getUsername().empty())
		buildMsg(ERR_ALREADYREGISTRED, fd);

	else
	{
		getClientByFD(fd)->setUsername(args[1]);
		getClientByFD(fd)->setHostname("Placeholder");
		getClientByFD(fd)->setRealname(args[4]);//setrealname, realname can have spaces
		buildMsg(RPL_WELCOME(args[1]) + RPL_YOURHOST + RPL_CREATED + RPL_MYINFO, fd);
	}
}
