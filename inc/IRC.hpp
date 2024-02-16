#pragma once

#include <ctype.h>
#include <iostream>
#include <netinet/ip.h>
#include <string>
#include <vector>
#include <unistd.h>
#include <cstring>

#include "Server.hpp"
#include "Client.hpp"
#include "Channel.hpp"
#include "Message.hpp"

#define BUFFER_SIZE 1024
#define MAX_CLIENTS 10

#define RPL_WELCOME(nick)					"Welcome to the Internet Relay Network" + nick + "!\r\n"
#define RPL_YOURHOST						"Your host is ft_irc, running version 42.1\r\n"
#define RPL_CREATED							"This server was created today\r\n"
#define RPL_MYINFO							"ft_irc version 42.1 without any modes\r\n"

#define ERR_ALREADYREGISTRED				":Unauthorized command (already registered)\r\n"
#define ERR_BADCHANNELKEY(user, chan)		user +  " " + chan + ":Cannot join channel (+k)\r\n"
#define ERR_CANNOTSENDTOCHAN				":Cannot send to channel\r\n"
#define ERR_CHANNELISFULL(user, chan)		user +  " " + chan + ":Cannot join channel (+l)\r\n"
#define ERR_CHANOPRIVSNEEDED				":You're not channel operator\r\n"
#define ERR_ERRONEUSNICKNAME				":Erroneous nickname\r\n"
#define ERR_INVITEONLYCHAN(user, chan)		user + " " + chan + ":Cannot join channel (+i)\r\n"
#define ERR_KEYSET(user, chan)				user + " " + chan + ":Channel key already set\r\n"
#define ERR_NEEDMOREPARAMS					":Not enough parameters\r\n"
#define ERR_NICKNAMEINUSE					":Nickname is already in use\r\n"
#define ERR_NONICKNAMEGIVEN					":No nickname given\r\n"
#define ERR_NORECIPIENT						":No recipient given ("
#define ERR_NOSUCHCHANNEL					":No such channel\r\n"
#define ERR_NOSUCHNICK						":No such nick/channel\r\n"
#define ERR_NOTEXTTOSEND					":No text to send\r\n"
#define ERR_NOTONCHANNEL					":You're not on that channel\r\n"
#define ERR_PASSWDMISMATCH					":Password incorrect\r\n"
#define ERR_UNKNOWNMODE(mode, chan)			mode + ":is unknown mode char to me for " + chan + "\r\n"
#define ERR_USERNOTINCHANNEL				":They aren't on that channel\r\n"
#define ERR_USERONCHANNEL					":is already on channel\r\n"
#define RPL_CHANNELMODEIS					":" + user + " " + chan + " " + mod + " " + params + "\r\n"
#define RPL_ENDOFNAMES						":End of NAMES list.\r\n"
#define RPL_ENDOFWHO						":End of WHO list.\r\n"
#define RPL_INVITING(user, chan)			chan + " " + user + "\r\n"
#define RPL_NAMEREPLY(user, chan)			user + " join " + chan + "\r\n"
#define RPL_NOTOPIC							":No topic is set\r\n"
#define RPL_TOPIC(chan, topic)				chan + " " + topic "\r\n"
#define RPL_UNIQOPIS(user, chan, target)	user + " " + chan + " " + target + "\r\n"

enum	eCommand {

	eINVITE,
	eJOIN,
	eKICK,
	eMODE,
	eNICK,
	eNOTICE,
	ePART,
	ePASS,
	ePRIVMSG,
	eQUIT,
	eTOPIC,
	eUSER,
	eWHO,
	eNOTFOUND
};

typedef std::vector<Client>			vecClient;
typedef std::vector<Channel>		vecChannel;
typedef std::vector<Message>		vecMessage;
typedef std::vector<std::string>	vecString;
typedef std::vector<Client&>		refClient;
typedef std::vector<Channel&>		refChannel;
