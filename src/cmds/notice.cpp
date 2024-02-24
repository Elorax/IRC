#include "Server.hpp"

//Param: NOTICE <msgtarget> <text>
void	Server::cmdNotice( vecString& args, int fd ) {

	Client& client = *getClientByFD(fd);

	if (!isUserSet(*getClientByFD(fd)))
		return (buildMsg(ERR_NOTREGISTERED, fd));

	if (args.size() < 3 )
		return;

	if (args[1][0] == '#') {

		if (doesChanExist(args[1])) {
			Channel &chan = getChanByRef(args[1]);
			if (chan.isUserOnChan(fd))
				buildMsg(MSGNOTICE(client.getNickname(), client.getUsername(), args[1], args[2]), chan, fd);
		}
	}
	else if (doesUserExist(args[1]))
		buildMsg(MSGNOTICE(client.getNickname(), client.getUsername(), args[1], args[2]), getClientByName(args[1])->getFD());
}