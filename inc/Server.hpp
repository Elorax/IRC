#pragma once

#include "IRC.hpp"

class Client;
class Channel;
class Message;

//Un message est une classe simple disposant d'un fd et d'une string a lui envoyer

class	Server {

	private:
		int							_port;
		int							_socketFD;
		fd_set						_readFDs;
		fd_set						_writeFDs;
		vecClient					_clients;
		vecChannel					_channels;
		vecMessage					_messages;
		std::string					_name;
		std::string 				_password;
		sockaddr_in					_serverAddr;
		sockaddr_in					_clientAddr;	//Non initialise a la construction,
									//initialisé a l'utilisation de accept pour un nouveau client.

	public:
									Server( std::string& port, std::string& password );
									~Server( void );

		/* Setters */
		int							addClient( fd_set& readFDs, fd_set& writeFDs );
		void						delClient( vecClient::iterator& toDel );
		void						addChannel( Channel& toJoin, const std::string& key );

		/* Getters */
		const int					getFD( void ) const;
		const int					getMaxFD( void ) const;
		const vecClient::iterator	getClientByFD( const int fd );
		const vecClient::iterator	getClientByName( const std::string& user );
		Channel&					getChannel( const std::string& channel );

		/* Runtime */
		void						run( void );
		void    					parseLine( std::string& line, int fd );


		/* Building args */
		void						buildMsg(const std::string& msg, int fd );
		void						buildMsg(const std::string& msg, Channel& chan);
		vecString					buildArgs( std::string& line);
		vecString					buildModes( std::string& line );

		/* Checkers */
	    bool						isKeyValid( std::string& key );
		bool						isChanValid( std::string& chanName );
		bool						isAvailNick( const std::string& nick );
        bool						isValidNick( const std::string& nick );
		bool						doesChanExist( const std::string& chan );
		bool						doesUserExist( const std::string& nickname );

		/* Commands */
		eCommand					findCommand( const std::string const& line );
		void						cmdInvite( vecString& args, int fd );
		void						cmdJoin( vecString& args, int fd );
		void						cmdKick( vecString& args, int fd );
		void						cmdMode( vecString& args, int fd );
		void						cmdNick( vecString& args, int fd );
		void						cmdNotice( vecString& args, int fd );
		void						cmdPart( vecString& args, int fd );
		void						cmdPass( vecString& args, int fd );
		void						cmdPrivmsg( vecString& args, int fd );
		void						cmdQuit( vecString& args, int fd );
		void						cmdTopic( vecString& args, int fd );
		void						cmdUser( vecString& args, int fd );
		void						cmdWho( vecString& args, int fd );

		/* Misc */
		int							createChannel( std::string chanName, std::string key, int fd );
		void						leaveAllChans( Client& client );
		void						whoAll( int requester );
		void						whoClient( const Client& target, int requesterFD );
		void						whoChannel( const Channel& target, int requesterFD );
		std::string					partMsgInit( vecString& arg, int fd);
		vecString					splitParamOnComas( std::string& arg );


};