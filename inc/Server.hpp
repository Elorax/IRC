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
		fd_set		_readFDs;
		fd_set		_writeFDs;

		int			_port;
		int			_socketFD;
		vecClient	_clients;
		vecChannel	_channels;
		vecMessage	_messages;
		std::string	_name;
		std::string _password;
		sockaddr_in	_serverAddr;
		sockaddr_in	_clientAddr;	//Non initialise a la construction,
									//initialisé a l'utilisation de accept pour un nouveau client.

	public:
							Server( std::string& port, std::string& password );
							~Server( void );

		int					getFD( void );
		int					getMaxFD( void );
		vecClient::iterator	getClientByFD( int fd );
		vecClient::iterator	getClientByName( std::string& user );

	    bool				isAvailNick( const std::string& nick );
        bool				isValidNick( const std::string& nick );
		int					addClient( fd_set& readFDs, fd_set& writeFDs );
		void				delClient( vecClient::iterator toDel );
		void				addChannel( Channel& channel );
		void				run( void );
		void				errorCase( int errorCode, int fd );
		void    			parseLine( std::string line, int fd );

		eCommand			findCommand( std::string const& line );
		void				cmdWho( std::vector<std::string>& args, int fd );
		void				cmdJoin( std::vector<std::string>& args, int fd );
		void				cmdMode( std::vector<std::string>& args, int fd );
		void				cmdNick( std::vector<std::string>& args, int fd );
		void				cmdPart( std::vector<std::string>& args, int fd );
		void				cmdPass( std::vector<std::string>& args, int fd );
		void				cmdQuit( std::vector<std::string>& args, int fd );
		void				cmdUser( std::vector<std::string>& args, int fd );
		void				cmdTopic( std::vector<std::string>& args, int fd );
		void				cmdNotice( std::vector<std::string>& args, int fd );
		void				cmdInvite( std::vector<std::string>& args, int fd );
		void				cmdPrivmsg( std::vector<std::string>& args, int fd );
};
