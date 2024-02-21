#include "Server.hpp"

//if no mask, show all visible users without common channel with requester
//same with 0
//otherwise return list of info matching the <mask> given
void	Server::cmdWho( vecString& args, int fd ) {

	if (!isUserSet(*getClientByFD(fd)))
		return (buildMsg(ERR_NOTREGISTERED, fd));

	if (args.size() == 1 || args[1][0] == '0')
		whoAll(fd);

	else if (doesChanExist(args[1])) {
		Channel& target = getChannel(args[1]);
		whoChannel(target, fd);
	}

	else if (doesUserExist(args[1])) {
		Client& target = *getClientByName(args[1]);
		whoClient(target, fd);
	}
}

/* --------------------------------- Helpers -------------------------------- */

std::string convertVecString( vecString args ) {

	std::string result;
	vecString::iterator it = args.begin();

	for (; it != args.end(); it++)
		result += *it;
	return (result);
}

void	Server::whoAll( int requester ) {

	std::string	nique = getClientByFD(requester)->getNickname();
	vecChannel::iterator itChan = _channels.begin();

	for (; itChan != _channels.end(); itChan++) {

		vecClient chanUsers = itChan->getChanUsers();
		vecClient::iterator itUser = chanUsers.begin();

		if (!itChan->isUserOnChan(requester))
			for (; itUser != chanUsers.end(); itUser++)
				whoClient(*itUser, requester);
	}
}

void	Server::whoClient( Client& target, int requesterFD ) {

	vecString	whoMsg;

	if (!target.getUserChanList().empty()) {
		whoMsg.push_back(target.getUserChanList().back().getName());
		whoMsg.push_back(" ");
	}
	whoMsg.push_back(target.getUsername());
	whoMsg.push_back(" ");
	whoMsg.push_back(target.getNickname());
	whoMsg.push_back(" ");
	whoMsg.push_back(target.getHostname());
	whoMsg.push_back(" ");
	whoMsg.push_back(target.getRealname());

	buildMsg(RPL_WHOREPLY(convertVecString(whoMsg)), requesterFD);
	buildMsg(RPL_ENDOFWHO(target.getNickname()), requesterFD);

}

void	Server::whoChannel( const Channel& target, int requesterFD ) {

	vecString whoMsg;
	whoMsg.push_back(target.getName());

	vecClient chanUsers = target.getChanUsers();
	vecClient::iterator itUser = chanUsers.begin();
	for (; itUser != chanUsers.end(); itUser++) {
		whoMsg.push_back(" ");
		whoMsg.push_back(itUser->getUsername());
	}

	buildMsg(RPL_WHOREPLY(convertVecString(whoMsg)), requesterFD);
	buildMsg(RPL_ENDOFWHO(target.getName()), requesterFD);
}

