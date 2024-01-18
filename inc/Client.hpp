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
					Client(int fd);
					Client( Client const& src );
					~Client( void );

					Client& operator=( Client const& rhs );

};

/*----------------------------------------------------------------------------*/

#include "Client.hpp"

Client::Client( void ) {

}

Client::Client( Client const& src ) {

	*this = src;
}

Client::~Client( void ) {

}

Client&	Client::operator=( Client const& rhs ) {

	return (*this);
}

/*----------------------------------------------------------------------------*/
