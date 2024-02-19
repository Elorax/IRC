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

	Channel& chan = getChannel(args[0]);

	if (args.size() < 2)
		return (buildMsg(ERR_NEEDMOREPARAMS, fd));

	else if (!doesChanExist(args[0]))
		return (buildMsg(ERR_NOSUCHCHANNEL, fd));

	else if (!chan.isUserOnChan(fd))
		return (buildMsg(ERR_NOTONCHANNEL, fd));

	else if (!chan.isUserChanOp(fd))
		return (buildMsg(ERR_CHANOPRIVSNEEDED, fd));

	vecString::iterator it = args.begin();
	for(; it != args.end(); it++) {

		std::string token = *it;
		if (token[0] == '+') {
			std::string::iterator s = token.begin();
			for (; s != token.end(); ++s) {
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
			for (; s != token.end(); ++s) {
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