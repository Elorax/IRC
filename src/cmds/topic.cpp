#include "Server.hpp"

//Parametres : TOPIC <channel> [ <topic> ]
//Si pas de parametre topic, on envoie au user un RPL_TOPIC ou un RPL_NOTOPIC suivant s'il existe ou non
//Ex : TOPIC chan1 prout -->On remplace le topic existant de chan1 par prout
//Pour supprimer le TOPIC d'un chan il faut le remplacer par une str vide avec la cmd :
//TOPIC chan1 :
//Donc un peu de parsing vite f à faire.

void	Server::cmdTopic( vecString& args, int fd) {

	if (args.size() < 1)
		buildMsg(ERR_NEEDMOREPARAMS, fd);

	Channel& chan = getChannel(args[0]);

	if (!chan.isUserOnChan(fd))
		buildMsg(ERR_NOTONCHANNEL, fd);

	else if (chan.isTopicPrivSet() && !chan.isUserChanOp(fd))
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