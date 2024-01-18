#include "Client.hpp"

Client::Client(int fd)
{
    _clientFD = fd;
    _userName = "";
    _nickName = "";
    _realName = "";
    _hostName = "";
}

Client::~Client()
{
    std::cout << "Client " << _userName << " (" << _nickName << ") deleted" << std:: endl;  //debug
}

/* -------------------------------------------------------------------------- */
/*                              Setter / Getters                              */
/* -------------------------------------------------------------------------- */

void Client::setPassword( std::string const& str ) {

}

void Client::setNickName(std::string const& str )
{
    return (_nickName) ;
}

void Client::setUserName(std::string const& str )
{
    return (_userName) ;
}


/* -------------------------------------------------------------------------- */

const std::string &Client::getHostName() const
{
    return (_hostName) ;
}

const std::string &Client::getRealName() const
{
    return (_realName) ;
}

const std::string &Client::getUserName() const
{
    return (_userName) ;
}

const std::string &Client::getNickName() const
{
    return (_nickName) ;
}

