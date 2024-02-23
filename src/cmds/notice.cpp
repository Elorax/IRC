#include "Server.hpp"
//! A tester, il marche pas parce que pas bon format de message recu.
//Param: NOTICE <msgtarget> <text>
void	Server::cmdNotice( vecString& args, int fd ) {

	Client& client = *getClientByFD(fd);

	if (!isUserSet(*getClientByFD(fd)))
		return (buildMsg(ERR_NOTREGISTERED, fd));

	if (args.size() < 3 )
		return;

	//Cible : channel
	if (args[1][0] == '#') {

		if (!doesChanExist(args[1])) {
			Channel &chan = getChanByRef(args[1]);
			if (chan.isUserOnChan(fd))
				buildMsg(MSGNOTICE(client.getNickname(), client.getUsername(), args[1], args[2]), chan, fd);
		}
	}

	else if (doesUserExist(args[1]))
		buildMsg(MSGNOTICE(client.getNickname(), client.getUsername(), args[1], args[2]), getClientByName(args[1])->getFD());

}