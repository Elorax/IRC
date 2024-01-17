#pragma once

#include <string>
#include <iostream>

class	Server {

	private:
		std::string	_name;
		std::string _passWord;
		int			_port;
		int			_socketFD;

	public:
					Server( void );
					Server( Server const& src );
					~Server( void );

					Server& operator=( Server const& rhs );

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


