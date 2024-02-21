#include "Server.hpp"

//Param: QUIT [ <quitmsg> ]
// A client session is terminated with a quit message.
// Te server acknowledges this by sending an ERROR message to the client.
//Ex: QUIT :Gone for lunch

void	Server::cmdQuit( vecString& args, int fd ) {

	std::string quitmsg;
	if (args.size() == 2)
		quitmsg = " :" + args[1];

	Client& client = *getClientByFD(fd);
	vecString quitAllChans;
	quitAllChans.push_back("JOIN");
	quitAllChans.push_back("0");
	cmdJoin(quitAllChans, fd);
	buildMsg(QUITNOTICE(client.getNickname(), quitmsg), fd);

	close(fd);
}