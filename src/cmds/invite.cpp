#include "Server.hpp"

//Parametres : INVITE <nickname> <channel>
//Invite un utilisateur a rejoindre un channel, il recoit une notif qui lui dit que qqn veut qu'il rejoigne tel chan
//Il faut etre membre du chan pour inviter quelqu'un sinon la commande echoue
//L'utilisateur invite recoit une notif, l'utilisateur qui invite recoit une RPL_INVITING
//La notification est une repoonse non ERR/RPL
//Reste a faire : Envoi notification, envoi RPL_INVITING.

void	Server::cmdInvite( vecString& args, int fd ) {


	if (!isUserSet(*getClientByFD(fd)))
		return (buildMsg(ERR_NOTREGISTERED, fd));

	if (args.size() != 3)
		return (buildMsg(ERR_NEEDMOREPARAMS(args[0]), fd));

	std::string nickname = args[1];
	Client& client = *getClientByFD(fd);
	Channel& chan = getChanByRef(args[2]);

	if (!doesUserExist(args[1]))
		buildMsg(ERR_NOSUCHNICK(args[1]), fd);

	else if (!doesChanExist(args[2]))
		buildMsg(ERR_NOSUCHNICK(args[2]), fd);

	else if (!chan.isUserOnChan(fd))
		buildMsg(ERR_NOTONCHANNEL(client.getNickname()), fd);

	else if (chan.isInviteOnly() && !chan.isUserChanOp(fd))
		buildMsg(ERR_CHANOPRIVSNEEDED(chan.getName()), fd);

	else if (chan.isUserOnChan(nickname))
		buildMsg(ERR_USERONCHANNEL(nickname, chan.getName()), fd);

	else {

		buildMsg(RPL_INVITING(getClientByFD(fd)->getNickname(), chan.getName(), nickname), fd);
		buildMsg(JOINNOTICE(nickname, getClientByName(nickname)->getUsername(), chan.getName()), fd);
		chan.addUser(*getClientByName(nickname));
	}

}