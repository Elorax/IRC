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
		std::string	key = *it_pw;

	return (key);
}

void	Server::cmdJoin( vecString& args, int fd ) {

	if (args.size() < 1)
		buildMsg(ERR_NEEDMOREPARAMS, fd);

	if (args[0] == "0")
		return (leaveAllChans(*getClientByFD(fd)));

	vecString chans = splitParamOnComas(args[0]);
	vecString passwords = splitParamOnComas(args[1]);
	vecString::iterator it = chans.begin();
	vecString::iterator it_pw = passwords.begin();

	for (; it != chans.end(); it++, it_pw++) {

		std::string chanName = *it;
		std::string key = keyInit(passwords, it_pw);

		if (!doesChanExist(chanName))
			createChannel(chanName, key, fd);

		Channel& toJoin = getChannel(chanName);
		std::string userName = getClientByFD(fd)->getNickname();

		if (toJoin.isInviteOnly())
			buildMsg(ERR_INVITEONLYCHAN(userName, chanName), fd);

		else if (toJoin.isFull())
			buildMsg(ERR_CHANNELISFULL(userName, chanName), fd);

		else if (!toJoin.isMatchingKey(key))
			buildMsg(ERR_BADCHANNELKEY(userName, key), fd);

		else {
			toJoin.addUserOnChan(*getClientByFD(fd));
			buildMsg(RPL_NAMEREPLY(userName, chanName), toJoin);
			buildMsg(RPL_ENDOFNAMES, toJoin);
		}
	}
}

/* ----------------------------- Function Helper ---------------------------- */

vecString	Server::splitParamOnComas( std::string& arg ) {

	vecString ret;
	size_t pos = 0;
	while ((pos = arg.find(",")) != std::string::npos) {
		ret.push_back(arg.substr(0, pos));
		arg.erase(0, pos + 1);
	}
	return (ret);
}


int	Server::createChannel( std::string chanName, std::string key, int fd ) {
	if (!isChanValid(chanName) || !isKeyValid(key))
		return (EXIT_FAILURE);
	Channel newChan(chanName, *getClientByFD(fd));
	addChannel(newChan, key);
	return (EXIT_SUCCESS);
}

bool	Server::isKeyValid( std::string& key ) {

	if (key.size() > 23)
		return (false);

	if (!key.empty())
		for (std::string::iterator it = key.begin(); it != key.end(); it++)
			if ((*it >= 9 && *it <= 13) || *it == ' ' || *it == ':')
				return (false);
	return (true);
}

bool	Server::isChanValid( std:: string& chanName ) {

	if (chanName[0] != '#')
		return (false);

	for (std::string::iterator it = chanName.begin(); it != chanName.end(); it++)
		if (*it == 7 || *it == 13 || *it == 10 || *it == ',' || *it == ':')
			return (false);

	return (true);
}

void	Server::leaveAllChans( Client& client ) {

	vecChannel chanList = client.getUserChanList();
	vecChannel::iterator oni_chan = chanList.begin();
	for (; oni_chan != chanList.end(); oni_chan++) {
		client.delChanOfUser(*oni_chan);
		buildMsg(LEAVENOTICE(client.getNickname(), oni_chan->getName()), *oni_chan);
	}
}