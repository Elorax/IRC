#include "Server.hpp"

//Parameters: <channel> *( "," <channel> ) <user> *( "," <user> ) [<comment>]
//Kick est censee utiliser la commande Part
void	Server::cmdKick( vecString& args, int fd ) {

	if (!isUserSet(*getClientByFD(fd)))
		return (buildMsg(ERR_NOTREGISTERED, fd));

	if (args.size() < 3)
		return (buildMsg(ERR_NEEDMOREPARAMS(args[0]), fd));

	vecString chans = splitParamOnComas(args[1]);
	vecString users = splitParamOnComas(args[2]);

	if (chans.size() == 1 && users.size() >= 1)
		kickUsers(args, users, chans, fd);

	else if (users.size() == 1 && chans.size() >= 1 )
		kickChans(args, users, chans, fd);

	else if (users.size() == chans.size() && users.size() >= 1)
		kickChansUsers(args, users, chans, fd);
}

/* --------------------------------- Helpers -------------------------------- */

// Kick multiple users from one channel
void	Server::kickUsers( vecString args, vecString users, vecString chans, int requesterFD ) {

	vecString kickMsg;
	vecString::iterator itChan = chans.begin();
	vecString::iterator itUser = users.begin();

	if (!getChannel(*itChan).isUserOnChan(requesterFD))
		return (buildMsg(ERR_NOTONCHANNEL(*itChan), requesterFD));

	else if (getChannel(*itChan).isUserOnChan(*itUser))
		return (buildMsg(ERR_USERNOTINCHANNEL(*itUser, *itChan), requesterFD));

	kickMsg.push_back(getClientByFD(requesterFD)->getNickname());
	for (; itUser != users.end(); itUser++) {
		if (!getChannel(*itChan).isUserChanOp(requesterFD))
			return (buildMsg(ERR_CHANOPRIVSNEEDED(*itChan), requesterFD));

		vecString args_part;
		kickMsg.push_back(" ");
		kickMsg.push_back(*itUser);
		args_part.push_back(*itChan);

		if (args.size() == 3)
			args_part.push_back(args[2]);
		else
			args_part.push_back(getClientByFD(requesterFD)->getNickname());
		cmdPart(args_part, getClientByName(*itUser)->getFD());
	} 
}

// Kick one user from multiple channels
void	Server::kickChans( vecString args, vecString users, vecString chans, int requesterFD ) {

	vecString kickMsg;
	vecString::iterator itChan = chans.begin();
	vecString::iterator itUser = users.begin();

	for (; itChan != chans.end(); itChan++) {

		if (!getChannel(*itChan).isUserOnChan(requesterFD))
			return (buildMsg(ERR_NOTONCHANNEL(*itChan), requesterFD));

		else if (!getChannel(*itChan).isUserOnChan(*itUser))
			return (buildMsg(ERR_USERNOTINCHANNEL(*itUser, *itChan), requesterFD));

		else if (!getChannel(*itChan).isUserChanOp(requesterFD))
			return (buildMsg(ERR_CHANOPRIVSNEEDED(*itChan), requesterFD));

		vecString args_part;
		kickMsg.push_back(" ");
		kickMsg.push_back(*itUser);
		args_part.push_back(*itChan);

		if (args.size() == 3)
			args_part.push_back(args[2]);
		else
			args_part.push_back(getClientByFD(requesterFD)->getNickname());
		cmdPart(args_part, getClientByName(*itUser)->getFD());
	}
}

// Kick multiple users from multiple channels. itUser[0] will be kick from itChan[0]
void	Server::kickChansUsers( vecString args, vecString users, vecString chans, int requesterFD ) {

	vecString kickMsg;
	vecString::iterator itChan = chans.begin();
	vecString::iterator itUser = users.begin();

	for (; itUser != users.end() || itChan != chans.end(); itUser++, itChan++) {

		if (!getChannel(*itChan).isUserOnChan(requesterFD))
			return (buildMsg(ERR_NOTONCHANNEL(*itChan), requesterFD));

		else if (getChannel(*itChan).isUserOnChan(*itUser))
			return (buildMsg(ERR_USERNOTINCHANNEL(*itUser, *itChan), requesterFD));

		else if (!getChannel(*itChan).isUserChanOp(requesterFD))
			return (buildMsg(ERR_CHANOPRIVSNEEDED(*itChan), requesterFD));

		vecString args_part;
		kickMsg.push_back(" ");
		kickMsg.push_back(*itUser);
		args_part.push_back(*itChan);

		if (args.size() == 3)
			args_part.push_back(args[2]);
		else
			args_part.push_back(getClientByFD(requesterFD)->getNickname());
		cmdPart(args_part, getClientByName(*itUser)->getFD());
	}
}
