#include "Server.hpp"

/* -------------------------------------------------------------------------- */
/*                                 Constructor                                */
/* -------------------------------------------------------------------------- */

Server::Server(std::string &port, std::string &password)
{
    _port = atoi(port.c_str());
    _chanKey = password;
	_name = "ft_irc";

    _socketFD = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if (_socketFD < 0)
		throw std::runtime_error("Socket creation failed");

	int optval = 1;
    // Definir l'option pour la socket
    if (setsockopt(_socketFD, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int)) == -1)
   		throw std::runtime_error("Socket option configuration failed");

    //Configuration adresse et port pour pouvoir lier la socket a une adresse et un port specifique
    _serverAddr.sin_family = AF_INET;//IPv4
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
}

/* -------------------------------------------------------------------------- */
/*                                   Setters                                  */
/* -------------------------------------------------------------------------- */
extern std::vector<int> my_fds;
int	Server::addClient( fd_set& readFDs, fd_set& writeFDs ) {

	int fd;
	socklen_t clientAddrLen = sizeof(_clientAddr);
    fd = accept(_socketFD, (struct sockaddr*) &_clientAddr, &clientAddrLen);
    if (fd == -1)
        return (-1);

    Client newClient(fd);
    _clients.push_back(newClient);
	_clientsFD.push_back(fd);

    FD_SET(fd, &readFDs);
    FD_SET(fd, &writeFDs);

    return (0);
}

void	Server::delClient( vecClient::iterator& toDel ) {

	vecString quitServ;
	quitServ.push_back("JOIN");
	quitServ.push_back("0");
	cmdJoin(quitServ, toDel->getFD());
	_clients.erase(toDel);
	close(toDel->getFD());
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


const vecChannel::iterator	Server::getChanByIt( const std::string& chan ) {

	vecChannel::iterator it = _channels.begin();

	for (; it != _channels.end(); it++){
		if (it->getName() == chan)
			return (it);
	}
	return (it);
}

Channel&	Server::getChanByRef( const std::string& chan) {

	vecChannel::iterator it = _channels.begin();

	for (; it != _channels.end(); it++){
		if (it->getName() == chan)
			return (*it);
	}
	return (*it);
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

bool	Server::isKeyValid( const std::string& key ) {

	if (key.size() > 23)
		return (false);

	if (!key.empty())
		for (std::string::const_iterator it = key.begin(); it != key.end(); it++)
			if ((*it >= 9 && *it <= 13) || *it == ' ' || *it == ':')
				return (false);
	return (true);
}

bool	Server::isChanValid( const std::string& chanName ) {

	if (chanName[0] != '#')
		return (false);

	for (std::string::const_iterator it = chanName.begin(); it != chanName.end(); it++)
		if (*it == 7 || *it == 13 || *it == 10 || *it == ',' || *it == ':')
			return (false);

	return (true);
}

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

	if (!_messages.empty() && !_clients.empty()){

		std::vector<Message>::iterator it = _messages.begin();
		for (; it != _messages.end(); it++){

			if (FD_ISSET(it->getFD(), &writeFDs)){
				std::cout << it->getMsg();
				send(it->getFD(), it->getMsg().c_str(), it->getMsg().size(), 0);
			}
		}
	}
	_messages.erase(_messages.begin(), _messages.end());
}

int    Server::parseLine(std::string &line, int fd) {

    if (line.find("\r\n") == std::string::npos)
		return (0);

	if (line[0] == ':')
    	line = line.substr(line.find(' '));

    vecString args = buildArgs(line);
	switch (findCommand(args[0])) {
		case eINVITE:	cmdInvite(args, fd);	break;
		case eJOIN: 	cmdJoin(args, fd);		break;
		case eMODE: 	cmdMode(args, fd);		break;
		case eNICK: 	cmdNick(args, fd);		break;
		case eNOTICE: 	cmdNotice(args, fd);	break;
		case ePART: 	cmdPart(args, fd, 1);		break;
		case eKICK: 	cmdKick(args, fd);		break;
		case ePASS:		cmdPass(args, fd);		break;
		case ePRIVMSG: 	cmdPrivmsg(args, fd);	break;
		case eQUIT: 	cmdQuit(args, fd);		return (1);
		case eTOPIC: 	cmdTopic(args, fd);		break;
		case eUSER: 	cmdUser(args, fd);		break;
		case eWHO: 		cmdWho();				break;
		case eCAP:		cmdCap();				break;
		case ePING:		cmdPong(args, fd);		break;

		case eNOTFOUND: buildMsg(ERR_UNKNOWNCOMMAND(getClientByFD(fd)->getNickname(), args[0]), fd);
	}
	return (0);
}

eCommand	Server::findCommand( std::string const& line ) {

	std::string	cmd[] = {	"INVITE", "JOIN", "KICK", "MODE", "NICK", "NOTICE",
							"PART", "PASS", "PRIVMSG", "QUIT", "TOPIC", "USER", "WHO", "CAP", "PING"};

    std::string tmp = line;
	if (tmp.find(' ') != std::string::npos)
    	tmp.erase(tmp.find(' '), tmp.size());

	for (size_t i = 0; i < 15; i++){
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

void	Server::buildMsg( const std::string& msg, Channel &chan, int sender ) {

	for(size_t i = 0; i < chan.getNbClients(); i++) {
		int fd = chan.getClient(i).getFD();
		if (fd != sender)
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
		last = line.substr(line.find(':') + 1);
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
	{
		args.pop_back();
   		args.push_back(last);
	}
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
