#include "Server.hpp"

//Parametres : PART <channel> *( "," <channel> ) [ <Part Message>]
//Ex : PART chan1 :j'me casse !
//Ex : PART chan1,chan2 :j'me casse !
//ex : PART chan1,chan2,chan3
//Ex : PART chan1 :prout
//Parsing supplementaire pour differencier Part message des differents channels (split ,) comme dans JOIN
//Si pas de <part message> alors le message par defaut est le nickname
//Envoyer une reply non RPL/ERR a tous les autres users du/des channels que l'utilisateur quitte avec part
//ex : PART chan1,chan2

void	Server::cmdPart( vecString& args, int fd ) {

	Client& client = *getClientByFD(fd);

	if (!isUserSet(client))
		return (buildMsg(ERR_NOTREGISTERED, fd));

	if (args.size() < 2)
		buildMsg(ERR_NEEDMOREPARAMS(args[0]), fd);

	vecString chans = splitParamOnComas(args[1]);
	vecString::iterator it = chans.begin();

	for (; it < chans.end(); it++) {

		Channel& toPart = getChanByRef(*it);
		if (!doesChanExist(*it)) {
			buildMsg(ERR_NOSUCHCHANNEL(client.getNickname(), *it), fd); 
			continue;
		}

		if (!toPart.isUserOnChan(fd))
			buildMsg(ERR_NOTONCHANNEL(*it), fd);

		else {

			std::string msg = (args.size() == 3 ? msg = client.getNickname() : msg = args[2]);
			buildMsg(PARTNOTICE(client.getNickname(), client.getUsername(), toPart.getName(), msg), toPart);
			toPart.delUser(client);
			std::cout << "DEBUG: PART : On del user " << client.getNickname() << " de " << *it << std::endl;

		}
		if (toPart.getNbClients() == 0)
			_channels.erase(getChanByIt(*it));
	}
}
