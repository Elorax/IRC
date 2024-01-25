#include "Channel.hpp"

/* -------------------------------------------------------------------------- */
/*                                 Constructor                                */
/* -------------------------------------------------------------------------- */

Channel::Channel( std::string name, Client& client ): _name(name), _password(""),
_chanCapacity(0), _usersVisible(true), _topicEnabled(false), _inviteOnly(false) {

	//getServer().addChannel(*this);	//A faire dans la commande JOIN, pas ici.
	_chanUsers.push_back(client);
	_chanOp.push_back(client);
}

/* -------------------------------------------------------------------------- */
/*                                   Getter                                   */
/* -------------------------------------------------------------------------- */

refClients::iterator Channel::getClientByName( const std::string& name ) const {

    refClients::iterator it = _chanOp.begin();
    for (; it != _chanOp.end(); it++)
        if (name == it->getUserName())
	        return (it);
}

/* -------------------------------------------------------------------------- */
/*                                   Setter                                   */
/* -------------------------------------------------------------------------- */

void	Channel::setPassword( std::string &password ) {

	_password = password;
}

void	Channel::setChanOP( Client& user ){

	_chanOp.push_back(user);
}

void	Channel::unsetChanOP( Client& user ){

	_chanOp.erase(getClientByName(user.getUserName()));
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

//Envoi d'un message a tous les users d'un channel
//Fonction d'envoi de message prive d'un user a un autre a faire directement depuis le serveur car ne passe pas par un channel ?
void	Channel::sendMsg( const std::string &msg ) const
{
	refClients::iterator it = _chanUsers.begin();
	for(; it != _chanUsers.end(); it++)
	{
		it->getFD();
		send(it->getFD(), msg.c_str(), msg.size() + 1, 0);	//+1 pour \0 ?
		//utilisation de IS_SET ? UNSET a la fin de l'envoi ? A verifier, regarder l'envoi de messages sur des githubs.
	}
	//Verifier le retour de send ?
}
