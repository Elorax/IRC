#pragma once

#include "IRC.hpp"

class	Client {

	private:
		int							_clientFD;
		std::string					_chanKey;
		std::string		 			_hostname;
		std::string		 			_realname;
		std::string					_username;
		std::string					_nickname;
		vecChannel					_userChannels;

	public:
									Client( int fd );
									Client( const Client& src );
									~Client( void );

		/* setters */
		void						setPassword( const std::string& password );
		void						setNickname( const std::string& nickname );
		void						setUsername( const std::string& username );//set username, realname and hostname: Parameters: <user> <mode> <unused> <realname>
		void						setRealname( const std::string& realname );
		void						setHostname( const std::string& hostname );

		void						addChanToUser( const Channel& add );
		void						delChanOfUser( const Channel& del );

		/* getters */
		const int&					getFD( void ) const;
		const std::string&			getHostname( void ) const;
		const std::string&			getUsername( void ) const;
		const std::string&			getNickname( void ) const;
		const std::string&			getRealname( void ) const;
		const std::string&			getPassword( void ) const;
		vecChannel&					getUserChanList( void );

		/* checker */
		bool						isChanInList( Channel& chan );
};

