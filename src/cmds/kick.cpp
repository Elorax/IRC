#include "Server.hpp"

//Parameters: <channel> *( "," <channel> ) <user> *( "," <user> ) [<comment>]
//Kick est censee utiliser la commande Part
void	Server::cmdKick( vecString& args, int fd ) {

	if (args.size() < 2)
		return (buildMsg(ERR_NEEDMOREPARAMS, fd));

	std::string requester = getClientByFD(fd)->getUsername();
	vecString chans = splitParamOnComas(args[0]);
	vecString users = splitParamOnComas(args[1]);

	vecString kickMsg;
	vecString::iterator itChan = chans.begin();
	vecString::iterator itUser = users.begin();
	if (chans.size() == 1 && users.size >= 1) {

		if (!itChan->isUserChanOp(requester))
				return (buildMsg(ERR_CHANOPRIVSNEEDED, fd));
		kickMsg.pushback(getClientByFD()->getUsername());

		for (; itUser != users.end(); itUser++) {
			if (!itChan->isUserOnChan(itUser->getFD()))
				return (buildMsg(ERR_USERNOTINCHANNEL, fd));

			kickMsg.pushback(" ");
			kickMsg.pushback(itUser->getName());
		}
		cmdPart(*itChan);
	}

	else if (users.size == 1 && chans.size() >= 1 ) {
		for (; itUser != users.end(); itChan++, itUser++) {

			if (!itChan->isUserOnChan(itUser->getFD()))
				return (buildMsg(ERR_USERNOTINCHANNEL, fd));

			else if (!itChan->isUserChanOp(requester))
				return (buildMsg(ERR_CHANOPRIVSNEEDED, fd));

			kickMsg.pushback();
			Client& user = getClientByFD(itUser->getFD());
			cmdPart()

		}
	}
}
