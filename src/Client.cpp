#include "Client.hpp"

Client::Client(int fd): _clientFD(fd), _password(""), _username(""),
						_nickname(""), _realname(""), _hostname("") {
}

Client::~Client()
{
    std::cout << "Client " << _username << " (" << _nickname << ") deleted" << std:: endl;  //debug
}

/* -------------------------------------------------------------------------- */
/*                              Setter / Getters                              */
/* -------------------------------------------------------------------------- */

void    Client::setPassword( std::string const& password ) {
    _password = password;
}

void    Client::setNickName(std::string const& nickname ) {
    _nickname = nickname;
}

void    Client::setUserName(std::string const& username ) {
    _username = username;
}

void    Client::addChanToUser( const Channel& add ) {
    _userChannels.push_back(add);
}

void	Client::delChanOfUser( const Channel& del ) {

    std::string name = static_cast<std::string>(del.getName());
	vecChannel::iterator it = _userChannels.begin();

	for(; it != _userChannels.end(); it++)
		if (name == it->getName())
			_userChannels.erase(it);
}

/* -------------------------------------------------------------------------- */

const int&          Client::getFD() const {
    return (_clientFD) ;
}

const std::string&  Client::getHostname() const {
    return (_hostname) ;
}

const std::string&  Client::getRealname() const {
    return (_realname) ;
}

const std::string&  Client::getUsername() const {
    return (_username) ;
}

const std::string&  Client::getNickname() const {
    return (_nickname) ;
}

const std::string&  Client::getPassword( void ) const {
    return (_password);
}

const vecChannel&   Client::getUserChanList( void ) const {
    return (_userChannels);
}


/* -------------------------------------------------------------------------- */
/*                                  Checkers                                  */
/* -------------------------------------------------------------------------- */

bool    Client::isChanInList( Channel& chan ) {

    vecChannel::iterator it = getUserChanList();
    for (; it != getUserChanList.end(); it++)
        if (it->getName() == chan.getName())
            return (true);

    return (false);
}