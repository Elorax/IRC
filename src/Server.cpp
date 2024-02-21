#include "Server.hpp"

/* -------------------------------------------------------------------------- */
/*                                 Constructor                                */
/* -------------------------------------------------------------------------- */

Server::Server(std::string &port, std::string &password)
{
    //Initialisation des parametres du serveur
    _port = atoi(port.c_str());
    _chanKey = password;

    //Initialisation des sockets et ouverture du serveur
    //AF_INET : IPv4. SOCK_NONBLOCK pour avoir un socket non bloquant pour le multi client sans fork(). 0 pour protocole automatique
    _socketFD = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);

    if (_socketFD < 0)
		throw std::runtime_error("Socket creation failed");
    
	int optval = 1;
    // Definir l'option pour la socket
    if (setsockopt(_socketFD, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int)) == -1)
   		throw std::runtime_error("Socket option configuration failed");

    //Configuration adresse et port pour pouvoir lier la socket a une adresse et un port specifique
    _serverAddr.sin_family = AF_INET;    //IPv4
    _serverAddr.sin_addr.s_addr = INADDR_ANY;
    _serverAddr.sin_port = htons(_port);

    //Affectation d'un nom a la socket
    if (bind(_socketFD, (struct sockaddr*) &_serverAddr, sizeof(_serverAddr)) < 0)
   		throw std::runtime_error("Socket binding failed");

    //indique que le serveur est pret a accepter des connexions clients
    if (listen(_socketFD, MAX_CLIENTS) < 0)
    	throw std::runtime_error("Socket listening failed");
}

Server::~Server( void ) {
	//truc dedans
}

/* -------------------------------------------------------------------------- */
/*                                   Setters                                  */
/* -------------------------------------------------------------------------- */

int	Server::addClient( fd_set& readFDs, fd_set& writeFDs ) {

	int fd;
	std::cout << "tentative d'accept" << std::endl;
	socklen_t clientAddrLen = sizeof(_clientAddr);
    fd = accept(_socketFD, (struct sockaddr*) &_clientAddr, &clientAddrLen);
	std::cout << "prout" << fd << std::endl;
    if (fd == -1)
        return (-1);

    Client newClient(fd);
    std::cout << "NEW CLIENT\n";    //Useless mais utile pour mon debug
    _clients.push_back(newClient);
    FD_SET(fd, &readFDs);
    FD_SET(fd, &writeFDs);

    return (0);
}

void	Server::delClient( vecClient::iterator& toDel ) {

	close(toDel->getFD());
	_clients.erase(toDel);
}

void	Server::addChannel( Channel& newChan, const std::string& key ) {

	if (!key.empty())
		newChan.setKey(key);
	_channels.push_back(newChan);
}

/* -------------------------------------------------------------------------- */
/*                                   Getters                                  */
/* -------------------------------------------------------------------------- */

int Server::getFD( void ) const {
	return (_socketFD);
}

int Server::getMaxFD( void ) const {

    int fdMax = _socketFD;
    vecClient::const_iterator it;
    for(it = _clients.begin(); it != _clients.end(); it++) {
        if (it->getFD() > fdMax)
            fdMax = it->getFD();
    }

    return (fdMax);
}

const vecClient::iterator	Server::getClientByFD( int fd ) {

	vecClient::iterator	it = _clients.begin();
	for (; it != _clients.end(); it++){
		if (it->getFD() == fd)
			return (it);
	}
	return (it);
}

const vecClient::iterator	Server::getClientByName( const std::string& nick ) {

	vecClient::iterator	it = _clients.begin();
	for (; it != _clients.end(); it++){
		if (it->getNickname() == nick)
			return (it);
	}
	return (it);
}

Channel&	Server::getChannel( const std::string& channel ) {

	vecChannel::iterator it = _channels.begin();

	for (; it != _channels.end(); it++){
		if (it->getName() == channel)
			return (*it);
	}
	return (*it);	//Si on arrive ici, on a renvoyé end()
}

/* -------------------------------------------------------------------------- */
/*                                  Checkers                                  */
/* -------------------------------------------------------------------------- */

bool    Server::isAvailNick( const std::string& nick ) {

	vecClient::iterator it = _clients.begin();
	for(; it != _clients.end(); it++){
		if (it->getNickname() == nick)
			return (false);
	}
	return (true);
}

bool	Server::isUserSet( const Client& client ) {

	if (client.getPassword().empty() || client.getUsername().empty()
		|| client.getNickname().empty() || client.getHostname().empty() || client.getRealname().empty())
		return (false);
	return (true);
}

bool    Server::isValidNick( const std::string& nick ) {

	std::string special = "[]\\_^{}|`";

	if (nick.length() > 9)
		return (false);

	else if (special.find(nick[0]) == std::string::npos && !isalpha(nick[0]))
		return (false);

	std::string::const_iterator it = nick.begin() + 1;
	for(;it != nick.end(); it++){
		if (!isalnum(*it) && special.find(*it) == std::string::npos && *it != '-')
			return (false);
	}
	return (true);
}
//nickname   =  ( letter / special ) *8( letter / digit / special / "-" )
//special    =  %x5B-60 / %x7B-7D
//                   ; "[", "]", "\", "`", "_", "^", "{", "|", "}"

