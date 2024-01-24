#pragma once

#include "IRC.hpp"

class Client;
class Channel;
class Message;

typedef std::vector<Client>	 vecClient;
typedef std::vector<Channel> vecChannel;
typedef std::vector<Message> vecMessage;

//Un message est une classe simple disposant d'un fd et d'une string a lui envoyer

class	Server {

	private:
		int			_port;
		int			_socketFD;
		vecClient	_clients;
		vecChannel	_channels;
		vecMessage	_messages;
		std::string	_name;
		std::string _password;
		sockaddr_in	_serverAdrr;
		sockaddr_in	_clientAdrr;	//Non initialise a la construction,
									//initialisé a l'utilisation de accept pour un nouveau client.

	public:
					Server( std::string& port, std::string& password );
					~Server( void );

		int			getFD( void );
		int			getMaxFD( void );

		int			addClient( fd_set& readFDs, fd_set& writeFDs );
		void		addChannel( Channel& channel );
};
