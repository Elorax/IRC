#pragma once

#include "IRC.hpp"

class Channel;

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
		std::string					buffer;

									Client( int fd );
									~Client( void );

		/* setters */
		void						setPassword( const std::string& password );
		void						setNickname( const std::string& nickname );
		void						setUsername( const std::string& username );
		void						setRealname( const std::string& realname );
		void						setHostname( const std::string& hostname );

		void						addChan( const Channel& add );
		void						delChan( const Channel& del );

		/* getters */
		const int&					getFD( void ) const;
		const std::string&			getHostname( void ) const;
		const std::string&			getUsername( void ) const;
		const std::string&			getNickname( void ) const;
		const std::string&			getRealname( void ) const;
		const std::string&			getPassword( void ) const;
		const std::string			getLastChan( void ) const;
		vecChannel&					getUserChanList( void );

		/* checker */
		bool						isChanInList( Channel& chan );
};

