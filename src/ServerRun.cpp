#include "Server.hpp"
#include <cerrno>

bool isRunning = true;
int	servFD;

static void signalHandler( int signal ) {

	if (signal == SIGINT) {
		std::cout << "\n\x1b[1;5;31mServer socket closed\x1b[0m" << std::endl;
		close(servFD);
		isRunning = false;
    }
	if (signal == SIGPIPE)
		return;
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

	servFD = getFD();
	while (isRunning) {
		signal(SIGINT, &signalHandler);
		signal(SIGPIPE, &signalHandler);
		signal(SIGQUIT, SIG_IGN);

		fd_set readFDs = initReadFDs();
		fd_set writeFDs = initWriteFDs();

		if (select(getMaxFD() + 1, &readFDs, &writeFDs, NULL, NULL) < 0) {
			std::cout << "error select(): " << std::strerror(errno) << std::endl;
			return;
		}

		if (FD_ISSET(getFD(), &readFDs)) {
			if (addClient(readFDs, writeFDs) != 0){
				std::cout << "add client failed" << std::endl;
				continue;
			}
		}
		sendMsgs(writeFDs);
		initBuffer(readFDs, writeFDs);
	}
}

void Server::initBuffer( fd_set &readFDs, fd_set &writeFDs ) {

	int bytesRead;
	char buff[BUFFER_SIZE] = {0};
	vecClient::iterator	it = _clients.begin();

	for (; it != _clients.end(); it++) {

		int fd = it->getFD();
		if (FD_ISSET(fd, &readFDs)) {

			bytesRead = recv(fd, buff, BUFFER_SIZE, 0);
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
					if (parseLine(tmp, fd) == 0)
						it->buffer.assign(it->buffer.substr(it->buffer.find("\r\n") + 2, it->buffer.size()));
					else
						return ;
				}
			}
			memset(buff, '\0', BUFFER_SIZE);
		}
	}
}