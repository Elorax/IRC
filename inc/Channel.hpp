#pragma once

#include "IRC.hpp"

class Server;
class Client;

class	Channel {

	private:
		bool					_inviteOnly;
		bool					_isChanKeySet;
		bool					_topicPriv;
		size_t					_chanLimit;
		std::string				_chanKey;
		std::string				_name;
		std::string				_topic;
		vecClient				_chanOp;
		vecClient				_chanUsers;

	public:
								Channel( std::string name, Client& client );
								~Channel( void );

		/* Getters */
		size_t					getNbClients( void );
		const Client&			getClient(int idx);
		const Client&			getOP( const std::string& name );
		const vecClient			getChanUsers( void ) const;
		const std::string		getName( void ) const;
		const std::string		getTopic( void ) const;
		const std::string		getPassword( void ) const;
		const std::string		getNamesOfChanUsers( void ) const;

		/* Setters */
		void					setTopicPriv( void );
		void					setInviteOnly( void );
		void					addUser( Client& user );
		void					setKey( const std::string& key );
		void					setTopic( const std::string& topic );
		void					setOP( Server& serv, vecString& args, vecString::iterator it, int fd );
		void					setKey( Server& serv, vecString& args, vecString::iterator it, int fd );
		void					setLimit( Server& serv, vecString& args, vecString::iterator it, int fd );
		/* Unsetters */
		void					unsetKey( void );
		void					unsetLimit( void );
		void					unsetInviteOnly( void );
		void					unsetTopicPriv( void );
		void					delUser( Client& user );
		void					unsetOP( Client& user );
		void					unsetOP( Server& serv, vecString& args, vecString::iterator it, int fd );

		/* Checkers */
		bool					isFull( void );
		bool					isChanKeySet( void );
		bool					isInviteOnly( void );
		bool					isChanLimitSet( void );
		bool					isTopicPrivSet( void );
		bool					isUserOnChan( int fd );
		bool					isUserChanOp( int fd ) const;
		bool					isUserAlreadyInChan( int fd );
		bool					isKeyValid( const std::string& key );
		bool					isMatchingKey( const std::string& key );
		bool					isUserOnChan( const std::string& nick );
		bool					isUserChanOp( const std::string& nick ) const;
		bool					isUserChanOp( std::string& nickname ) const;

		int						isChanJoinable( const std::string& key, int fd );
};
