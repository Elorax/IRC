#include "Channel.hpp"

/* -------------------------------------------------------------------------- */
/*                                 Constructor                                */
/* -------------------------------------------------------------------------- */

Channel::Channel( std::string name, Client& client )
: _inviteOnly(false), _isChanKeySet(false), _topicPriv(false), _chanLimit(-1), _chanKey(""),  _name(name), _topic("") {

	client.addChan(*this);
	_chanOp.push_back(client);
	_chanUsers.push_back(client);
	// std::cout << "DEBUG: channel cree : " + _name + " --- Createur : " + _chanOp[0].getNickname() << std::endl;
}

Channel::~Channel( void ) {
	// std::cout << "DEBUG: channel DELETE : " + _name << std::endl;
}

/* -------------------------------------------------------------------------- */
/*                                   Getter                                   */
/* -------------------------------------------------------------------------- */

size_t	Channel::getChanLimit( void )
{
	return (_chanLimit);
}


const Client&	Channel::getOP( const std::string& name ) {

	vecClient client;
    vecClient::iterator it = _chanOp.begin();
    for (; it != _chanOp.end(); it++) {
        if (name == it->getNickname())
			return (*it);
	}
	return (*it);
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

const std::string	Channel::getNamesOfChanUsers( void ) const {

	std::string names;
	vecClient::const_iterator it = _chanUsers.begin();
	for (; it != _chanUsers.end(); it++) {
		if (isUserChanOp(it->getNickname()))
			names += "@";
		names += it->getNickname();
		if (it != _chanUsers.end() -1)
			names += " ";
	}
	return (names);
	//! J'ai fini rapidement la fonction mais je sais pas ce que tu voulais en faire donc a toi de veirfier
}

const Client&	Channel::getClient(int idx) {
	return (_chanUsers[idx]);
}

size_t	Channel::getNbClients( void ) { 

	size_t i = 0;
	vecClient::iterator it = _chanUsers.begin();
	for (; it != _chanUsers.end(); it++) {
		std::cout << "CASE NO CLIENT :" <<  it->getNickname() << " \r\n" << std::endl;
		i++;
	}

	return (i);
}

/* -------------------------------------------------------------------------- */
/*                                   Setters                                  */
/* -------------------------------------------------------------------------- */

void	Channel::addUser( Client& user ) {

	_chanUsers.push_back(user);
	user.addChan(*this);
}

void	Channel::setKey( const std::string& key ) {

	if (isKeyValid(key)) {
		_chanKey = key;
		_isChanKeySet = true;
	}
}
// MODE #chan1 +k
int	Channel::setKey( Server& serv, vecString& args, vecString::iterator it, int fd ) {

	if (it == args.end())
		serv.buildMsg(ERR_NEEDMOREPARAMS(args[0]), fd);

	else if (isChanKeySet())
		serv.buildMsg(ERR_KEYSET(getName()), fd);

	else if (isKeyValid(*it)) {
		_chanKey = *it;
		_isChanKeySet = true;
		return (EXIT_SUCCESS);
	}

	else //Qu'est-ce qu'on envoie au client qui effectue la requete ??
		serv.buildMsg(":ft_irc 467" + getName() + ":Channel key incorrect\r\n", fd);
	return (EXIT_FAILURE);
}

int	Channel::setOP( Server& serv, vecString& args, vecString::iterator it, int fd ) {

	if (it == args.end())
		serv.buildMsg(ERR_NEEDMOREPARAMS(args[0]), fd);

	else if (!isUserOnChan(*it))
		serv.buildMsg(ERR_USERNOTINCHANNEL(*it, getName()), fd);

	else if (!isUserChanOp(*it)){
		_chanOp.push_back(*serv.getClientByName(*it));
		return (EXIT_SUCCESS);
	}
	return (EXIT_FAILURE);
}

int	Channel::setLimit( Server& serv, vecString& args, vecString::iterator it, int fd ) {

	if (it == args.end()){
		serv.buildMsg(ERR_NEEDMOREPARAMS(args[0]), fd);
		return (EXIT_FAILURE);
	}
	else
		_chanLimit = atoi(it->c_str());	//!VERIFIER QUE it->c_str EST DU BON FORMAT, C'est a dire UNIQUEMENT COMPOSE DE CHIFFRES.
	return (EXIT_SUCCESS);
}

void	Channel::setTopicPriv( void ) {
	_topicPriv = true;
}

void	Channel::setInviteOnly( void ) {
	_inviteOnly = true;
}

void	Channel::setTopic( const std::string& topic ) {
	_topic = topic;
}

/* -------------------------------------------------------------------------- */
/*                                  Unsetters                                 */
/* -------------------------------------------------------------------------- */

//remove un user d'un chan ainsi que de la liste des op si il en faisait partie.
void	Channel::delUser( Client& user ) {

	std::string name = user.getNickname();
	vecClient::iterator it = _chanUsers.begin();

	for(; it < _chanUsers.end(); it++)
		if (name == it->getNickname()) {
			unsetOP(user);
			_chanUsers.erase(it);
			user.delChan(*this);
			break ;
		}
}

void	Channel::unsetKey( void ) {

	_chanKey = "";
	_isChanKeySet = false;
}

void	Channel::unsetOP( Client& user ) {

	std::string name = user.getNickname();
	vecClient::iterator it = _chanOp.begin();

	for(; it < _chanOp.end(); it++)
		if (name == it->getNickname())
			_chanOp.erase(it);
}


int	Channel::unsetOP( Server& serv, vecString& args, vecString::iterator it, int fd ) {

	if (it == args.end())
		serv.buildMsg(ERR_NEEDMOREPARAMS(args[0]), fd);

	else if (!isUserOnChan(*it))
		serv.buildMsg(ERR_USERNOTINCHANNEL(*it, getName()), fd);

	else if (isUserChanOp(*it))
	{
		unsetOP(*serv.getClientByName(*it));
		return (EXIT_SUCCESS);
	}
	return (EXIT_FAILURE);
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

bool	Channel::isChanLimitSet( void ) {

	if (_chanLimit == __SIZE_MAX__ )
		return (false);
	return (true);
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

bool	Channel::isUserChanOp( const std::string& nickname ) const
{
	vecClient::const_iterator it = _chanOp.begin();

	for (; it != _chanOp.end(); it++)
		if (it->getNickname() == nickname)
			return (true);

	return (false);
}

bool	Channel::isUserChanOp( std::string& nickname ) const
{
	vecClient::const_iterator it = _chanOp.begin();

	for (; it != _chanOp.end(); it++)
		if (it->getNickname() == nickname)
			return (true);

	return (false);
}

bool	Channel::isUserChanOp( int fd ) const {

	vecClient::const_iterator it = _chanOp.begin();

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

bool	Channel::isUserAlreadyInChan( int fd ) {

	vecClient::iterator it = _chanUsers.begin();
	for(; it != _chanUsers.end(); it++)
	{
		if (it->getFD() == fd)
			return (true);
	}
	return (false);
}

int	Channel::isChanJoinable( const std::string& key, int fd ) {

	if (isInviteOnly())
		return (473);
	if (isFull())
		return (471);
	if (!isMatchingKey(key))
		return (475);
	if (isUserAlreadyInChan(fd))
		return (666);

	return (0);
}