#include "Server.hpp"

/* -------------------------------------------------------------------------- */
/*                                 Constructor                                */
/* -------------------------------------------------------------------------- */

Server::Server(std::string &port, std::string &password)
{
    //Initialisation des parametres du serveur
    _port = atoi(port.c_str());
    _password = password;

    //Initialisation des sockets et ouverture du serveur
    //AF_INET : IPv4. SOCK_NONBLOCK pour avoir un socket non bloquant pour le multi client sans fork(). 0 pour protocole automatique
    _socketFD = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);

    if (_socketFD < 0)
    {
        //socket a échoué
        //std::cerr << "Error: socket creation" << std::endl;
        perror(strerror(errno));
        exit(errno);
    }

    int optval = 1;
    // Definir l'option pour la socket
    if (setsockopt(_socketFD, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int)) == -1)
    {
        //std::cerr << "Error: socket option configuration" << std::endl;
        perror(strerror(errno));
        close(_socketFD);
        exit(errno);
    }

    //Configuration adresse et port pour pouvoir lier la socket a une adresse et un port specifique
    _serverAddr.sin_family = AF_INET;    //IPv4
    _serverAddr.sin_addr.s_addr = INADDR_ANY;
    _serverAddr.sin_port = htons(_port);

    //Affectation d'un nom a la socket
    if (bind(_socketFD, (struct sockaddr*) &_serverAddr, sizeof(_serverAddr)) < 0)
    {
        perror(strerror(errno));
        close(_socketFD);
        exit(errno);
    }

    //indique que le serveur est pret a accepter des connexions clients
    if (listen(_socketFD, MAX_CLIENTS) < 0)
    {
        perror(strerror(errno));
        close(_socketFD);
        exit(errno);
    }
}


/* -------------------------------------------------------------------------- */
/*                                   Setters                                  */
/* -------------------------------------------------------------------------- */

int	Server::addClient(fd_set &readFDs, fd_set &writeFDs) {

	int fd;
    fd = accept(_socketFD, (struct sockaddr*) &_clientAddr, (socklen_t *) sizeof(_clientAddr)); //Pas sur du cast. A voir a la compil
    if (fd == -1)
        return (-1);

    Client newClient(fd);
    std::cout << "NEW CLIENT\n";    //Useless mais utile pour mon debug
    _clients.push_back(newClient);    //Vecteur<client> a implementer
    FD_SET(fd, &readFDs);
    FD_SET(fd, &writeFDs);

    return (0);
}

void	Server::delClient( vecClient::iterator toDel ) {

	close(toDel->getFD());
	_clients.erase(toDel);
}

void	Server::addChannel( Channel& newChan, const std::string& key ) {

	if (!key.empty())
		newChan.setPassword(key);
	_channels.push_back(newChan);
}

/* -------------------------------------------------------------------------- */
/*                                   Getters                                  */
/* -------------------------------------------------------------------------- */

int Server::getFD( void ) const {

	return (_socketFD);
}

//Return plus grand des fds entre celui du serveur et celui des clients
int Server::getMaxFD( void ) const {

    int fdMax = _socketFD;
    vecClient::const_iterator it;

    for(it = _clients.begin(); it != _clients.end(); it++)
    {
        if (it->getFD() > fdMax)
            fdMax = it->getFD();
    }

    return (fdMax);
}

vecClient::iterator	Server::getClientByFD( int fd ) {

	vecClient::iterator	it = _clients.begin();
	for (; it != _clients.end(); it++) {
		if (it->getFD() == fd);
			return (it);
	}
}

vecClient::iterator	Server::getClientByName( std::string& user ) {

	vecClient::iterator	it = _clients.begin();
	for (; it != _clients.end(); it++) {
		if (it->getUserName() == user);
			return (it);
	}
}

Channel&	Server::getChannel( const std::string& channel ) const {

	vecChannel::const_iterator it = _channels.begin();

	for (; it != _channels.end(); it++)
		if (it->getName() == channel)
			return (const_cast<Channel &>(*it));
}

/* -------------------------------------------------------------------------- */
/*                                  Checkers                                  */
/* -------------------------------------------------------------------------- */

bool    Server::isAvailNick( const std::string& nick ) {

	vecClient::iterator it = _clients.begin();
	for(; it != _clients.end(); it++)
		if (it->getNickName() == nick)
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
	for(;it != nick.end(); it++)
		if (!isalnum(*it) && special.find(*it) == std::string::npos && *it != '-')
			return (false);
	return (true);
}
//nickname   =  ( letter / special ) *8( letter / digit / special / "-" )
//special    =  %x5B-60 / %x7B-7D
//                   ; "[", "]", "\", "`", "_", "^", "{", "|", "}"

/* -------------------------------------------------------------------------- */
/*                                  Commands                                  */
/* -------------------------------------------------------------------------- */

