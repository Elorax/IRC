#pragma once

#include "IRC.hpp"

class Server;

class	Channel {

	private:
		std::string				_name;
		std::string				_password;
		int						_chanCapacity;
		bool					_usersVisible;
		bool					_topicEnabled;
		bool					_inviteOnly;
		refClients				_chanUsers;
		refClients				_chanOp;

	public:
								Channel( std::string name, Client &client );
								~Channel( void );

		/* Getters */
		refClients::iterator	getChanOp( const std::string& name ) const;

		/* Setters */
		void					setPassword( std::string& password );
		void					setChanOP( Client& user );
		void					unsetChanOP( Client& user );
		void					setChanCapacity( int capacity );
		void					setTopic( bool status );
		void					setInviteOnly( bool status );
		void					addUserOnChan( vecClient::iterator user );

		/* Checkers */
		bool					isInviteOnly( void );
		bool					isUserOnChan( const std::string& nickname );

		/* Misc */
		void					sendMsg( const std::string& msg ) const;

};
