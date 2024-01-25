
#include "inc/IRC.hpp"

bool	isNumber(char *s)
{
	while (*s)
		if (!isdigit(*s++))
			return (false);
	return (true);
}

int	main(int ac, char **av)
{
	//av[1] : port ; av[2] : password
	if (ac != 3)
	{
		std::cerr << "Error: Wrong number of arguments" << std::endl;
		return (1);
	}
	if(!isNumber(av[1]))
	{
		std::cerr << "Error: PORT must be a number" << std::endl;
		return (1);
	}
	std::string port(av[1]);
	std::string password(av[2]);
	Server server(port, password);
	server.run();
	
	return (0);
	
}

void	Server::run()
{
	FD_ZERO(&_readFDs);
	FD_ZERO(&_writeFDs);
	FD_SET(this->getFD(), &_readFDs);

	while (true)	//variable globale mise a zero par une interruption ?
	{
	//Penser a une solution de quitter le programme proprement en cas de ctrl c avec des signaux.
		if (select(this->getMaxFD() + 1, &_readFDs, &_writeFDs, NULL, NULL) < 0)
		{
			std::cerr << "error ta mere" << std::endl;
			break ;
		}

		if (FD_ISSET(this->getFD(), &_readFDs)) //maginoire listen te titille le this->donc tu rentres 
		{
			if (this->addClient(_readFDs, _writeFDs) != 0)   //Ptete ca marche pas avec une ref tavu
			{
				std::cerr << "accept failed" << std::endl;
				continue;
			}
		}

		int					bytesRead;
		char				buff[BUFFER_SIZE] = {0};
		vecClient::iterator	it = _clients.begin();

		for (; it < _clients.end(); it++) {

			int fd = it->getFD();
			if (FD_ISSET(fd, &_readFDs)) {
				bytesRead = recv(fd, buff, BUFFER_SIZE, 0);

				if (bytesRead < 0) {
					delClient(it);
					FD_CLR(fd, &_readFDs);
					FD_CLR(fd, &_writeFDs);
					it = _clients.begin();
				}

				//if the client exceed the IRC allowed amount of bytes, we don't process it.
				else if (bytesRead <= 512)
					//processClient(buff); //processClient vide pour l'instant. parseLine a la place pour faire des tests.
					parseLine(buff, fd);
				memset(buff, '\0', BUFFER_SIZE);
			}
		}
		// faire une fonction qui envoie tous les messages a envoyer (les messages etant des reponses (erreur ou non ERR/RPL/non defini))
		
		// faire une boucle sur les clients et quand FD_ISSET(fd_client, read) reussi, read ce que tu recois du client puis parser la ligne, si elle finit par \r\n l executer, si tu reconnais une commande la faire sinon rien faire
	}



	//Ajouter un nouveau client (accept, update les fd sets)

	//Verifier si un client deja accepte fait une action
	//handleClient()

}

vecClient::iterator	Server::getClientByFD( int fd ) {

	vecClient::iterator	it = _clients.begin();
	for (; it != _clients.end(); it++) {
		if (it->getFD() == fd);
			return (it);
	}
}

vecClient::iterator	Server::getClientByName( std::string& user ) {

	vecClient::iterator	it = _clients.begin();
	for (; it != _clients.end(); it++) {
		if (it->getUserName() == user);
			return (it);
	}
}

void	Server::delClient( vecClient::iterator toDel ) {

	close(toDel->getFD());
	_clients.erase(toDel);
}

void processClient( char *buff ) {
	
	// Connection Establishment: The IRC client establishes a connection with the IRC server1.

	// Registration: The client must first register its connection. This is done by sending two messages: NICK and USER.

	// NICK specifies the user’s nickname.
	// USER provides additional information about the user (realName, hostName).
	// Welcome Message: Assuming the chosen nickname is not already taken, the IRC server will send back a RPL_WELCOME reply (which is assigned code 001). This reply includes the full client identifier (<nick>!<user>@<host>), which is used in other types of messages2.

	// Error Handling: If a user tries to register with a nickname that is already taken, the server will send back an ERR_NICKNAMEINUSE reply (code 433)2.

	//while (1);


}