#pragma once

#include "IRC.hpp"

class Server;
class Client;

class	Channel {

	private:
		std::string				_name;
		std::string				_chanKey;
		std::string				_topic;
		bool					_isChanKeySet;
		bool					_inviteOnly;
		bool					_topicPriv;
		size_t					_chanLimit;
		vecClient				_chanOp;
		vecClient				_chanUsers;

	public:
								Channel( std::string name, Client& client );
								~Channel( void );

		/* Getters */
		size_t					getNbClients( void );
		const Client&			getClient(int idx);
		const Client&			getOP( const std::string& name );
		const vecClient			getChanUsers( void ) const ;
		const std::string		getName( void ) const;
		const std::string		getTopic( void ) const;
		const std::string		getPassword( void ) const;
		const std::string		getNamesOfChanUsers( void ) const;

		/* Setters */
		void					addUserOnChan( Client& user );
		void					setInviteOnly( void );
		void					setKey( const std::string& key );
		void					setKey( Server& serv, vecString& args, vecString::iterator it, int fd );
		void					setLimit( Server& serv, vecString& args, vecString::iterator it, int fd );
		void					setOP( Server& serv, vecString& args, vecString::iterator it, int fd );
		void					setTopicPriv( void );
		void					setTopic( const std::string& topic );
		/* Unsetters */
		void					delUserOnChan( Client& user );
		void					unsetInviteOnly( void );
		void					unsetKey( void );
		void					unsetLimit( void );
		void					unsetOP( Server& serv, vecString& args, vecString::iterator it, int fd );
		void					unsetTopicPriv( void );

		/* Checkers */
		bool					isFull( void );
		bool					isChanKeySet( void );
		bool					isInviteOnly( void );
		bool					isTopicPrivSet( void );
		bool					isUserOnChan( int fd );
		bool					isUserChanOp( int fd );
		bool					isKeyValid( const std::string& key );
		bool					isUserOnChan( const std::string& nickname );
		bool					isUserChanOp( const std::string& nickname );
		bool					isMatchingKey( const std::string& key );
};
