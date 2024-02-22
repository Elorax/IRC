#include "Server.hpp"

//Parametres : MODE <channel> *( ("-" / "+") *<modes> *<modeparams> )
//Ex : MODE #chan1 +k password
//k pour pw (avec param)
//o pour op un user dont le nickname est en parametre
//l pour limiter le nb d'user avec le nb en parametre
//t pour autoriser les user non OP a utiliser la commande TOPIC
//i pour set up le channel en inviteonly
//t et i ne prennent pas de parametre.
//Lettre inconnue : ERR_UNKNOWNMODE
// MODE +t 
// MODE +j
// MODE +jt

void	Server::cmdMode( vecString& args, int fd ) {

	if (!isUserSet(*getClientByFD(fd)))
		return (buildMsg(ERR_NOTREGISTERED, fd));

	if (args.size() < 2)
		return (buildMsg(ERR_NEEDMOREPARAMS(args[0]), fd));

	else if (args.size() == 2)
		handleModeListMsg(args[1], fd);

	else if (!doesChanExist(args[1]))
	{
		if (doesUserExist(args[1]))//keske
			return ;
		else
			return (buildMsg(ERR_NOSUCHCHANNEL(getClientByFD(fd)->getNickname(), args[1]), fd));
	}
	Channel& chan = getChanByRef(args[1]);

	if (!chan.isUserOnChan(fd))
		return (buildMsg(ERR_NOTONCHANNEL(args[1]), fd));

	else if (!chan.isUserChanOp(fd))
		return (buildMsg(ERR_CHANOPRIVSNEEDED(args[1]), fd));

	vecString::iterator it = args.begin();
	for(; it != args.end(); it++) {

		std::string token = *it;
		if (token[0] == '+') {
			std::string::iterator s = token.begin();
			for (++s; s != token.end(); ++s) {
				switch(*s) {
					case 'k': chan.setKey(*this, args, it, fd);		break;
					case 'o': chan.setOP(*this, args, it, fd);		break;
					case 'l': chan.setLimit(*this, args, it, fd);	break;
					case 't': chan.setTopicPriv();					break;
					case 'i': chan.setInviteOnly();					break;
					default: return buildMsg(ERR_UNKNOWNMODE(*it, chan.getName()), fd);
				}
			}
		}
		else if (token[0] == '-') {
			std::string::iterator s = token.begin();
			for (++s; s != token.end(); ++s) {
				switch(*s) {
					case 'k': chan.unsetKey();						break;
					case 'o': chan.unsetOP(*this, args, it, fd);	break;
					case 'l': chan.unsetLimit();					break;
					case 't': chan.unsetTopicPriv();				break;
					case 'i': chan.unsetInviteOnly();				break;
					default: return buildMsg(ERR_UNKNOWNMODE(*it, chan.getName()), fd);
				}
			}
		}
	}
}

void	Server::handleModeListMsg( std::string chanName, int fd ) {

	std::string modes = "+";
	Client& client = *getClientByFD(fd);
	Channel& chan = getChanByRef(chanName);

	if (chan.isChanKeySet())
		modes += "k";
	if (chan.isChanLimitSet())
		modes += "l";
	if (chan.isTopicPrivSet())
		modes += "t";
	if (chan.isInviteOnly())
		modes += "i";
	else
		modes = "No modes set for this channel";

	buildMsg(RPL_CHANNELMODEIS(client.getNickname(), chanName, modes), fd);
}