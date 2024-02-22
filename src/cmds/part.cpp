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

	if (!isUserSet(*getClientByFD(fd)))
		return (buildMsg(ERR_NOTREGISTERED, fd));

	if (args.size() < 2)
		buildMsg(ERR_NEEDMOREPARAMS(args[0]), fd);

	vecString chans = splitParamOnComas(args[1]);
	vecString::iterator it = chans.begin();

	for (; it < chans.end(); it++) {

		if (!doesChanExist(*it)) {
			buildMsg(ERR_NOSUCHCHANNEL(getClientByFD(fd)->getNickname(), *it), fd); 
			continue;
		}

		Channel& toPart = getChanByRef(*it);

		if (!toPart.isUserOnChan(fd))
			buildMsg(ERR_NOTONCHANNEL(*it), fd);

		else if (toPart.getNbClients() == 1)
			_channels.erase(getChanByIt(*it));

		else {
			toPart.delUser(*getClientByFD(fd));
			std::string str = toPart.getName();
			buildMsg(initMsgs(*getClientByFD(fd), args, str), toPart);
			std::cout << "DEBUG: PART : On del user " << getClientByFD(fd)->getNickname() << " de " << *it << std::endl;

		}
	}

/*
	vecString::iterator it = args.begin();
	it++;





	for (; it != args.end() - 1; it++) {

		std::istringstream ss(*it);
        std::string channelName;

		while (std::getline(ss, channelName, ',')) {

			Channel& toPart = getChanByRef(channelName);

			if (!doesChanExist(channelName))
				buildMsg(ERR_NOSUCHCHANNEL(channelName), fd);

			if (!toPart.isUserOnChan(fd))
				buildMsg(ERR_NOTONCHANNEL(channelName), fd);

			else {
				toPart.delUser(*getClientByFD(fd));
				buildMsg(partMsg, toPart);
			}
		}
	}*/
}
