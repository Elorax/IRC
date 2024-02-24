#include "Server.hpp"

//PARAMETRES : pong <sever1> [<server2>]
//param <server2> optionnel et ne nous interesse pas pour ce projet car on ne gere qu'un seul serveur.
void	Server::cmdPong( vecString& args, int fd ) {

	Client& client = *getClientByFD(fd);

	if (args[1].empty())
		return (buildMsg(ERR_NOORIGIN(getClientByFD(fd)->getNickname()), fd));

	return (buildMsg(PONGNOTICE(client.getNickname()), fd));
}