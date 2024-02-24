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

	else if (!doesChanExist(args[1])) {
		if (doesUserExist(args[1]))
			return ;
		else
			return (buildMsg(ERR_NOSUCHCHANNEL(getClientByFD(fd)->getNickname(), args[1]), fd));
	}

	Channel& chan = getChanByRef(args[1]);
	if (!chan.isUserOnChan(fd))
		return (buildMsg(ERR_NOTONCHANNEL(args[1]), fd));
	if (args.size() == 2)
		return (handleModeListMsg(args[1], fd));
	if (!chan.isUserChanOp(fd))
		return (buildMsg(ERR_CHANOPRIVSNEEDED(args[1]), fd));

	vecString::iterator it = args.begin() + 2;
	vecString rep;

	std::string token = *it;
	if (token[0] == '+') {

		std::string::iterator s = token.begin();
		rep.push_back("+");
		for (++s; s != token.end(); ++s) {
			switch(*s) {
				case 'k':
					if (chan.setKey(*this, args, ++it, fd) == 0) {
						rep[0] += "k";
						rep.push_back(*it);
					}
					break;
				case 'o': 
					if (chan.setOP(*this, args, ++it, fd) == 0) {
						rep[0] += "o";
						rep.push_back(*it);
					}
					break;
				case 'l':
					if (chan.setLimit(*this, args, ++it, fd) == 0) {
						rep[0] += "l";
						rep.push_back(*it);
					}
					break;
				case 't': chan.setTopicPriv(); rep[0] += "t";	break;
				case 'i': chan.setInviteOnly();	rep[0] += "i";	break;
				default: return (buildMsg(ERR_UNKNOWNMODE(token, chan.getName()), fd));
			}
		}
	}

	else if (token[0] == '-') {

		std::string::iterator s = token.begin();
		rep.push_back("-");
		for (++s; s != token.end(); ++s) {
			switch(*s) {
				case 'k': chan.unsetKey(); rep[0] += "k";		break;
				case 'o': 
					if (chan.unsetOP(*this, args, ++it, fd) == 0) {
						rep[0] += "o";
						rep.push_back(*it);
					}
					break;
				case 'l': chan.unsetLimit(); rep[0] += "l";		break;
				case 't': chan.unsetTopicPriv(); rep[0] += "t";	break;
				case 'i': chan.unsetInviteOnly();rep[0] += "i";	break;
				default: return (buildMsg(ERR_UNKNOWNMODE(token, chan.getName()), fd));
			}
		}
	}
	else if (token == "b")
		return ;
	else
		return (buildMsg(ERR_UNKNOWNMODE(*it, chan.getName()), fd));
	vecString::iterator ite = rep.begin();
	std::string repStr = "";
	for (;ite != rep.end(); ite++){
		repStr += *ite;
		if (ite + 1 != rep.end())
			repStr += " ";
	} 
	std::string toSend = ":" + getClientByFD(fd)->getNickname() +"!~"+ getClientByFD(fd)->getUsername()+"@"+_name+" MODE "+ chan.getName() + " " + repStr + "\r\n";
	if (!(rep[0] == "+" || rep[0] == "-"))
		buildMsg(toSend, chan);
}

static std::string size_t_to_string(size_t nb){
    std::stringstream ss;
    ss << nb;
    std::string str = ss.str();
    return str;
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
	if (chan.isChanKeySet())
		modes += " " + chan.getPassword();
	if (chan.isChanLimitSet())
		modes += " " + size_t_to_string(chan.getChanLimit());
	if (modes == "+")
		modes = "No modes set for this channel";

	buildMsg(RPL_CHANNELMODEIS(client.getNickname(), chanName, modes), fd);
}

