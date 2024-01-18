#include "Server.hpp"

Server::server(std::string port, std::string password)
{
    //Initialisation des parametres du serveur
    _port = atoi(port);
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
    return (_socketFD);
    //! ^ solution temporaire pour tests
}

int	Server::addClient(fd_set &readFDs, fd_set &writeFDs)
{
    int fd;
    fd = accept(_socketFD, (struct sockaddr*) &_clientAdrr, sizeof(_clientAdrr));
    if (fd == -1)
        return (-1);
    Client newClient(fd);
    std::cout << "NEW CLIENT\n";    //Useless mais utile pour mon debug
    _clients.push_back(newClient);    //Vecteur<client> a implementer
    FD_SET(fd, &readFDs);
    FD_SET(fd, &writeFDs);

    return (0);
}
