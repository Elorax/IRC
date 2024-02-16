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

void	Server::cmdMode( std::vector<std::string>& args, int fd ) {
	
	Channel& chan = getChannel(args[0]);

	if (args.size() < 2)
		buildMsg(ERR_NEEDMOREPARAMS, fd);

	else if (!doesChanExist(args[0]))
		buildMsg(ERR_NOSUCHCHANNEL, fd);

	else if (!chan.isUserOnChan(fd))
		buildMsg(ERR_NOTONCHANNEL, fd);

	else if (!chan.isUserChanOp(fd))
		buildMsg(ERR_CHANOPRIVSNEEDED, fd);
	std::vector<std::string>::iterator it = args.begin();
	for(; it != args.end(); it++)
	{
		std::string tmp = *it;
		if (tmp[0] == '+')
		{
			std::string::iterator s = tmp.begin();
			s++;
			for (; s != tmp.end(); s++)
			{
				switch (*s)
				{
					case 'k':
					{
						if (++it == args.end())
						{
							buildMsg(ERR_NEEDMOREPARAMS, fd);
							return ;
						}
						else if (chan.isChanKeySet())
						{
							buildMsg(ERR_KEYSET(getClientByFD(fd)->getNickName(), chan.getName()), fd);
							return ;
						}
						else
						{
							chan.setPassword(*it);
							chan.setChanKeyStatus(true);
						}
						break;
					}
					case 'o':
					{
						if (++it == args.end())
						{
							buildMsg(ERR_NEEDMOREPARAMS, fd);
							return ;
						}
						else if (!chan.isUserOnChan(*it))
						{
							buildMsg(ERR_USERNOTINCHANNEL, fd);
							return ;
						}
						else if (!chan.isUserChanOp(*it))
							chan.setChanOP(*getClientByName(*it));
						break;
					}
					case 'l':
					{
						if (++it == args.end())
						{
							buildMsg(ERR_NEEDMOREPARAMS, fd);
							return ;
						}
						else
						{
							chan.setInviteOnly(true);
							chan.setChanCapacity(atoi(it->c_str()));
						}
						break;
					}
					case 't':
					{
						chan.setTopicPriv(true);
						break;
					}
					case 'i':
					{
						chan.setInviteOnly(true);
						break;
					}
					default:
					{
						buildMsg(ERR_UNKNOWNMODE(tmp, chan.getName()), fd);
						return ;
					}
				}
			}
		}


		else if (tmp[0] == '-')
		{
			std::string::iterator s = tmp.begin();
			s++;
			for (; s != tmp.end(); s++)
			{
				switch (*s)
				{
					case 'k':
					{
						chan.setPassword("");
						chan.setChanKeyStatus(false);
						break;
					}
					case 'o':
					{
						if (++it == args.end())
						{
							buildMsg(ERR_NEEDMOREPARAMS, fd);
							return ;
						}
						else if (!chan.isUserOnChan(*it))
						{
							buildMsg(ERR_USERNOTINCHANNEL, fd);
							return ;
						}
						else if (chan.isUserChanOp(*it))
							chan.unsetChanOP(*getClientByName(*it));
						break;
					}
					case 'l':
					{
						if (++it == args.end())
						{
							buildMsg(ERR_NEEDMOREPARAMS, fd);
							return ;
						}
						else
						{
							chan.setInviteOnly(false);
							chan.setChanCapacity(-1);
						}
						break;
					}
					case 't':
					{
						chan.setTopicPriv(false);
						break;
					}
					case 'i':
					{
						chan.setInviteOnly(false);
						break;
					}
					default:
					{
						buildMsg(ERR_UNKNOWNMODE(tmp, chan.getName()), fd);
						return ;
					}
				}
			}
		}
	}
}