bool	Server::doesChanExist( const std::string& chan ) {

	vecChannel::iterator it = _channels.begin();

	for (; it != _channels.end(); it++){
		if (it->getName() == chan)
			return (true);
	}
	return (false);
}

bool	Server::doesUserExist( const std::string& nickname ) {

	vecClient::iterator it = _clients.begin();

	for (; it != _clients.end(); it++){
		if (it->getNickname() == nickname)
			return (true);
	}
	return (false);
}

/* -------------------------------------------------------------------------- */
/*                                    Misc                                    */
/* -------------------------------------------------------------------------- */

void	Server::sendMsgs(fd_set writeFDs){

	std::cout << "ON SEND TOUS LES MESSAGES C LE FACTEUR QUI REGALE" << std::endl;
	if (!_messages.empty() && !_clients.empty()){

		std::vector<Message>::iterator it = _messages.begin();
		for (; it != _messages.end(); it++){

			if (FD_ISSET(it->getFD(), &writeFDs)){
				std::cout << "Sent message to :" << getClientByFD(it->getFD())->getNickname() << " :" << it->getMsg() << std::endl;
				send(it->getFD(), it->getMsg().c_str(), it->getMsg().size(), 0);
				// std::cout << "Envoi du msg" + it->getMsg() + "au fd " << it->getFD() << std::endl;;	//Debugging
			}
		}
	}
	_messages.erase(_messages.begin(), _messages.end());	//Clear de nos messages.
}

void    Server::parseLine(std::string &line, int fd) {

	std::cout << "Ligne recue : >" << line << "<" << std::endl;
    if (line.find("\r\n") == std::string::npos)
		return;

	if (line[0] == ':')
    	line = line.substr(line.find(' '));

    vecString args = buildArgs(line);

	for(vecString::iterator it = args.begin(); it != args.end(); it++)
	{
		std::cout << "arg : >" << *it << "<" << std::endl;
	}

	switch (findCommand(args[0])) {	//!mettre args[0]

		case eINVITE:	cmdInvite(args, fd);	break;
		case eJOIN: 	cmdJoin(args, fd);		break;
		case eMODE: 	cmdMode(args, fd);		break;
		case eNICK: 	cmdNick(args, fd);		break;
		case eNOTICE: 	cmdNotice(args, fd);	break;
		case ePART: 	cmdPart(args, fd);		break;
		case eKICK: 	cmdKick(args, fd);		break;
		case ePASS:		cmdPass(args, fd);		break;
		case ePRIVMSG: 	cmdPrivmsg(args, fd);	break;
		case eQUIT: 	cmdQuit(args, fd);		break;
		case eTOPIC: 	cmdTopic(args, fd);		break;
		case eUSER: 	cmdUser(args, fd);		break;
		case eWHO: 		cmdWho(args, fd);		break;
		case eNOTFOUND: buildMsg(ERR_UNKNOWNCOMMAND(args[0]), fd);
	}
}

eCommand	Server::findCommand( std::string const& line ) {

	std::string	cmd[] = {	"INVITE", "JOIN", "KICK", "MODE", "NICK", "NOTICE",
							"PART", "PASS", "PRIVMSG", "QUIT", "TOPIC", "USER", "WHO"};

    std::string tmp = line;
	if (tmp.find(' ') != std::string::npos)
    	tmp.erase(tmp.find(' '), tmp.size());

	for (size_t i = 0; i < 13; i++){
		if (cmd[i] == line)
			return ((eCommand)i);
	}
	return (eNOTFOUND);
}

/* -------------------------------------------------------------------------- */
/*                                Building Args                               */
/* -------------------------------------------------------------------------- */

void	Server::buildMsg( const std::string& msg, Channel &chan ) {

	for(size_t i = 0; i < chan.getNbClients(); i++) {
		int fd = chan.getClient(i).getFD();
		_messages.push_back(Message(msg, fd));
	}
}

void	Server::buildMsg( const std::string& msg, int fd ) {
	_messages.push_back(Message(msg, fd));
}

vecString Server::buildArgs( std::string& line ) {

	vecString args;
	std::string last;

	if (line.find(':') != std::string::npos)
		last = line.substr(line.find(':'));
	else
		last = "";

    std::string tmp = line.substr(0, line.find(':'));
    while (tmp.find(' ') != std::string::npos) {
		args.push_back(tmp.substr(0, tmp.find(' ')));
        tmp = tmp.substr(tmp.find(' ') + 1);
	}
	if (tmp.find("\r\n") != std::string::npos)
		tmp.erase(tmp.find("\r\n"));
	args.push_back(tmp);
	if (!last.empty())
   		args.push_back(last);
	return (args);
}

vecString Server::buildModes( std::string& line ) {

	vecString modes;
	std::string chan= line.substr(0, ' ');
	std::string tmp = line.substr(chan.size() + 1, line.find_first_of("+-"));

	modes.push_back(chan);
    while (tmp.find_first_not_of("+-") != std::string::npos) {
		modes.push_back(tmp.substr(0, tmp.find_first_of("+-")));
		tmp = tmp.substr(tmp.find_first_of("+-") + 1);
	}
    modes.push_back(tmp);

	return (modes);
}