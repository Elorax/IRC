#include "Server.hpp"

//if no mask, show all visible users without common channel with requester
//same with 0
//otherwise return list of info matching the <mask> given
void	Server::cmdWho( vecString& args, int fd ) {

	if (args.empty() || args[0][0] == '0')
		whoAll(fd);

	else if (doesChanExist(args[0])) {
		Channel& target = getChannel(args[0]);
		whoChannel(target, fd);
	}

	else if (doesUserExist(args[0])) {
		Client& target = *getClientByName(args[0]);
		whoClient(target, fd);
	}
}

/* --------------------------------- Helpers -------------------------------- */

void	Server::whoAll( int requester ) {

	std::string	requester = getClientByFD(requester)->getNickname();
	vecChannel::iterator itChan = _channels.begin();

	for (; itChan != _channels.end(); itChan++) {

		refClient chanUsers = itChan->getChanUsers();
		refClient::iterator itUser = chanUsers.begin();

		if (!itChan->isUserOnChan(requester))
			for (; itUser != chanUsers.end(); itUser++)
				whoClient(*itUser, requester);
	}
}

void	Server::whoClient( Client& target, int requesterFD ) {

	vecString	whoMsg;

	whoMsg.push_back(target.getNickname());
	whoMsg.push_back(" ");
	if (!target.getUserChanList().empty())
		whoMsg.push_back(target.getUserChanList().back().getName());
	whoMsg.push_back(" ");
	whoMsg.push_back(target.getUsername());
	whoMsg.push_back(" ");
	whoMsg.push_back(target.getRealname());
	whoMsg.push_back(" ");
	whoMsg.push_back(target.getHostname());

	buildMsg(RPL_WHOREPLY(getClientByFD(requesterFD)->getNickname(), convertVecString(whoMsg)), requesterFD);
}

void	Server::whoChannel( const Channel& target, int requesterFD ) {

	vecString whoMsg;
	whoMsg.push_back(target.getName());

	refClient chanUsers = target.getChanUsers();
	refClient::iterator itUser = chanUsers.begin();
	for (; itUser != chanUsers.end(); itUser++) {
		whoMsg.push_back(" ");
		whoMsg.push_back(itUser->getUsername());
	}

	buildMsg(RPL_WHOREPLY(getClientByFD(requesterFD)->getNickname(), convertVecString(whoMsg)), requesterFD);
}

std::string convertVecString( vecString args ) {

	std::string result;
	vecString::iterator it = args.begin();

	for (; it != args.end(); it++)
		result += *it;
}