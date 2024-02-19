#include "Channel.hpp"

/* -------------------------------------------------------------------------- */
/*                                 Constructor                                */
/* -------------------------------------------------------------------------- */

Channel::Channel( std::string name, Client& client ): _name(name), _chanKey(""), _topic(""),
_isChanKeySet(false), _inviteOnly(false), _topicPriv(false), _chanLimit(-1) {

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
	return (_chanKey);
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

void	Channel::addUserOnChan( Client& user ) {

	_chanUsers.push_back(user);
	user.addChanToUser(*this);
}

void	Channel::setKey( const std::string& key ) {

	if (isKeyValid(key)) {
		_chanKey = key;
		_isChanKeySet = true;
	}
}

void	Channel::setKey( Server& serv, vecString& args, vecString::iterator it, int fd ) {

	if (++it == args.end())
		serv.buildMsg(ERR_NEEDMOREPARAMS, fd);

	else if (isChanKeySet())
		serv.buildMsg(ERR_KEYSET(*it, getName()), fd);

	else if (isKeyValid(*it)) {
		_chanKey = *it;
		_isChanKeySet = true;
	}

	else //Qu'est-ce qu'on envoie au client qui effectue la requete ??
		serv.buildMsg("Key incorrect\r\n", fd);
}

void	Channel::setOP( Server& serv, vecString& args, vecString::iterator it, int fd ) {

	if (++it == args.end())
		serv.buildMsg(ERR_NEEDMOREPARAMS, fd);

	else if (isUserOnChan(*it))
		serv.buildMsg(ERR_USERNOTINCHANNEL, fd);

	else if (!isUserChanOp(*it))
		_chanOp.push_back(*serv.getClientByName(*it));
}

void	Channel::setLimit( Server& serv, vecString& args, vecString::iterator it, int fd ) {

	if (++it == args.end())
		serv.buildMsg(ERR_NEEDMOREPARAMS, fd);
	else
		_chanLimit = atoi(it->c_str());
}

void	Channel::setTopicPriv( void ) {
	_topicPriv = true;
}

void	Channel::setInviteOnly( void ) {
	_inviteOnly = true;
}

/* -------------------------------------------------------------------------- */
/*                                  Unsetters                                 */
/* -------------------------------------------------------------------------- */

void	Channel::delUserOnChan( Client& user ) {

	std::string name = user.getUsername();
	vecClient::iterator it = _chanUsers.begin();

	for(; it != _chanUsers.end(); it++)
		if (name == it->getUsername()) {
			_chanUsers.erase(it);
			user.delChanOfUser(*this);
		}
}

void	Channel::unsetKey( void ) {

	_chanKey = "";
	_isChanKeySet = false;
}

void	Channel::unsetOP( Server& serv, vecString& args, vecString::iterator it, int fd ) {

	if (++it == args.end())
		serv.buildMsg(ERR_NEEDMOREPARAMS, fd);

	else if (isUserOnChan(*it))
		serv.buildMsg(ERR_USERNOTINCHANNEL, fd);

	else if (isUserChanOp(*it))
		_chanOp.erase(serv.getClientByName(*it));
}

void	Channel::unsetLimit( void ) {
	_chanLimit = -1;
}

void	Channel::unsetTopicPriv( void ) {
	_topicPriv = false;
}

void	Channel::unsetInviteOnly( void ) {
	_topicPriv = false;
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
	return (_chanLimit == _chanUsers.size());
}

bool	Channel::isMatchingKey( const std::string& key ) {
	return (key == _chanKey);
}


bool	Channel::isKeyValid( const std::string& key ) {

	if (key.size() > 23)
		return (false);

	if (!key.empty())
		for (std::string::const_iterator it = key.begin(); it != key.end(); it++)
			if ((*it >= 9 && *it <= 13) || *it == ' ' || *it == ':')
				return (false);
	return (true);
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
