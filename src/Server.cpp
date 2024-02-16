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
	for (; it != _clients.end(); it++)
		if (it->getFD() == fd);
			return (it);

	return (it);
}

vecClient::iterator	Server::getClientByName( const std::string& user ) {

	vecClient::iterator	it = _clients.begin();
	for (; it != _clients.end(); it++)
		if (it->getUserName() == user);
			return (it);

	return (it);
}

Channel&	Server::getChannel( const std::string& channel ) {

	vecChannel::iterator it = _channels.begin();

	for (; it != _channels.end(); it++)
		if (it->getName() == channel)
			return (*it);
	return (*it);	//Si on arrive ici, on a renvoyé end()
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

bool	Server::doesChanExist( const std::string& chan ) {

	vecChannel::iterator it = _channels.begin();

	for (; it != _channels.end(); it++)
		if (it->getName() == chan)
			return (true);

	return (false);
}

bool	Server::doesUserExist( const std::string& nickname ) {

	vecClient::iterator it = _clients.begin();

	for (; it != _clients.end(); it++)
		if (it->getNickName() == nickname)
			return (true);

	return (false);
}

/* -------------------------------------------------------------------------- */
/*                                  Commands                                  */
/* -------------------------------------------------------------------------- */

void	Server::cmdNick( std::vector<std::string>& args, int fd ) {

	if (args.size() != 1)
		buildMsg(ERR_NEEDMOREPARAMS, fd);

	else if (args[0].empty())
		buildMsg(ERR_NONICKNAMEGIVEN, fd);

	else if (!isAvailNick(args[0]))
		buildMsg(ERR_NICKNAMEINUSE, fd);

	else if (!isValidNick(args[0]))
		buildMsg(ERR_ERRONEUSNICKNAME, fd);
	
	else
		getClientByFD(fd)->setNickName(args[0]);
}

void	Server::cmdPass( std::vector<std::string>& args, int fd ) {

	if(args.empty())
		buildMsg(ERR_NEEDMOREPARAMS, fd);

	else if (!getClientByFD(fd)->getPassword().empty())
		buildMsg(ERR_ALREADYREGISTRED, fd);

	else if (getClientByFD(fd)->getPassword() != args[0])
		buildMsg(ERR_PASSWDMISMATCH, fd);

	else
		getClientByFD(fd)->setPassword(args[0]);
}

void    Server::cmdUser( std::vector<std::string>& args, int fd)
{
    if (args.size() < 4)
        buildMsg(ERR_NEEDMOREPARAMS, fd);

	else if (!getClientByFD(fd)->getUserName().empty())
		buildMsg(ERR_ALREADYREGISTRED, fd);

	else
		getClientByFD(fd)->setUserName(args[0]);//setrealname, realname can have spaces
}


//Parametres : INVITE <nickname> <channel>
//Invite un utilisateur a rejoindre un channel, il recoit une notif qui lui dit que qqn veut qu'il rejoigne tel chan
//Il faut etre membre du chan pour inviter quelqu'un sinon la commande echoue
//L'utilisateur invite recoit une notif, l'utilisateur qui invite recoit une RPL_INVITING
//La notification est une repoonse non ERR/RPL

//Reste a faire : Envoi notification, envoi RPL_INVITING.

void	Server::cmdInvite( std::vector<std::string>& args, int fd ) {


	if (args.size() != 2)
		buildMsg(ERR_NEEDMOREPARAMS, fd);

	std::string nickname = args[0];
	Channel& chan = getChannel(args[1]);

	if (!isValidNick(nickname))
		buildMsg(ERR_NOSUCHNICK, fd);

	else if (!chan.isUserOnChan(fd))
		buildMsg(ERR_NOTONCHANNEL, fd);

	else if (chan.isInviteOnly() && !chan.isUserChanOp(fd))
		buildMsg(ERR_CHANOPRIVSNEEDED, fd);

	else if (chan.isUserOnChan(nickname))
		buildMsg(ERR_USERONCHANNEL, fd);

	else {
		chan.addUserOnChan(*getClientByName(nickname));
		buildMsg(RPL_INVITING(args[0], chan.getName()), fd);
	}

}

//Parametres : JOIN ( <channel> *( "," <channel> ) [ <key> *( "," <key> ) ] ) / "0"
//Entre crochets : optionnels
//JOIN 0 --> L'utilisateur quitte tous les channels dont il est membre.

//Parsing supplementaire necessaire : split sur les virgules pour savoir si on veut join 2 chan ou si on veut en rejoindre un seul dont on specifie le mdp
void	Server::cmdJoin( std::vector<std::string>& args, int fd ) {

	std::string msg;

	if (args.size() < 1)
		buildMsg(ERR_NEEDMOREPARAMS, fd);

	std::vector<std::string>::iterator it = args.begin();
	for (; it != args.end() - 1; it++) {

		Channel& toJoin = getChannel(args[0]);
		std::string	key = *(it + 1);
		std::string chanName = toJoin.getName();
		std::string userName = getClientByFD(fd)->getNickName();

		if (!doesChanExist(chanName))
			addChannel(toJoin, key);

		else if (toJoin.isInviteOnly())		//Attention : L'utilisateur a peut etre ete invite. Verifier ?
			buildMsg(ERR_INVITEONLYCHAN(userName, chanName), fd);

		else if (toJoin.isFull())
			buildMsg(ERR_CHANNELISFULL(userName, chanName), fd);

		else if (!toJoin.isMatchingKey(key))
			buildMsg(ERR_BADCHANNELKEY(userName, key), fd);

		else {
			toJoin.addUserOnChan(*getClientByFD(fd));
			buildMsg(RPL_NAMEREPLY(userName, chanName), toJoin);
			buildMsg(RPL_ENDOFNAMES, toJoin);
		}
	}
}

//Parametres : PART <channel> *( "," <channel> ) [ <Part Message>]
//Ex : PART chan1 :j'me casse !
//Ex : PART chan1,chan2 :j'me casse !
//ex : PART chan1,chan2,chan3
//Parsing supplementaire pour differencier Part message des differents channels (split ,) comme dans JOIN
//Si pas de <part message> alors le message par defaut est le nickname
//Envoyer une reponse non RPL/ERR a tous les autres users du/des channels que l'utilisateur quitte avec part
//ex : PART chan1,chan2
void	Server::cmdPart( std::vector<std::string>& args, int fd ) {

	if (args.size() < 1)
		buildMsg(ERR_NEEDMOREPARAMS, fd);

	std::string partMsg;
	if (args.back()[0] == ':') {
		partMsg = args.back();
	    args.pop_back();
	}
	else
		partMsg = getClientByFD(fd)->getNickName();

	std::vector<std::string>::iterator it = args.begin();
	for (; it != args.end() - 1; it++) {

		std::istringstream ss(*it);
        std::string channelName;

		while (std::getline(ss, channelName, ',')) {

			Channel& toPart = getChannel(channelName);

			if (!toPart.isUserOnChan(fd))
				buildMsg(ERR_NOTONCHANNEL, fd);

			else {
				toPart.delUserOnChan(fd);
				buildMsg(partMsg, toPart);
			}
		}
	}
}


//Parametres : TOPIC <channel> [ <topic> ]
//Si pas de parametre topic, on envoie au user un RPL_TOPIC ou un RPL_NOTOPIC suivant s'il existe ou non
//Ex : TOPIC chan1 prout -->On remplace le topic existant de chan1 par prout
//Pour supprimer le TOPIC d'un chan il faut le remplacer par une str vide avec la cmd :
//TOPIC chan1 :
//Donc un peu de parsing vite f à faire.
void	Server::cmdTopic( std::vector<std::string>& args, int fd) {

	if (args.size() < 1)
		buildMsg(ERR_NEEDMOREPARAMS, fd);

	Channel& chan = getChannel(args[0]);

	if (!chan.isUserOnChan(fd))
		buildMsg(ERR_NOTONCHANNEL, fd);

	else if (!chan.isUserChanOp(fd))
		buildMsg(ERR_CHANOPRIVSNEEDED, fd);

	else if (args.size() == 1 && !chan.getTopic().empty())
		buildMsg(chan.getTopic(), fd);

	else if (args.size() == 1 && chan.getTopic().empty())
		buildMsg(RPL_NOTOPIC, fd);

	else {
		chan.setTopic(args[1]);
		buildMsg(RPL_TOPIC(chan.getName(), chan.getTopic()), chan);
	}
}


void	Server::cmdMode( const std::vector<std::string>& modes, int fd ) {

	if (!chan.isUserChanOp(fd))
		buildMsg(ERR_CHANOPRIVSNEEDED, fd);

	else if (!chan.isUserOnChan(fd))
		buildMsg(ERR_NOTONCHANNEL, fd);

	size_t	i = 0;
	for (; i < modes.size(); i++) {
		std::string tmp = modes[i].substr(0, ' ');
		
	}

}

void	Server::cmdNotice( std::vector<std::string>& args, int fd ) {

	if (args.size() < 2 )
		return;

	//Cible : channel
	if (args[0][0] == '#') {

		if (!doesChanExist(args[0])) {
			Channel &chan = getChannel(args[0]);
			if (chan.isUserOnChan(fd))
				buildMsg(args[1], chan);
		}
	}

	else if (doesUserExist(args[0]))
		if (doesUserExist(args[0]))
			buildMsg(args[2], getClientByName(args[0])->getFD());
}

void	Server::cmdPrivmsg( std::vector<std::string>& args, int fd ) {

	if (args.size() < 1)
		buildMsg(ERR_NORECIPIENT, fd);
	else if (args.size() < 2)
		buildMsg(ERR_NOTEXTTOSEND, fd);

	//Cible : channel
	else if (args[0][0] == '#') {

		if (!doesChanExist(args[0]))
			buildMsg(ERR_NOSUCHNICK, fd);

		else {

			Channel &chan = getChannel(args[0]);
			if (!chan.isUserOnChan(fd))
				buildMsg(ERR_CANNOTSENDTOCHAN, fd);
			else
				buildMsg(args[1], chan);
		}
	}

	//Cible : user unique.
	else {
		if (!doesUserExist(args[0]))
			buildMsg(ERR_NOSUCHNICK, fd);
		else
			buildMsg(args[2], getClientByName(args[0])->getFD());
	}
}



void	Server::cmdQuit( std::vector<std::string>& args, int fd ) {

	
}

void	Server::cmdWho( std::vector<std::string>& args, int fd ) {

	
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
		case eINVITE:
			cmdInvite(args, fd);//change by getClient(fd) -- Envoi des notif et des RPL a faire
			break;
		case eJOIN:
			cmdJoin(args, fd);	//Parsing a faire + rpl
			break;
		case eMODE:	//A verif plus tard, wip.
			cmdMode(args, fd);
			break;
		case eNICK:
			cmdNick(args, fd);
			break;
		case eNOTICE:
			cmdNotice(args, fd);
			break;
		case ePART: //Parsing a faire + envoi de tous les msg.
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
				{
					std::string tmp(buff);
					parseLine(tmp, fd);
				}
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


/* -------------------------------------------------------------------------- */
/*                                Building Args                               */
/* -------------------------------------------------------------------------- */

void	Server::buildMsg(const std::string& msg, Channel &chan)
{
	for(int i = 0; i < chan.nbClients(); i++)
	{
		int fd = chan.getClient(i).getFD();
		_messages.push_back(Message(msg, fd));
	}
}

void	Server::buildMsg(const std::string& msg, int fd)
{
	_messages.push_back(Message(msg, fd));
}


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