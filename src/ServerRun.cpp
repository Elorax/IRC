#include "Server.hpp"

bool isRunning = true;

void	Server::run( void ) {

	while (isRunning) {

		signal(SIGINT, &signalHandler);
		signal(SIGQUIT, SIG_IGN);
		initReadFDs();
		initWriteFDs();

		if (select(getMaxFD() + 1, &_readFDs, &_writeFDs, NULL, NULL) < 0)
			return;

		if (FD_ISSET(getFD(), &_readFDs))
			if (addClient(_readFDs, _writeFDs) != 0)
				continue;

		initBuffer();
	}
}

static void signalHandler( int signal ) {

	if (signal == SIGINT) {
		std::cout << "\n\x1b[1;5;31mServer socket closed\x1b[0m" << std::endl;
		isRunning = false;
    }
}

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

void Server::initBuffer( void ) {

	int bytesRead;
	char buff[BUFFER_SIZE] = {0};
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

			else if (bytesRead <= 512) {
				std::string tmp(buff);
				parseLine(tmp, fd);
			}
			memset(buff, '\0', BUFFER_SIZE);
		}
	}
}