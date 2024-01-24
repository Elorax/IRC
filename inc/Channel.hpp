#pragma once

#include "IRC.hpp"

typedef std::vector<Client&> refClients;

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

		refClients::iterator	getClientByName( const std::string& name ) const;

		void					setPassword( std::string& password );
		void					setChanOP( Client& user );
		void					unsetChanOP( Client& user );
		void					setChanCapacity( int capacity );
		void					setTopic( bool status );
		void					setInviteOnly( bool status );

		void					sendMsg( std::string msg );

};
