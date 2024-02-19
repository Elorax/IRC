#include "Channel.hpp"

/* -------------------------------------------------------------------------- */
/*                                 Constructor                                */
/* -------------------------------------------------------------------------- */

Channel::Channel( std::string name, Client& client ): _name(name), _password(""), _topic(""),
_isChanKeySet(false), _inviteOnly(false), _topicPriv(false), _chanCapacity(-1) {

	client.addChanToUser(*this);
	_chanOp.push_back(client);
	_chanUsers.push_back(client);
}

Channel::~Channel( void ) {
}

/* -------------------------------------------------------------------------- */
/*                                   Getter                                   */
/* -------------------------------------------------------------------------- */

const Client&	Channel::getChanOp( const std::string& name ) {

	vecClient client;
    vecClient::iterator it = _chanOp.begin();
    for (; it != _chanOp.end(); it++)
        if (name == it->getUsername())
			return (*it);

	throw std::runtime_error("No matching Client found");
}

const vecClient	Channel::getChanUsers( void ) const {
	return (_chanUsers);
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

size_t	Channel::getNbClients( void ) { 

	size_t i = 0;
	vecClient::iterator it = _chanUsers.begin();
	for (; it != _chanUsers.end(); it++)
		i++;

	return (i);
}

/* -------------------------------------------------------------------------- */
/*                                   Setters                                  */
/* -------------------------------------------------------------------------- */

void	Channel::setChanCapacity( const size_t capacity ){
	_chanCapacity = capacity;
}

void	Channel::setInviteOnly( const bool status ) {
	_inviteOnly = status;
}

void	Channel::setPassword(const std::string& password ) {
	_password = password;
}

void	Channel::setChanOP( Client& user ){
	_chanOp.push_back(user);
}

void	Channel::unsetChanOP( const Client& user ) {

	std::string name = user.getUsername();
	vecClient::iterator it = _chanOp.begin();

	for(; it != _chanOp.end(); it++)
		if (name == it->getUsername())
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
	vecClient::iterator it = _chanUsers.begin();

	for(; it != _chanUsers.end(); it++)
		if (name == it->getUsername()) {
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

	vecClient::iterator it = _chanUsers.begin();

	for (; it != _chanUsers.end(); it++)
		if (it->getNickname() == nickname)
			return (true);

	return (false);
}

bool	Channel::isUserChanOp( const std::string& nickname )
{
	vecClient::iterator it = _chanOp.begin();

	for (; it != _chanOp.end(); it++)
		if (it->getNickname() == nickname)
			return (true);

	return (false);
}

bool	Channel::isUserChanOp( int fd ) {

	vecClient::iterator it = _chanOp.begin();

	for (; it != _chanOp.end(); it++)
		if (it->getFD() == fd)
			return (true);

	return (false);
}

bool	Channel::isUserOnChan( int fd ) {

	vecClient::iterator it = _chanUsers.begin();

	for (; it != _chanUsers.end(); it++)
		if (it->getFD() == fd)
			return (true);

	return (false);
}
