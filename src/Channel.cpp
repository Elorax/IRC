#include "Channel.hpp"

/* -------------------------------------------------------------------------- */
/*                                 Constructor                                */
/* -------------------------------------------------------------------------- */

Channel::Channel( std::string name, Client& client ): _name(name), _password(""),
_chanCapacity(0), _usersVisible(true), _topicEnabled(false), _inviteOnly(false) {

	_chanUsers.push_back(client);
	_chanOp.push_back(client);
}

Channel::~Channel( void ) {
}

/* -------------------------------------------------------------------------- */
/*                                   Getter                                   */
/* -------------------------------------------------------------------------- */

refClient Channel::getChanOp( const std::string& name ) const {

    refClient::iterator it = _chanOp.begin();
    for (; it != _chanOp.end(); it++)
        if (name == it->getUserName())
	        return (*it);
}

/* -------------------------------------------------------------------------- */
/*                                   Setters                                  */
/* -------------------------------------------------------------------------- */

void	Channel::setPassword( std::string &password ) {

	_password = password;
}

void	Channel::setChanOP( Client& user ){

	_chanOp.push_back(user);
}

void	Channel::unsetChanOP( Client& user ){

	_chanOp.erase(getChanOp(user.getUserName()));
}

void	Channel::setChanCapacity( int capacity ){

	_chanCapacity = capacity;
}

void	Channel::setTopic( bool status ){

	_topicEnabled = status;
}

void	Channel::setInviteOnly( bool status ){

	_inviteOnly == status;
}

void	Channel::addUserOnChan( Client& user ) {

	_chanUsers.push_back(user);
}

void	Channel::delUserOfChan( Client& user ) {

	_chanUsers.erase(user);
}

/* -------------------------------------------------------------------------- */
/*                               Checkers                                     */
/* -------------------------------------------------------------------------- */

bool	Channel::isInviteOnly( void ) {

	return (_inviteOnly);
}

bool	Channel::isFull( void ) {

	return (_chanCapacity == _chanUsers.size());
}

bool	Channel::isMatchingKey( const std::string& key ) {

	return (key == _password);
}

bool	Channel::isUserOnChan( const std::string& nickname ) {

	refClient::iterator it = _chanUsers.begin();

	for (; it != _chanUsers.end(); it++)
		if (it->_name == nickname)
			return (true);

	return (false);
}

/* -------------------------------------------------------------------------- */
/*                                    Misc                                    */
/* -------------------------------------------------------------------------- */

//Envoi d'un message a tous les users d'un channel
//Fonction d'envoi de message prive d'un user a un autre a faire directement depuis le serveur car ne passe pas par un channel ?
void	Channel::sendMsg( const std::string &msg ) const
{
	refClient::iterator it = _chanUsers.begin();
	for(; it != _chanUsers.end(); it++)
	{
		it->getFD();
		send(it->getFD(), msg.c_str(), msg.size() + 1, 0);	//+1 pour \0 ?
		//utilisation de IS_SET ? UNSET a la fin de l'envoi ? A verifier, regarder l'envoi de messages sur des githubs.
	}
	//Verifier le retour de send ?
}
