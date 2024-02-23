#include "Server.hpp"
//! Quitte le serveur : FAut CARREMENT LE REVOIR. J'l'ai pas fait cettenuit deso pas deso.
//Param: QUIT [ <quitmsg> ]
// A client session is terminated with a quit message.
// Te server acknowledges this by sending an ERROR message to the client.
//Ex: QUIT :Gone for lunch

void	Server::cmdQuit( vecString& args, int fd ) {

	std::cout << "QUIT COMMAND\r\n" << std::endl;
	vecString quitAllChans;
	if (args.size() == 2)
		quitAllChans.push_back(args[1]);
	quitAllChans.push_back("JOIN");
	quitAllChans.push_back("0");
	cmdJoin(quitAllChans, fd);

	vecClient::iterator del = getClientByFD(fd);
	delClient(del);
}