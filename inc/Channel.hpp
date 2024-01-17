#pragma once

#include <string>
#include <iostream>

/* error codes */
#define ERR_NEEDMOREPARAMS		461		//"<char> : invalid parameter"
#define ERR_KEYSET				467		//"<channel> : Channel key already set"
#define ERR_CHANOPRIVSNEEDED	482		//"<channel> : You're not channel operator"
#define ERR_USERNOTINCHANNEL	441		//"<nick> <channel> : They aren't on that channel"
#define ERR_UNKNOWNMODE			472		//"<char> : unknown channel mode"
#define ERR_NOSUCHCHANNEL		403		//"<channel name> : No such channel"
#define ERR_INVITEONLYCHAN      473 	//"<channel> : Cannot join channel (+i)"
#define ERR_BADCHANNELKEY		475		//"<channel> : Cannot join channel (+k)""
#define ERR_CHANNELISFULL       471     //"<channel> : Cannot join channel (+l)"

class	Channel {

	private:
		std::string	_name;
		std::string _password;
		int			_chanCapacity;
		bool		_usersVisible;
		bool		_topicEnabled;
		bool		_inviteOnly;

	public:
					Channel( void );
					Channel( Channel const& src );
					~Channel( void );

					Channel& operator=( Channel const& rhs );

	/* channel modes */
		void		setPassword( std::string password );
		void		setChanOP( std::string user );
		void		setChanCapacity( int capacity );
		void		setTopic( bool status );
		void		setInviteOnly( bool status );

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
