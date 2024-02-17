#include "Server.hpp"

//Parametres : PRIVMSG <target> <message>
//Ex : PRIVMSG lea :coucou ca va ?
//Ex : PRIVMSG #iciclafete :salut les zboubs

void	Server::cmdPrivmsg( vecString& args, int fd ) {

	if (args.size() < 1)
		buildMsg(ERR_NORECIPIENT, fd);
	else if (args.size() < 2)
		buildMsg(ERR_NOTEXTTOSEND, fd);

	//Msg to channel
	else if (args[0][0] == '#') {

		if (!doesChanExist(args[0]))
			buildMsg(ERR_NOSUCHNICK, fd);

		else {

			Channel &chan = getChannel(args[0]);
			if (!chan.isUserOnChan(fd))
				buildMsg(ERR_CANNOTSENDTOCHAN, fd);
			else
				buildMsg(args[1], chan);
		}
	}

	//Msg to user.
	else {
		if (!doesUserExist(args[0]))
			buildMsg(ERR_NOSUCHNICK, fd);
		else
			buildMsg(args[1], getClientByName(args[0])->getFD());
	}
}
