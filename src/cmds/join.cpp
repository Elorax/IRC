#include "Server.hpp"

///Parametres : JOIN ( <channel> *( "," <channel> ) [ <key> *( "," <key> ) ] ) / "0"
//Entre crochets : optionnels
//JOIN 0 --> L'utilisateur quitte tous les channels dont il est membre.
//Parsing supplementaire necessaire : split sur les virgules pour savoir si on veut join 2 chan ou si on veut en rejoindre un seul dont on specifie le mdp
//Ex : JOIN #iciclafete
//Ex avec mdp : JOIN #iciclafeteprivee motdepasse123
//Ex avec virgules : JOIN #iciclafete,#contresoiree
//Ex avec virgules et mdp : JOIN #iciclafeteprivee,#contresoireepriveeaussi motdepasse123,lasecuriteclavie

//args[0] : Noms de channels, possiblement separes par des virgules.
//args[1] : Passwords optionnels, possiblement separes par des virgules.

std::string	keyInit( vecString passwords, vecString::iterator it_pwd ) {

	std::string key;
	if (it_pwd >= passwords.end())
			key = "";
	else
		key = *it_pwd;

	return (key);
}

void	Server::cmdJoin( vecString& args, int fd ) {
	if (!isUserSet(*getClientByFD(fd)))
		return (buildMsg(ERR_NOTREGISTERED, fd));

	if (args.size() < 2)
		return (buildMsg(ERR_NEEDMOREPARAMS(args[0]), fd));

	if (args[1] == "0")
		return (leaveAllChans(*getClientByFD(fd), args));

	vecString passwords;
	vecString chans = splitParamOnComas(args[1]);
	if (args.size() > 2)
		passwords = splitParamOnComas(args[2]);

	std::cout << chans[0] << std::endl; // debug
	vecString::iterator it = chans.begin();
	vecString::iterator it_pwd = passwords.begin();

	for (; it != chans.end(); it++, it_pwd++) {

		std::string chanName = *it;
		std::string key = keyInit(passwords, it_pwd);

		if (!doesChanExist(chanName)) {
			// std::cout <<"DEBUG : On cree le channel " << chanName << std::endl;
			//if (createChannel(chanName, key, fd) == 1)	//nom du chan incorrect
			//	buildMsg(ERR_BADCHANMASK(chanName), fd);
			//else {		
				handleJoinMsg(chanName, fd);
				createChannel(chanName, key, fd);
				// Channel& toJoin = getChanByRef(chanName);
				continue ;
			}
		Channel& toJoin = getChanByRef(chanName);

		switch (toJoin.isChanJoinable(key, fd)) {

			case   0: toJoin.addUser(*getClientByFD(fd)); handleJoinMsg(args, toJoin, fd); break;
			case 471: buildMsg(ERR_CHANNELISFULL(chanName), fd); break;
			case 473: buildMsg(ERR_INVITEONLYCHAN(chanName), fd); break;
			case 475: buildMsg(ERR_BADCHANNELKEY(chanName), fd); std::cout << "DEBUG: key entree = >" + key +"<\nkey du serveur : >" + toJoin.getPassword() + "<" << std::endl;  break;
			case 666: std::cout << "DEBUG: User already in chan." << std::endl;
		}
	}
}

/* ----------------------------- Function Helper ---------------------------- */

vecString	Server::splitParamOnComas( std::string& arg ) {

	vecString ret;
	size_t pos = 0;
	if (arg.find(",") == std::string::npos) {
		ret.push_back(arg);
		return (ret);
	}

	while ((pos = arg.find(",")) != std::string::npos) {
		ret.push_back(arg.substr(0, pos));
		arg.erase(0, pos + 1);
	}
	ret.push_back(arg);
	return (ret);
}

int	Server::createChannel( std::string chanName, std::string key, int fd ) {

	if (!isChanValid(chanName) || !isKeyValid(key))
		return (EXIT_FAILURE);
	Channel newChan(chanName, *getClientByFD(fd));
	addChannel(newChan, key);
	return (EXIT_SUCCESS);
}

void	Server::leaveAllChans( Client& client, vecString& args ) {

	vecChannel::iterator it = client.getUserChanList().begin();

	while (it != client.getUserChanList().end()) {

		it->delUser(client);
		std::string nameChan = it->getName();
		buildMsg(initMsgs(client, args, nameChan), *it);

		if (it->getNbClients() == 0) {
			std::cout << "CASE NO CLIENT :" << it->getNbClients() << " \r\n" << std::endl;
			vecChannel::iterator toErase = it; it++;
			_channels.erase(toErase);
		}
		else
			it++;
	}
}

//Appele lors de la REJOINDATION d'un channel deja existant
void	Server::handleJoinMsg( vecString& args, Channel& chan, int requesterFD ) {

	(void)args;
	std::string chanName = chan.getName();
	std::string send;
	send = ":"+getClientByFD(requesterFD)->getNickname()+"!~"+getClientByFD(requesterFD)->getUsername()+"@"+_name+" JOIN :"+chan.getName()+"\r\n";
	buildMsg(send, chan);
	if (chan.getTopic().empty())
		buildMsg(RPL_NOTOPIC(chanName), requesterFD);
	else
		buildMsg(RPL_TOPIC(chanName, chan.getTopic()), requesterFD);

	buildMsg(RPL_NAMEREPLY(getClientByFD(requesterFD)->getNickname(), chanName, chan.getNamesOfChanUsers()), requesterFD);
	buildMsg(RPL_ENDOFNAMES(getClientByFD(requesterFD)->getNickname(), chanName), requesterFD);
	// buildMsg(initMsgs(*getClientByFD(requesterFD), args, chanName), chan);
}

//Appele lors de la CREATION d'un channel
void	Server::handleJoinMsg( std::string chanName, int requesterFD ) {

	std::string send;
	send = ":"+getClientByFD(requesterFD)->getNickname()+"!~"+getClientByFD(requesterFD)->getUsername()+"@"+_name+" JOIN :"+chanName+"\r\n";
	buildMsg(send, requesterFD);
	buildMsg(RPL_NOTOPIC(chanName), requesterFD);
	buildMsg(RPL_NAMEREPLY(getClientByFD(requesterFD)->getNickname(), chanName, "@"+getClientByFD(requesterFD)->getNickname()), requesterFD);
	buildMsg(RPL_ENDOFNAMES(getClientByFD(requesterFD)->getNickname(), chanName), requesterFD);
	// buildMsg(initMsgs(*getClientByFD(requesterFD), args, chanName), requesterFD);
}