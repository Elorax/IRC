#include "Server.hpp"

//Parametres : INVITE <nickname> <channel>
//Invite un utilisateur a rejoindre un channel, il recoit une notif qui lui dit que qqn veut qu'il rejoigne tel chan
//Il faut etre membre du chan pour inviter quelqu'un sinon la commande echoue
//L'utilisateur invite recoit une notif, l'utilisateur qui invite recoit une RPL_INVITING
//La notification est une repoonse non ERR/RPL
//Reste a faire : Envoi notification, envoi RPL_INVITING.

void	Server::cmdInvite( vecString& args, int fd ) {


	if (args.size() != 2)
		buildMsg(ERR_NEEDMOREPARAMS, fd);

	std::string nickname = args[0];
	Channel& chan = getChannel(args[1]);

	if (!doesChanExist(args[1]) || !doesUserExist(args[0]))
		buildMsg(ERR_NOSUCHNICK, fd);

	else if (!chan.isUserOnChan(fd))
		buildMsg(ERR_NOTONCHANNEL, fd);

	else if (chan.isInviteOnly() && !chan.isUserChanOp(fd))
		buildMsg(ERR_CHANOPRIVSNEEDED, fd);

	else if (chan.isUserOnChan(nickname))
		buildMsg(ERR_USERONCHANNEL, fd);

	else {
		chan.addUserOnChan(*getClientByName(nickname));
		 //Send to requester invite in process
		buildMsg(RPL_INVITING(args[0], chan.getName()), fd);
		// Send to target invite notice
		buildMsg(INVITENOTICE(getClientByFD(fd)->getNickname(), chan.getName()), getClientByName(nickname)->getFD());
	}

}