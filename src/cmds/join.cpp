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

std::string	keyInit( vecString passwords, vecString::iterator it_pw ) {

	std::string key;
	if (it_pw >= passwords.end())
			key = "";
	else
		key = *it_pw;

	return (key);
}

void	Server::cmdJoin( vecString& args, int fd ) {

	if (!isUserSet(*getClientByFD(fd)))
		return (buildMsg(ERR_NOTREGISTERED, fd));

	if (args.size() < 2)
		return (buildMsg(ERR_NEEDMOREPARAMS(args[0]), fd));

	if (args[1] == "0")
		return (leaveAllChans(*getClientByFD(fd)));

	vecString chans = splitParamOnComas(args[1]);
	std::cout << chans[0] << std::endl; // debug
	vecString passwords;
	if (args.size() > 2)
		passwords = splitParamOnComas(args[2]);	
	vecString::iterator it = chans.begin();
	vecString::iterator it_pw = passwords.begin();
	for (; it != chans.end(); it++, it_pw++) {

		std::string chanName = *it;
		std::string key = keyInit(passwords, it_pw);

		if (!doesChanExist(chanName)) {
			std::cout <<"DEBUG : On cree le channel " << chanName << std::endl;
			if (createChannel(chanName, key, fd) == 1)	//nom du chan incorrect
				buildMsg(ERR_BADCHANMASK(chanName), fd);
			continue ;
		}

		Channel& toJoin = getChannel(chanName);
		if (toJoin.isInviteOnly())
			buildMsg(ERR_INVITEONLYCHAN(chanName), fd);

		else if (toJoin.isFull())
			buildMsg(ERR_CHANNELISFULL(chanName), fd);

		else if (!toJoin.isMatchingKey(key))
		{
			std::cout << "DEBUG: key entree = >" + key +"<\nkey du serveur : >" + toJoin.getPassword()+"<" << std::endl;
			buildMsg(ERR_BADCHANNELKEY(chanName), fd);
		}
		else {
			toJoin.addUserOnChan(*getClientByFD(fd));
			if (toJoin.getTopic().empty())
				buildMsg(RPL_NOTOPIC(chanName), fd);
			else
				buildMsg(RPL_TOPIC(chanName, toJoin.getTopic()), fd);
			buildMsg(RPL_NAMEREPLY(chanName, toJoin.getNamesOfChanUsers()), fd);
			buildMsg(RPL_ENDOFNAMES(chanName), fd);
			buildMsg(":" + getClientByFD(fd)->getNickname() + "!~" + getClientByFD(fd)->getUsername()
					+ "@" + _name + " JOIN " + chanName + "\r\n", toJoin);		}
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

bool	Server::isKeyValid( const std::string& key ) {

	if (key.size() > 23)
		return (false);

	if (!key.empty())
		for (std::string::const_iterator it = key.begin(); it != key.end(); it++)
			if ((*it >= 9 && *it <= 13) || *it == ' ' || *it == ':')
				return (false);
	return (true);
}

bool	Server::isChanValid( const std::string& chanName ) {

	if (chanName[0] != '#')
		return (false);

	for (std::string::const_iterator it = chanName.begin(); it != chanName.end(); it++)
		if (*it == 7 || *it == 13 || *it == 10 || *it == ',' || *it == ':')
			return (false);

	return (true);
}

void	Server::leaveAllChans( Client& client ) {

	vecChannel chanList = client.getUserChanList();
	vecChannel::iterator oni_chan = chanList.begin();
	for (; oni_chan != chanList.end(); oni_chan++) {
		client.delChanOfUser(*oni_chan);
		buildMsg(":" + client.getNickname() + "!~" + client.getUsername()
				+ "@" + _name + " PART " + oni_chan->getName() + "\r\n", *oni_chan);
	}
}