#pragma once

#include "IRC.hpp"

class Server;

class	Channel {

	private:
		std::string				_name;
		std::string				_password;
		std::string				_topic;
		bool					_isChanKeySet;
		bool					_inviteOnly;
		bool					_topicPriv;
		size_t					_chanCapacity;
		vecClient				_chanOp;
		vecClient				_chanUsers;

	public:
								Channel( std::string name, Client& client );
								~Channel( void );

		/* Getters */
		size_t					getNbClients( void );
		const Client&			getClient(int idx);
		const Client&			getChanOp( const std::string& name );
		const vecClient			getChanUsers( void ) const ;
		const std::string		getTopic( void ) const;
		const std::string		getPassword( void ) const;
		const std::string		getName( void ) const;


		/* Setters */
		void					setChanOP( Client& user );
		void					unsetChanOP( const Client& user );
		void					setTopicPriv( const bool status );
		void					setInviteOnly( const bool status );
		void					addUserOnChan( Client& user );
		void					delUserOnChan( Client& user );
		void					setChanKeyStatus( const bool status);
		void					setChanCapacity( const size_t capacity );
		void					setTopic( const std::string& status );
		void					setPassword( const std::string& password );

		/* Checkers */
		bool					isFull( void );
		bool					isChanKeySet( void );
		bool					isInviteOnly( void );
		bool					isTopicPrivSet( void );
		bool					isUserOnChan( int fd );
		bool					isUserChanOp( int fd );
		bool					isUserOnChan( const std::string& nickname );
		bool					isUserChanOp( const std::string& nickname );
		bool					isMatchingKey( const std::string& key );
};
