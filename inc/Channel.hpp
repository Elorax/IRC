#pragma once

#include <string>
#include <iostream>

class	Channel {

	private:
		std::string	_name;
		std::string _passWord;
		int			_usersLimited;
		bool		_usersVisible;
		bool		_topicEnabled;
		bool		_inviteOnly;

	public:
					Channel( void );
					Channel( Channel const& src );
					~Channel( void );

					Channel& operator=( Channel const& rhs );

};

/*----------------------------------------------------------------------------*/

#include "Channel.hpp"

Channel::Channel( void ) {

}

Channel::Channel( Channel const& src ) {

	*this = src;
}

Channel::~Channel( void ) {

}

Channel&	Channel::operator=( Channel const& rhs ) {

	return (*this);
}

/*----------------------------------------------------------------------------*/
