#include "Server.hpp"
#include <cerrno>

bool isRunning = true;
//std::vector<int> my_fds;
int	servFD;
static void signalHandler( int signal ) {

	if (signal == SIGINT) {
		std::cout << "\n\x1b[1;5;31mServer socket closed\x1b[0m" << std::endl;
		close(servFD);
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

//fonction de debug, inutile
/*
void printFDSet(const fd_set& fds) {
    std::cout << "File Descriptors in the set: ";
    for (int i = 0; i < FD_SETSIZE; ++i) {
        if (FD_ISSET(i, &fds)) {
            std::cout << i << " ";
        }
    }
    std::cout << std::endl;
}
*/

void	Server::run( void ) {

	servFD = getFD();
	while (isRunning) {
		signal(SIGINT, &signalHandler);
		signal(SIGQUIT, SIG_IGN);

		fd_set readFDs = initReadFDs();
		fd_set writeFDs = initWriteFDs();

		if (select(getMaxFD() + 1, &readFDs, &writeFDs, NULL, NULL) < 0)
		{
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
					if (parseLine(tmp, fd) == 0)	//Pas de commande speciale
						it->buffer.assign(it->buffer.substr(it->buffer.find("\r\n") + 2, it->buffer.size()));
					else	//QUIT
					{
						return ;
					//	it = _clients.begin();
					//	break;
					}

					//it->buffer.assign(it->buffer.substr(it->buffer.find("\r\n") + 2));	


				}
				//voir server.cpp ligne 305 chez fchouky pour gerer le ctrl +d en plein milieu d'une commande (pas de \r\n).
				//std::string tmp(buff);
				//parseLine(tmp, fd);
			}
			memset(buff, '\0', BUFFER_SIZE);
		}
	}
}