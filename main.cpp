
#include <iostream>
#include <ctype.h>

bool	isNumber(char *s)
{
	while (*s)
		if (!isdigit(*s++))
			return (false);
	return (true);
}

int	main(int ac, char **av)
{
	//av[1] : port ; av[2] : password
	if (ac != 3)
	{
		std::cerr << "Error: Wrong number of arguments" << std::endl;
		return (1);
	}
	if(!isNumber(av[1]))
	{
		std::cerr << "Error: PORT must be a number" << std::endl;
		return (1);
	}
	server server(av[1], av[2]);
	handleServer(server);
	
	return (0);
	
}

void	handleServer(Server &server)
{
	fd_set readFDs, writeFDs;
	FD_ZERO(&readFDs);
	FD_ZERO(&writeFDs);
	FD_SET(server.getFD(), &readFDs);

	while (1)	//variable globale mise a zero par une interruption ?
	{
	//Penser a une solution de quitter le programme proprement en cas de ctrl c avec des signaux.
		if (select(server.getMaxFD() + 1, &readFDs, &writeFDs, NULL, NULL) < 0)
		{
			std::cerr << "error ta mere" << std::endl;
			break ;
		}

		if (FD_ISSET(server.getFD(), &readFDs)) //maginoire listen te titille le server donc tu rentres 
		{
			if (server.addClient(readFDs, writeFDs) != 0)   //Ptete ca marche pas avec une ref tavu
			{
				std::cerr << "accept failed" << std::endl;
				continue;
			}
		}
		// faire une fonction qui envoie tous les messages a envoyer (les messages etant des reponses (erreur ou non ERR/RPL/non defini))
		
		// faire une boucle sur les clients et quand FD_ISSET(fd_client, read) reussi, read ce que tu recois du client puis parser la ligne, si elle finit par \r\n l executer, si tu reconnais une commande la faire sinon rien faire
	}



	//Ajouter un nouveau client (accept, update les fd sets)

	//Verifier si un client deja accepte fait une action
	
}
