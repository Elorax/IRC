#include "Server.hpp"

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
		partMsg = getClientByFD(fd)->getNickname();

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