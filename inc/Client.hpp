#pragma once

#include <string>
#include <iostream>

class	Client {

	private:
		std::string _hostName;
		std::string _realName;
		std::string	_userName;
		std::string	_nickName;
		int			_clientFD;
		bool		_chanOP;

	public:
					Client( int fd );
					Client( Client const& src );
					~Client( void );

		/* setters */
		void		setPassword( std::string const& );
		void		setNickName( std::string const& );
		void		setUserName( std::string const& );//set username, realname and hostname: Parameters: <user> <mode> <unused> <realname>
		//void		setRealName( std::string const& );
		//void		setHostName( std::string const& );

		/* getters */
		const		std::string &getHostName( void ) const;
		const		std::string &getUserName( void ) const;
		const		std::string &getNickName( void ) const;
		const		std::string &getRealName( void ) const;
	
		//♥w♥
					Client& operator=( Client const& rhs );

};

