#include "Server.hpp"

//Param: <msgtarget> <text>
void	Server::cmdNotice( std::vector<std::string>& args, int fd ) {

	if (args.size() < 2 )
		return;

	//Cible : channel
	if (args[0][0] == '#') {

		if (!doesChanExist(args[0])) {
			Channel &chan = getChannel(args[0]);
			if (chan.isUserOnChan(fd))
				buildMsg(args[1], chan);
		}
	}

	else if (doesUserExist(args[0]))
		if (doesUserExist(args[0]))
			buildMsg(args[2], getClientByName(args[0])->getFD());
}