void	Server::cmdNick( const std::vector<std::string>& args, int fd ) {

	if (args.size() != 1)
		errorCase(ERR_NEEDMOREPARAMS, fd);

	else if (!isAvailNick(args[0]))
		errorCase(ERR_NICKNAMEINUSE, fd);

	else if (!isValidNick(args[0]))
		errorCase(ERR_ERRONEUSNICKNAME, fd);

	else
		getClientByFD(fd)->setNickName(args[0]);
}

void	Server::cmdPass( const std::vector<std::string>& args, int fd ) {

	if(args.empty())
		errorCase(ERR_NEEDMOREPARAMS, fd);

	// if (getClient(fd).getPassword() != "")
	// 	errorCase(ERR_ALREADYREGISTERED, fd);

	else if (getClientByFD(fd)->getPassword() != args[0])
		errorCase(ERR_PASSWDMISMATCH, fd);

	else
		getClientByFD(fd)->setPassword(args[0]);
}

void    Server::cmdUser( const std::vector<std::string>& args, int fd)
{
    if (args.size() < 4)
        errorCase(ERR_NEEDMOREPARAMS, fd);

	else if (args[0] == getClientByFD(fd)->getUserName())
		errorCase(ERR_ALREADYREGISTERED, fd);

	else
		getClientByFD(fd)->setUserName(args[0]);//setrealname, realname can have spaces
}

void	Server::cmdInvite( const std::vector<std::string>& args, int fd ) {


	if (args.size() != 2)
		errorCase(ERR_NEEDMOREPARAMS, fd);

	std::string nickname = args[0];
	Channel& chan = getChannel(args[1]);

	if (!isValidNick(nickname))
		errorCase(ERR_NOSUCHNICK, fd);

	else if (!chan.isUserOnChan(*getClientByFD(fd)))
		errorCase(ERR_NOTONCHANNEL, fd);

	else if (chan.isInviteOnly() && !chan.isUserChanOp(*getClientByFD(fd)))
		errorCase(ERR_CHANOPRIVSNEEDED, fd);

	else if (chan.isUserOnChan(nickname))
		errorCase(ERR_USERONCHANNEL, fd);

	else
		chan.addUserOnChan(*getClientByName(nickname));

}

void	Server::cmdJoin( const std::vector<std::string>& args, int fd ) {

	if (args.size() < 1)
		errorCase(ERR_NEEDMOREPARAMS, fd);

	std::vector<std::string>::const_iterator it = args.begin();
	for (; it != args.end() - 1; it++) {

		Channel& toJoin = getChannel(args[0]);
		std::string	key = *(it + 1);

		if (toJoin.is)
			addChannel(toJoin, key);

		else if (toJoin.isInviteOnly())
			errorCase(ERR_INVITEONLYCHAN, fd);

		else if (toJoin.isFull())
			errorCase(ERR_CHANNELISFULL, fd);

		else if (!toJoin.isMatchingKey(key))
			errorCase(ERR_BADCHANNELKEY, fd);

		else
			toJoin.addUserOnChan(*getClientByFD(fd));
	}
}

void	Server::cmdPart( const std::vector<std::string>& args, int fd ) {

	if (args.size() < 1)
		errorCase(ERR_NEEDMOREPARAMS, fd);

	std::vector<std::string>::const_iterator it = args.begin();
	for (; it != args.end() - 1; it++) {

		Channel& toPart = getChannel(*it);

		if (toPart.isUserOnChan(*getClientByFD(fd)))
			errorCase(ERR_NOTONCHANNEL, fd);

		else
			toPart.delUserOfChan(*getClientByFD(fd));
	}
}

void	Server::cmdTopic( const std::vector<std::string>& args, int fd) {

	if (args.size() < 1)
		errorCase(ERR_NEEDMOREPARAMS, fd);

	Channel& chan = getChannel(args[0]);

	if (!chan.isUserOnChan(*getClientByFD(fd)))
		errorCase(ERR_NOTONCHANNEL, fd);

	else if (!chan.isUserChanOp(*getClientByFD(fd)))
		errorCase(ERR_CHANOPRIVSNEEDED, fd);

	else if (args.size() == 1)
		return (chan.getTopic());

	else if (args.size() == 2 && args[1].empty())
		chan.setTopic("");

	else
		chan.setTopic(args[1]);
}

void	Server::cmdMode( const std::vector<std::string>& modes, int fd ) {

	if (!chan.isUserChanOp(*getClientByFD(fd)))
		errorCase(ERR_CHANOPRIVSNEEDED, fd);

	else if (!chan.isUserOnChan(*getClientByFD(fd)))
		errorCase(ERR_NOTONCHANNEL, fd);

	size_t	i = 0;
	for (; i < modes.size(); i++) {
		std::string tmp = modes[i].substr(0, ' ');
		
	}

}

void	Server::cmdNotice( const std::vector<std::string>& args, int fd ) {


}

void	Server::cmdPrivmsg( const std::vector<std::string>& args, int fd ) {


}

