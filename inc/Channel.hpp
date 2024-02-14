#pragma once

#include "IRC.hpp"

class Server;

class	Channel {

	private:
		std::string				_name;
		std::string				_topic;
		std::string				_password;
		int						_chanCapacity;
		bool					_usersVisible;
		bool					_inviteOnly;
		refClient				_chanUsers;
		refClient				_chanOp;

	public:
								Channel( std::string name, Client& client );
								~Channel( void );

		/* Getters */
		refClient				getChanOp( const std::string& name ) const;
		std::string				getTopic( const std::string& topic ) const;
		std::string				getPassword( void ) const;
		std::string				getName( void ) const;

		/* Setters */
		void					setInviteOnly( bool status );
		void					setChanCapacity( int capacity );
		void					setChanOP( Client& user );
		void					unsetChanOP( const Client& user );
		void					addUserOnChan( Client& user );
		void					delUserOfChan( const Client& user );
		void					setTopic( const std::string& status );
		void					setPassword( const std::string& password );

		/* Checkers */
		bool					isFull( void );
		bool					isInviteOnly( void );
		bool					isMatchingKey( const std::string& key );
		bool					isUserOnChan( const std::string& nickname );
		bool					isUserChanOp(const std::string &nickname);

		/* Misc */
		void					sendMsg( const std::string& msg ) const;

};
