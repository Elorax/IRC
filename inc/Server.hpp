#pragma once

#include <string>
#include <iostream>
#include <vector>
#include <netinet/ip.h>
#include "Client.hpp"
#include "Message.hpp"

class Client;
class Message;

typedef std::vector<Client> vecClient;
typedef std::vector<Message> vecMessage;
//Un message est une classe simple disposant d'un fd et d'une string a lui envoyer
class	Server {

	private:
		vecClient	_clients;
		vecMessage	_messages;
		std::string	_name;
		std::string _password;
		int			_port;
		int			_socketFD;
		sockaddr_in	_serverAdrr;
		sockaddr_in	_clientAdrr;	//Non initialise a la construction,
									//initialisé a l'utilisation de accept pour un nouveau client.
		



	public:
					Server( void );
					Server( Server const& src );
					~Server( void );

					Server& operator=( Server const& rhs );
					int	getFD();
					int	getMaxFD();
					int	addClient(fd_set &readFDs, fd_set &writeFDs);

};

/*----------------------------------------------------------------------------*/

#include "Server.hpp"

Server::Server( void ) {

}

Server::Server( Server const& src ) {

	*this = src;
}

Server::~Server( void ) {

}

Server&	Server::operator=( Server const& rhs ) {

	return (*this);
}

/*----------------------------------------------------------------------------*/

