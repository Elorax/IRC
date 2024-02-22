#include "Server.hpp"

bool isRunning = true;

static void signalHandler( int signal ) {

	if (signal == SIGINT) {
		std::cout << "\n\x1b[1;5;31mServer socket closed\x1b[0m" << std::endl;
		isRunning = false;
    }
}

fd_set	Server::initReadFDs( void ) {

	fd_set readFDs;
	FD_ZERO(&readFDs);
	FD_SET(getFD(), &readFDs);

	vecClient::iterator it = _clients.begin();
	for (; it != _clients.end(); it++) {
		FD_SET(it->getFD(), &readFDs);
	}
	return (readFDs);

}

fd_set	Server::initWriteFDs( void ) {

	fd_set writeFDs;
	FD_ZERO(&writeFDs);

	vecMessage::iterator it = _messages.begin();
	for (; it != _messages.end(); it++) {
		if (FD_ISSET(it->getFD(), &writeFDs) == 0)
			FD_SET(it->getFD(), &writeFDs);
	}
	return (writeFDs);
}

void	Server::run( void ) {

	while (isRunning) {

		signal(SIGINT, &signalHandler);
		signal(SIGQUIT, SIG_IGN);
		fd_set readFDs = initReadFDs();
		fd_set writeFDs = initWriteFDs();
		//initReadFDs();
		//initWriteFDs();
		//std::cout << "message 1" << std::endl;
		if (select(getMaxFD() + 1, &readFDs, &writeFDs, NULL, NULL) < 0)
			return;

		if (FD_ISSET(getFD(), &readFDs)) {
			if (addClient(readFDs, writeFDs) != 0){
				std::cout << "add client failed" << std::endl;
				continue;
			}
		}
		sendMsgs(writeFDs);
		//debug();	//Presente tous les chans et tous leurs users.
		initBuffer(readFDs, writeFDs);
		//std::cout << "message 2" << std::endl;
	}
}

/*
void	Server::initReadFDs( void ) {

	FD_ZERO(&_readFDs);
	FD_SET(getFD(), &_readFDs);

	vecClient::iterator it = _clients.begin();
	for (; it != _clients.end(); it++) {
		FD_SET(it->getFD(), &_readFDs);
	}

}

void	Server::initWriteFDs( void ) {

	FD_ZERO(&_writeFDs);

	if (_messages.empty())
		return;

	vecMessage::iterator it = _messages.begin();
	for (; it != _messages.end(); it++) {
		if (FD_ISSET(it->getFD(), &_writeFDs) == 0)
			FD_SET(it->getFD(), &_writeFDs);
	}

}
*/

void Server::initBuffer( fd_set &readFDs, fd_set &writeFDs ) {

	int bytesRead;
	char buff[BUFFER_SIZE] = {0};
	vecClient::iterator	it = _clients.begin();

	for (; it < _clients.end(); it++) {

		int fd = it->getFD();
		if (FD_ISSET(fd, &readFDs)) {
			bytesRead = recv(fd, buff, BUFFER_SIZE, 0);	//recv chez nous, read chez lilian 
			std::cout << buff;
			if (bytesRead < 0) {
				delClient(it);
				FD_CLR(fd, &readFDs);
				FD_CLR(fd, &writeFDs);
				it = _clients.begin();
			}

			else if (bytesRead <= 512) {
				it->buffer += buff;
				while (it->buffer.find("\r\n") != std::string::npos)
				{
					std::string tmp;
					tmp = it->buffer.substr(0, it->buffer.find("\r\n") + 2);
					// std::cout << "Debug: ligne envoyee dans parseLine -->" + tmp + "<--" << std::endl;
					parseLine(tmp, fd);
					it->buffer.assign(it->buffer.substr(it->buffer.find("\r\n") + 2, it->buffer.size()));
					

				}
				//voir server.cpp ligne 305 chez fchouky pour gerer le ctrl +d en plein milieu d'une commande (pas de \r\n).
				//std::string tmp(buff);
				//parseLine(tmp, fd);
			}
			memset(buff, '\0', BUFFER_SIZE);
		}
	}
}