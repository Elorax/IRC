#include "Server.hpp"
//! Pas mal teste, a finir de tester + invite only une fois que invite est bon. Voir les msg d'erreur, fchouky il a des doutes.
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

	// ! ENTOINE ranger mode plus tard

	else if (!doesChanExist(args[1]))
	{
		if (doesUserExist(args[1]))//!keske
			return ;
		else
			return (buildMsg(ERR_NOSUCHCHANNEL(getClientByFD(fd)->getNickname(), args[1]), fd));
	}
	Channel& chan = getChanByRef(args[1]);
	if (!chan.isUserOnChan(fd))
		return (buildMsg(ERR_NOTONCHANNEL(args[1]), fd));
	if (args.size() == 2)	//!Ici cette fois on sait que le chan existe : plus de segfault.
		return (handleModeListMsg(args[1], fd));
	if (!chan.isUserChanOp(fd))
		return (buildMsg(ERR_CHANOPRIVSNEEDED(args[1]), fd));


	vecString::iterator it = args.begin() + 2;
	//for(; it != args.end(); it++) {	//Pas sur que ce soit un FOR, ca nous arrangerait que non en fait.
		vecString rep;
		//rep doit contenir +, tous les flags QUI ONT FONCTIONNE, et les parametres de ces flags.
		
		std::string token = *it;
		if (token[0] == '+') {
			std::string::iterator s = token.begin();
			rep.push_back("+");
			//Ici, verifier tout le token pour voir s'il y a un caractere interdit. Auquel cas on ne fait aucun mod.
			for (++s; s != token.end(); ++s) {
				switch(*s) {
					case 'k':
						if (chan.setKey(*this, args, ++it, fd) == 0)
						{
							rep[0] += "k";
							rep.push_back(*it);
						}
						break;
					case 'o': 
						if (chan.setOP(*this, args, ++it, fd) == 0){
							rep[0] += "o";
							rep.push_back(*it);
						}
						break;
					case 'l':
						if (chan.setLimit(*this, args, ++it, fd) == 0){
							rep[0] += "l";
							rep.push_back(*it);
						}
						break;
					case 't': chan.setTopicPriv(); rep[0] += "t";	break;
					case 'i': chan.setInviteOnly();	rep[0] += "i";	break;
					default: return (buildMsg(ERR_UNKNOWNMODE(*it, chan.getName()), fd));	//J'ai enleve le return parce qu'on fait quand meme les commandes suivantes.
				}
			}
		}
		else if (token[0] == '-') {
			std::string::iterator s = token.begin();
			rep.push_back("-");
			//Ici, verifier tout le token pour voir s'il y a un caractere interdit. Auquel cas on ne fait aucun mod.
			for (++s; s != token.end(); ++s) {
				switch(*s) {
					case 'k': chan.unsetKey(); rep[0] += "k";		break;
					case 'o': 
						if (chan.unsetOP(*this, args, ++it, fd) == 0)
						{
							rep[0] += "o";
							rep.push_back(*it);
						}
						break;
					case 'l': chan.unsetLimit(); rep[0] += "l";		break;
					case 't': chan.unsetTopicPriv(); rep[0] += "t";	break;
					case 'i': chan.unsetInviteOnly();rep[0] += "i";	break;
					default: return (buildMsg(ERR_UNKNOWNMODE(*it, chan.getName()), fd));
				}
			}
		}
		else if (token == "b")
			return ;	//on ignore MODE #chan b, envoye par defaut depuis irssi
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
		buildMsg(toSend, chan);//! C'etait tosend, fd avant mais je pense que chan c'est mieux. A tester et reverse si pas bien
	//}
}

/*
std::string random = "+k+i+j+l";
while (random.rfind("+") != random.find("+")))
	random.erase(random.rfind("+"));
--> random = "	+kijl"
*/

static std::string size_t_to_string(size_t nb){
    std::stringstream ss;
    ss << nb;
    std::string str = ss.str();
    return str;
}

/*
static std::string size_t_to_str(size_t nb);
{
	char buffer[256] = {0};
	std::ret;
	ret = ultoa(nb, buffer, 10);
	return (ret);
}*/

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

