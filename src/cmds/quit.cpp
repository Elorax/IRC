#include "Server.hpp"

//Param: QUIT [ <quitmsg> ]
//A client session is terminated with a quit message.
//The server acknowledges this by sending an ERROR message to the client.
//Ex: QUIT :Gone for lunch
void	Server::cmdQuit( vecString& args, int fd ) {

	vecString quitAllChans;
	quitAllChans.push_back("JOIN");
	quitAllChans.push_back("0");
	if (args.size() == 2)
		quitAllChans.push_back(args[1]);
	cmdJoin(quitAllChans, fd);

	for (vecClient::iterator it = _clients.begin(); it != _clients.end(); it++) {
		if (it->getFD() == fd) {
			_clients.erase(it);
			break;
		}
	}

	vecMessage::iterator it = _messages.begin();
	while (it != _messages.end()) {
		if (it->getFD() == fd)
			it = _messages.erase(it);
		else
			it++;
	}
	close(fd);
}