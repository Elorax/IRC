#include "Server.hpp"
//! Ca fonctionne, verifier join 0
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
		return (partAllChans(fd));

	vecString passwords;
	vecString chans = splitParamOnComas(args[1]); 
	if (args.size() > 2)
		passwords = splitParamOnComas(args[2]);

	std::cout << chans[0] << std::endl; // debug
	vecString::iterator it = chans.begin();
	vecString::iterator it_pwd = passwords.begin();

	for (; it != chans.end(); it++, it_pwd++) {

		std::string chanName = *it;
		Channel& toJoin = getChanByRef(chanName);
		std::string key = keyInit(passwords, it_pwd);

		if (!doesChanExist(chanName)) {
			// std::cout <<"DEBUG : On cree le channel " << chanName << std::endl;
			//if (createChannel(chanName, key, fd) == 1)	//nom du chan incorrect
			//	buildMsg(ERR_BADCHANMASK(chanName), fd);
			//else {
			if (isChanValid(chanName) && isKeyValid(key)) {
				handleJoinMsg(chanName, *getClientByFD(fd));
				createChannel(chanName, key, fd);
			}
			continue;
		}

		switch (toJoin.isChanJoinable(key, fd)) {

			case   0: toJoin.addUser(*getClientByFD(fd)); handleJoinMsg(toJoin, *getClientByFD(fd)); break;
			case 471: buildMsg(ERR_CHANNELISFULL(getClientByFD(fd)->getNickname(), chanName), fd); break;
			case 473: buildMsg(ERR_INVITEONLYCHAN(getClientByFD(fd)->getNickname(), chanName), fd); break;
			case 475: buildMsg(ERR_BADCHANNELKEY(getClientByFD(fd)->getNickname(), chanName), fd); std::cout << "DEBUG: key entree = >" + key +"<\nkey du serveur : >" + toJoin.getPassword() + "<" << std::endl;  break;
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
		arg = arg.substr(arg.find(",") + 1);
		//arg.erase(0, pos + 1);
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

void	Server::partAllChans( int fd ) {

	std::cout <<"DEBUG: cas JOIN 0" << std::endl;
	vecString argsPart;
	argsPart.push_back("PART");
	std::string chans;

	vecChannel userChans = getClientByFD(fd)->getUserChanList();
	vecChannel::iterator it = userChans.begin();
	for(; it != userChans.end(); it++) {
		chans += it->getName();
		if (it + 1 != userChans.end())
			chans += ",";
	}
	//chans.erase(chans.rfind(","));
	argsPart.push_back(chans);
	cmdPart(argsPart, fd, 1);
}

//Appele lors de la REJOINDATION d'un channel deja existant
void	Server::handleJoinMsg( Channel& chan, Client& client ) {

	int fd = client.getFD();
	std::string chanName = chan.getName();

	buildMsg(JOINNOTICE(client.getNickname(), client.getUsername(), chanName), chan);

	if (chan.getTopic().empty())
		buildMsg(RPL_NOTOPIC(chanName), fd);
	else
		buildMsg(RPL_TOPIC(client.getNickname(), chanName, chan.getTopic()), fd);

	buildMsg(RPL_NAMEREPLY(client.getNickname(), chanName, chan.getNamesOfChanUsers()), fd);
	buildMsg(RPL_ENDOFNAMES(client.getNickname(), chanName), fd);
}

//Appele lors de la CREATION d'un channel
void	Server::handleJoinMsg( std::string chanName, Client& client ) {

	int fd = client.getFD();

	buildMsg(JOINNOTICE(client.getNickname(), client.getUsername(), chanName), fd);
	buildMsg(RPL_NOTOPIC(chanName), fd);
	buildMsg(RPL_NAMEREPLY(client.getNickname(), chanName, "@" + client.getNickname()), fd);
	buildMsg(RPL_ENDOFNAMES(client.getNickname(), chanName), fd);
}