#include "Channel.hpp"

/* -------------------------------------------------------------------------- */
/*                                 Constructor                                */
/* -------------------------------------------------------------------------- */

Channel::Channel( std::string name, Client& client ): _name(name), _password(""), _topic(""),
_topicPriv(false), _chanCapacity(0), _usersVisible(true), _inviteOnly(false) {

	client.addChanToUser(*this);
	_chanUsers.push_back(client);
	_chanOp.push_back(client);
	_isChanKeySet = false;
}

Channel::~Channel( void ) {
}

/* -------------------------------------------------------------------------- */
/*                                   Getter                                   */
/* -------------------------------------------------------------------------- */

const refClient	Channel::getChanOp( const std::string& name ) const {

    refClient::iterator it = _chanOp.begin();

    for (; it != _chanOp.end(); it++)
        if (name == it->getUserName())
	        return (*it);
}

const std::string	Channel::getPassword( void ) const{
	return (_password);
}

const std::string	Channel::getName( void ) const{
	return (_name);
}

const std::string	Channel::getTopic( void ) const {
	return (_topic);
}

const Client&	Channel::getClient(int idx) {
	return (_chanUsers[idx]);
}

const int	Channel::getNbClients( void ) const { 

	int i = 0;
	refClient::iterator it = _chanUsers.begin();
	for (; it != _chanUsers.end(); it++)
		i++;
	return (i);
}

/* -------------------------------------------------------------------------- */
/*                                   Setters                                  */
/* -------------------------------------------------------------------------- */

void	Channel::setChanCapacity( const int capacity ){
	_chanCapacity = capacity;
}

void	Channel::setInviteOnly( const bool status ) {
	_inviteOnly == status;
}

void	Channel::setPassword(const std::string& password ) {
	_password = password;
}

void	Channel::setChanOP( Client& user ){
	_chanOp.push_back(user);
}

void	Channel::unsetChanOP( const Client& user ) {

	std::string name = user.getUsername();
	refClient::iterator it = _chanOp.begin();

	for(; it != _chanOp.end(); it++)
		if (name == it->getUserName())
			_chanOp.erase(it);
}

void	Channel::setTopicPriv( const bool status ) {
	_topicPriv = status;
}


void	Channel::setTopic( const std::string& topic ){
	_topic = topic;
}

void	Channel::addUserOnChan( Client& user ) {

	_chanUsers.push_back(user);
	user.addChanToUser(*this);
}

void	Channel::delUserOnChan( Client& user ) {

	std::string name = user.getUsername();
	refClient::iterator it = _chanUsers.begin();

	for(; it != _chanUsers.end(); it++)
		if (name == it->getUserName()) {
			_chanUsers.erase(it);
			user.delChanOfUser(*this);
		}
}

void	Channel::setChanKeyStatus(bool status) {
	_isChanKeySet = status;
}


/* -------------------------------------------------------------------------- */
/*                               Checkers                                     */
/* -------------------------------------------------------------------------- */

bool	Channel::isInviteOnly( void ) {
	return (_inviteOnly);
}

bool	Channel::isChanKeySet( void ) {
	return (_isChanKeySet);
}

bool	Channel::isTopicPrivSet( void ) {
	return (_topicPriv);
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

bool	Channel::isUserChanOp( const std::string& nickname )
{
	refClient::iterator it = _chanOp.begin();

	for (; it != _chanOp.end(); it++)
		if (it->_name == nickname)
			return (true);

	return (false);
}

bool	Channel::isUserChanOp( int fd ) {

	refClient::iterator it = _chanOp.begin();

	for (; it != _chanOp.end(); it++)
		if (it->_clientFD == fd)
			return (true);

	return (false);
}

bool	Channel::isUserOnChan( int fd ) {

	refClient::iterator it = _chanUsers.begin();

	for (; it != _chanUsers.end(); it++)
		if (it->_clientFD == fd)
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
