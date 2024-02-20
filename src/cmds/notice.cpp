#include "Server.hpp"

//Param: NOTICE <msgtarget> <text>
void	Server::cmdNotice( vecString& args, int fd ) {

	if (args.size() < 3 )
		return;

	//Cible : channel
	if (args[1][0] == '#') {

		if (!doesChanExist(args[1])) {
			Channel &chan = getChannel(args[1]);
			if (chan.isUserOnChan(fd))
				buildMsg(args[2], chan);
		}
	}

	else if (doesUserExist(args[1]))
		if (doesUserExist(args[1]))
			buildMsg(args[2], getClientByName(args[1])->getFD());
}