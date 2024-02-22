#include "Server.hpp"

//Parametres : PRIVMSG <target> <message>
//Ex : PRIVMSG lea :coucou ca va ?
//Ex : PRIVMSG #iciclafete :salut les zboubs

void	Server::cmdPrivmsg( vecString& args, int fd ) {

	Client& client = *getClientByFD(fd);

	if (!isUserSet(client))
		return (buildMsg(ERR_NOTREGISTERED, fd));

	if (args.size() < 2)
		buildMsg(ERR_NORECIPIENT(args[0]), fd);

	else if (args.size() < 3)
		buildMsg(ERR_NOTEXTTOSEND, fd);

	//Msg to channel
	else if (args[1][0] == '#') {

		if (!doesChanExist(args[1]))
			buildMsg(ERR_NOSUCHNICK(args[1]), fd);

		else {

			Channel &chan = getChanByRef(args[1]);
			if (!chan.isUserOnChan(fd))
				buildMsg(ERR_CANNOTSENDTOCHAN(chan.getName()), fd);
			else if (!client.isChanInList(chan))
				buildMsg(ERR_CANNOTSENDTOCHAN(chan.getName()), fd);
			else
				buildMsg(":" + client.getNickname() + "!~" + client.getUsername() + "@" + _name + " PRIVMSG " + args[1] + " " + args[2] + "\r\n", getChanByRef(args[1]));
		}
	}
	//Msg to user.
	//args[1] : nom du destinataire
	//args[2] : message
	else {
		if (!doesUserExist(args[1]))
			buildMsg(ERR_NOSUCHNICK(args[1]), fd);
		else
			buildMsg(":" + client.getNickname() + "!~" + args[1] + "@" + _name + " PRIVMSG " + args[1] + " :" + args[2] + "\r\n", getClientByName(args[1])->getFD());
	}
}
