#pragma once

#include "IRC.hpp"

class Client;
class Channel;
class Message;

class	Server {

	private:
		int							_port;
		int							_socketFD;
		std::vector<int>			_clientsFD;
		vecClient					_clients;
		vecChannel					_channels;
		vecMessage					_messages;
		std::string					_name;
		std::string 				_chanKey;
		sockaddr_in					_serverAddr;
		sockaddr_in					_clientAddr;

	public:
									Server( std::string& port, std::string& key );
									~Server( void );

		/* Setters */
		int							addClient( fd_set& readFDs, fd_set& writeFDs );
		void						delClient( vecClient::iterator& toDel );
		void						addChannel( Channel& toJoin, const std::string& key );

		/* Getters */
		int							getFD( void ) const;
		int							getMaxFD( void ) const;
		const vecClient::iterator	getClientByFD( const int fd );
		const vecClient::iterator	getClientByName( const std::string& nick );
		const vecChannel::iterator	getChanByIt( const std::string& chan );
		Channel&					getChanByRef( const std::string& chan );

		/* Runtime */
		void						run( void );
		void						sendMsgs( fd_set writeFDs );
		void 						initBuffer( fd_set &readFDs, fd_set &writeFDs );
		fd_set						initReadFDs( void );
		fd_set						initWriteFDs( void );
		std::string 				initMsgs( Client& user, vecString& args, std::string& target );

		/* Building args */
		vecString					buildArgs( std::string& line);
		vecString					buildModes( std::string& line );
		void						buildMsg(const std::string& msg, int fd );
		void						buildMsg(const std::string& msg, Channel& chan);
		void						buildMsg( const std::string& msg, Channel &chan, int sender );

		/* Checkers */
		bool						isUserSet( const Client& client );
		bool						isKeyValid( const std::string& key );
		bool						isChanValid( const std::string& name );
		bool						isAvailNick( const std::string& nick );
        bool						isValidNick( const std::string& nick );
		bool						doesChanExist( const std::string& chan );
		bool						doesUserExist( const std::string& nick );

		/* Commands */
		eCommand					findCommand( const std::string& line );
		void						cmdCap( void );
		void						cmdWho( void );
		void						cmdInvite( vecString& args, int fd );
		void						cmdJoin( vecString& args, int fd );
		void						cmdKick( vecString& args, int fd );
		void						cmdMode( vecString& args, int fd );
		void						cmdNick( vecString& args, int fd );
		void						cmdNotice( vecString& args, int fd );
		void						cmdPart( vecString& args, int fd, int send_msg );
		void						cmdPass( vecString& args, int fd );
		void						cmdPong( vecString& args, int fd ); 
		void						cmdPrivmsg( vecString& args, int fd );
		void						cmdQuit( vecString& args, int fd );
		void						cmdTopic( vecString& args, int fd );
		void						cmdUser( vecString& args, int fd );

		/* Misc */
		int	    					parseLine( std::string& line, int fd );
		int							createChannel( std::string chan, std::string key, int fd );
		vecString					splitParamOnComas( std::string& arg );
		void						partAllChans( int fd );
		void						handleJoinMsg( Channel& chan, Client& client );
		void						handleModeListMsg( std::string chan, int fd );
		void						handleJoinMsg( std::string chan, Client& client );
		void						kickChans( vecString args, vecString users, vecString chans, int fd );
		void						kickUsers( vecString args, vecString users, vecString chans, int fd );
		void						kickChansUsers( vecString args, vecString users, vecString chans, int fd );
};

