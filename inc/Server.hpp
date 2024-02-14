#pragma once

#include "IRC.hpp"

class Client;
class Channel;
class Message;

//Un message est une classe simple disposant d'un fd et d'une string a lui envoyer

class	Server {

	private:
		fd_set					_readFDs;
		fd_set					_writeFDs;

		int						_port;
		int						_socketFD;
		vecClient				_clients;
		vecChannel				_channels;
		vecMessage				_messages;
		std::string				_name;
		std::string 			_password;
		sockaddr_in				_serverAddr;
		sockaddr_in				_clientAddr;	//Non initialise a la construction,
									//initialisé a l'utilisation de accept pour un nouveau client.

	public:
								Server( std::string& port, std::string& password );
								~Server( void );

		/* setters */
		int						addClient( fd_set& readFDs, fd_set& writeFDs );
		void					delClient( vecClient::iterator toDel );
		void					addChannel( Channel& toJoin, const std::string& key );

		/* getters */
		int						getFD( void ) const;
		int						getMaxFD( void ) const;
		refClient::iterator		getClientByFD( const int fd ) const;
		refClient::iterator		getClientByName( const std::string& user ) const;
		Channel&				getChannel( const std::string& channel ) const;

		/* checkers */
	    bool					isAvailNick( const std::string& nick );
        bool					isValidNick( const std::string& nick );

		/* commands */
		eCommand				findCommand( const std::string const& line );
		void					cmdWho( const std::vector<std::string>& args, int fd );//comme le docteur
		void					cmdJoin( const std::vector<std::string>& args, int fd );
		void					cmdMode( const std::vector<std::string>& args, int fd );
		void					cmdNick( const std::vector<std::string>& args, int fd );
		void					cmdPart( const std::vector<std::string>& args, int fd );
		void					cmdPass( const std::vector<std::string>& args, int fd );
		void					cmdQuit( const std::vector<std::string>& args, int fd );
		void					cmdUser( const std::vector<std::string>& args, int fd );
		void					cmdTopic( const std::vector<std::string>& args, int fd );
		void					cmdNotice( const std::vector<std::string>& args, int fd );
		void					cmdInvite( const std::vector<std::string>& args, int fd );
		void					cmdPrivmsg( const std::vector<std::string>& args, int fd );

		/* Misc */
		void					run( void );
		void					errorCase( int errorCode, int fd );
		void    				parseLine( std::string& line, int fd );

};
