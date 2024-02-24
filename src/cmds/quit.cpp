#include "Server.hpp"
//! Quitte le serveur : FAut CARREMENT LE REVOIR. J'l'ai pas fait cettenuit deso pas deso.
//Param: QUIT [ <quitmsg> ]
// A client session is terminated with a quit message.
// Te server acknowledges this by sending an ERROR message to the client.
//Ex: QUIT :Gone for lunch

void	Server::cmdQuit( vecString& args, int fd ) {

	//std::cout << "DEBUG: QUIT COMMAND" << std::endl;
	vecString quitAllChans;
	quitAllChans.push_back("JOIN");
	quitAllChans.push_back("0");
	if (args.size() == 2)
		quitAllChans.push_back(args[1]);
	cmdJoin(quitAllChans, fd);
	{
		vecClient::iterator it = _clients.begin();
		for (; it != _clients.end(); it++)
		{
			if (it->getFD() == fd)
			{
				_clients.erase(it);
				break;
				//delUSer
			}
		}
	}
	vecMessage::iterator it = _messages.begin();
	while (it != _messages.end())
	{
		if (it->getFD() == fd)
			it = _messages.erase(it);
		else
			it++;
	}
	/*//!ancienne version, avec le it != messages.begin() pour la posterite...
	vecMessage::iterator it = _messages.begin();
	for(; it != _messages.begin(); it++)
	{
		if (it->getFD() == fd)
		{
			std::string tmp = "";
			it->setMsg(tmp);
			it->setFD(0);
		}
	}*/
	close(fd);

}