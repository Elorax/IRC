#include "Server.hpp"

//Param: QUIT [ <quitmsg> ]
// A client session is terminated with a quit message.
// Te server acknowledges this by sending an ERROR message to the client.
//Ex: QUIT :Gone for lunch

void	Server::cmdQuit( std::vector<std::string>& args, int fd ) {

	std::string quitmsg;
	if (args.size() == 1)
		quitmsg = " :" + args[0];

	Client& client = *getClientByFD(fd);
	std::vector<std::string> quitAllChans = {"0"};
	cmdJoin(quitAllChans, fd);
	buildMsg(QUITNOTICE(client.getNickname(), quitmsg), fd);

	close(fd);
}