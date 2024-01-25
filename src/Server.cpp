#include "../inc/Server.hpp"

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

int Server::getFD()
{
    return (_socketFD);
}

int Server::getMaxFD()
{
    //Return plus grand des fds entre celui du serveur et celui des clients
    int fdMax = _socketFD;
    vecClient::iterator it;
    for(it = _clients.begin(); it != _clients.end(); it++)
    {
        if (it->getFD() > fdMax)
            fdMax = it->getFD();
    }
    return (fdMax);
}

int	Server::addClient(fd_set &readFDs, fd_set &writeFDs)
{
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

std::vector<std::string> buildArgs(std::string line)
{
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


void    Server::parseLine(std::string line, int fd)
{
    //Si line ne se finit pas par \r\n, return
    if (line.find("\r\n") == std::string::npos)
        return ;

    if (line[0] == ':')//Prefixe : ignoré dans le contexte de ft_IRC
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

bool    Server::isAvailNick(const std::string &nick){}
bool    Server::isValidNick(const std::string &nick){}


void	Server::cmdNick( std::vector<std::string>& args, int fd ) {

	if (args.size() != 1)
		errorCase(ERR_NEEDMOREPARAMS, fd);

	if (!isAvailNick(args[0]))
		errorCase(ERR_NICKNAMEINUSE, fd);

	if (!isValidNick(args[0]))
		errorCase(ERR_ERRONEUSNICKNAME, fd);

	else
		getClientByFD(fd)->setNickName(args[0]);
}

void	Server::cmdPass( std::vector<std::string>& args, int fd ) {

	if(args.empty())
		errorCase(ERR_NEEDMOREPARAMS, fd);

	// if (getClient(fd).getPassword() != "")
	// 	errorCase(ERR_ALREADYREGISTERED, fd);

	if (getClientByFD(fd)->getPassword() != args[0])
		errorCase(ERR_PASSWDMISMATCH, fd);

	else
		getClientByFD(fd)->setPassword(args[0]);
}

void    Server::cmdUser( std::vector<std::string>& args, int fd)
{
    if (args.size() < 4)
        errorCase(ERR_NEEDMOREPARAMS, fd);

	else if (args[0] == getClientByFD(fd)->getUserName())
		errorCase(ERR_ALREADYREGISTERED, fd);
        
	else
		getClientByFD(fd)->setUserName(args[0]);
}

void		Server::addChannel( Channel& channel )
{
    _channels.push_back(channel);
}

void					Server::cmdInvite(std::vector<std::string> &args, int fd){}
void					Server::cmdJoin(std::vector<std::string> &args, int fd){}
void					Server::cmdMode(std::vector<std::string> &args, int fd){}
void					Server::cmdNotice(std::vector<std::string> &args, int fd){}
void					Server::cmdPart(std::vector<std::string> &args, int fd){}
void					Server::cmdPrivmsg(std::vector<std::string> &args, int fd){}
void					Server::cmdQuit(std::vector<std::string> &args, int fd){}
void					Server::cmdTopic(std::vector<std::string> &args, int fd){}
void					Server::cmdWho(std::vector<std::string> &args, int fd){}