void	Server::cmdQuit( const std::vector<std::string>& args, int fd ) {

	
}

void	Server::cmdWho( const std::vector<std::string>& args, int fd ) {

	
}

/* -------------------------------------------------------------------------- */
/*                                    Misc                                    */
/* -------------------------------------------------------------------------- */

std::vector<std::string> buildArgs( std::string& line) {

	std::vector<std::string> args;
    std::string last = line.substr(line.find(':'));
    std::string tmp = line.substr(0, line.find(':'));

    while (tmp.find(' ') != std::string::npos) {
		args.push_back(tmp.substr(0, tmp.find(' ')));
        tmp = tmp.substr(tmp.find(' ') + 1);
	}
    args.push_back(last);

	return (args);
}


std::vector<std::string> buildModes( std::string& line ) {

	std::vector<std::string> modes;
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

eCommand	Server::findCommand( std::string const& line ) {

	std::string	cmd[] = {	"PASS", "NICK", "USER", "INVITE", "JOIN", "KICK",
							"PART", "TOPIC", "PRIVMSG", "NOTICE", "CAP", "MODE", "WHO", "QUIT" };
    std::string tmp = line;
    tmp.erase(tmp.find(' '), tmp.size());

	for (size_t i = 0; i < 14; i++)
		if (cmd[i] == line)
			return ((eCommand)i);

	return (eNOTFOUND);
}

void    Server::parseLine(std::string &line, int fd) {

    //Si line ne se finit pas par \r\n, return
    if (line.find("\r\n") == std::string::npos)
        return ;

	else if (line[0] == ':')//Prefixe : ignoré dans le contexte de ft_IRC
        line = line.substr(line.find(' '));//":prefixe commande arg1 arg2" --> "commande arg1 arg2"
    std::vector<std::string> args = buildArgs(line);

	switch (findCommand(line))
	{
		// case eCAP:
		case eINVITE:
			cmdInvite(args, fd);//change by getClient(fd)
			break;
		case eJOIN:
			cmdJoin(args, fd);
			break;
		case eMODE:
			cmdMode(args, fd);
			break;
		case eNICK:
			cmdNick(args, fd);
			break;
		case eNOTICE:
			cmdNotice(args, fd);
			break;
		case ePART:
			cmdPart(args, fd);
			break;
		case ePASS:
			cmdPass(args, fd);
			break;
		case ePRIVMSG:
			cmdPrivmsg(args, fd);
			break;
		case eQUIT:
			cmdQuit(args, fd);
			break;
		case eTOPIC:
			cmdTopic(args, fd);
			break;
		case eUSER:
			cmdUser(args, fd);
			break;
		case eWHO:
			cmdWho(args, fd);
			break;
		case eNOTFOUND:
	}
}

void	Server::run( void ) {
	FD_ZERO(&_readFDs);
	FD_ZERO(&_writeFDs);
	FD_SET(getFD(), &_readFDs);

	while (true)	//variable globale mise a zero par une interruption ?
	{
	//Penser a une solution de quitter le programme proprement en cas de ctrl c avec des signaux.
		if (select(getMaxFD() + 1, &_readFDs, &_writeFDs, NULL, NULL) < 0)
		{
			std::cerr << "error ta mere" << std::endl;
			break ;
		}

		if (FD_ISSET(getFD(), &_readFDs)) //maginoire listen te titille le donc tu rentres 
		{
			if (addClient(_readFDs, _writeFDs) != 0)   //Ptete ca marche pas avec une ref tavu
			{
				std::cerr << "accept failed" << std::endl;
				continue;
			}
		}

		int					bytesRead;
		char				buff[BUFFER_SIZE] = {0};
		vecClient::iterator	it = _clients.begin();

		for (; it < _clients.end(); it++) {

			int fd = it->getFD();
			if (FD_ISSET(fd, &_readFDs)) {
				bytesRead = recv(fd, buff, BUFFER_SIZE, 0);

				if (bytesRead < 0) {
					delClient(it);
					FD_CLR(fd, &_readFDs);
					FD_CLR(fd, &_writeFDs);
					it = _clients.begin();
				}

				//if the client exceed the IRC allowed amount of bytes, we don't process it.
				else if (bytesRead <= 512)
					//processClient(buff); //processClient vide pour l'instant. parseLine a la place pour faire des tests.
					parseLine(buff, fd);
				memset(buff, '\0', BUFFER_SIZE);
			}
		}
		// faire une fonction qui envoie tous les messages a envoyer (les messages etant des reponses (erreur ou non ERR/RPL/non defini))
		
		// faire une boucle sur les clients et quand FD_ISSET(fd_client, read) reussi, read ce que tu recois du client puis parser la ligne, si elle finit par \r\n l executer, si tu reconnais une commande la faire sinon rien faire
	}



	//Ajouter un nouveau client (accept, update les fd sets)

	//Verifier si un client deja accepte fait une action
	//handleClient()

}