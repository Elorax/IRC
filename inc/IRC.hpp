#pragma once

#include <ctype.h>
#include <iostream>
#include <netinet/ip.h>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <unistd.h>
#include <cstring>
#include <csignal>
#include <stdlib.h>

class Server;
class Client;
class Channel;
class Message;

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
	eCAP,
	ePING,
	eNOTFOUND
};

typedef std::vector<Client>			vecClient;
typedef std::vector<Channel>		vecChannel;
typedef std::vector<Message>		vecMessage;
typedef std::vector<std::string>	vecString;



#include "Server.hpp"
#include "Client.hpp"
#include "Channel.hpp"
#include "Message.hpp"
#define BUFFER_SIZE 1024
#define MAX_CLIENTS 10

#define RPL_WELCOME(nick)							":ft_irc 001 "	+ nick 	+ " :Welcome to the Internet Relay Network " + nick + "!\r\n"
#define RPL_YOURHOST(nick)							":ft_irc 002 "	+ nick 	+ " :Your host is ft_irc, running version 42.1\r\n"
#define RPL_CREATED(nick)							":ft_irc 003 "	+ nick 	+ " :This server was created today\r\n"
#define RPL_MYINFO(nick)							":ft_irc 004 "	+ nick 	+ " :Version 42.1 running with the following channel modes : 'k, o, l, t, i' \r\n"

#define ERR_NOSUCHNICK(nick)						":ft_irc 401 "	+ nick 	+ " :No such nick/channel\r\n"
#define ERR_NOSUCHCHANNEL(user, chan)				":ft_irc 403 "	+ user 	+ " " + chan + " :No such channel\r\n"
#define ERR_CANNOTSENDTOCHAN(chan)					":ft_irc 404 "	+ chan 	+ " :Cannot send to channel\r\n"
#define ERR_NOORIGIN(nick)							":ft_irc 409 "	+ nick 	+ " :No origin specified\r\n"	
#define ERR_NORECIPIENT(cmd)						":ft_irc 411 "	+ cmd 	+ ": No recipient given\r\n"
#define ERR_NOTEXTTOSEND							":ft_irc 412 :No text to send\r\n"
#define ERR_UNKNOWNCOMMAND(user, cmd) 				":ft_irc 421 "	+ user 	+ " " + cmd  + " :Unknown command\r\n"
#define ERR_NONICKNAMEGIVEN							":ft_irc 431 :No nickname given\r\n"
#define ERR_ERRONEUSNICKNAME(nick)					":ft_irc 432 "	+ nick 	+ " :Erroneous nickname\r\n"
#define ERR_NICKNAMEINUSE(user, nick)				":ft_irc 433 "	+ user 	+ " " + nick + " :Nickname is already in use\r\n"
#define ERR_USERNOTINCHANNEL(nick, chan)			":ft_irc 441 "	+ nick 	+ " " + chan + " :They aren't on that channel\r\n"
#define ERR_NOTONCHANNEL(chan)						":ft_irc 442 "	+ chan 	+ " :You're not on that channel\r\n"
#define ERR_USERONCHANNEL(user, chan)				":ft_irc 443 "	+ user 	+ " " + chan + " :is already on channel\r\n"
#define ERR_NOTREGISTERED 							":ft_irc 451 :You have not registered\r\n"
#define ERR_NEEDMOREPARAMS(cmd)						":ft_irc 461 "	+ cmd 	+ " :Not enough parameters\r\n"
#define ERR_ALREADYREGISTRED						":ft_irc 462 :Unauthorized command (already registered)\r\n"
#define ERR_PASSWDMISMATCH							":ft_irc 464 :Password incorrect\r\n"
#define ERR_KEYSET(chan)							":ft_irc 467 "	+ chan	+ " :Channel key already set\r\n"
#define ERR_CHANNELISFULL(nick, chan)				":ft_irc 471 "	+ nick	+ " " + chan + " :Cannot join channel (+l)\r\n"
#define ERR_UNKNOWNMODE(char, chan)					":ft_irc 472 "	+ char	+ " :is unknown mode char to me for " + chan + "\r\n"
#define ERR_INVITEONLYCHAN(nick, chan)				":ft_irc 473 "	+ nick	+ " " + chan + " :Cannot join channel (+i)\r\n"
#define ERR_BADCHANNELKEY(nick, chan)				":ft_irc 475 "	+ nick	+ " " + chan + " :Cannot join channel (+k)\r\n"
#define ERR_BADCHANMASK(chan)    					":ft_irc 476 "	+ chan	+ " :Bad Channel Mask\r\n"
#define ERR_CHANOPRIVSNEEDED(chan)					":ft_irc 482 "	+ chan	+ " :You're not channel operator\r\n"

#define RPL_ENDOFWHO(nick, target)					":ft_irc 315 "	+ nick	+ target + " :End of WHO list.\r\n"
#define RPL_CHANNELMODEIS(nick, chan, mode)			":ft_irc 324 "	+ nick	+ " " + chan + " :" + mode + " " + "\r\n"
#define RPL_UNIQOPIS(chan, nick)					":ft_irc 325 "	+ chan	+ " " + nick + "\r\n"
#define RPL_NOTOPIC(chan)							":ft_irc 331 "	+ chan	+ " :No topic is set\r\n"
#define RPL_TOPIC(nick, chan, topic)				":ft_irc 332 "	+ nick	+ " " + chan + " :" + topic + "\r\n"
#define RPL_INVITING(src, chan, dest)				":ft_irc 341 "	+ src	+ " " + chan + " " + dest + "\r\n"
#define RPL_WHOREPLY(nick, who)						":ft_irc 352 "	+ nick	+ " :" + who + "\r\n"
#define RPL_NAMEREPLY(nick, chan, users)			":ft_irc 353 "	+ nick	+ " = "	+ chan + " :" + users + "\r\n"
#define RPL_ENDOFNAMES(nick, chan)					":ft_irc 366 "	+ nick	+ " " + chan + " :End of NAMES list.\r\n"

/* irssi format replies */
#define PONGNOTICE(client)							":ft_irc PONG "	+ client + "\r\n"
#define WHONOTICE(nickname, username, arg)			":"	+ nickname	+ "!~" + username + "@ft_irc" + " WHO " + arg + "\r\n"
#define NICKNOTICE(nickname, username, arg)			":" + nickname	+ "!~" + username + "@ft_irc" + " NICK " + arg + "\r\n"
#define JOINNOTICE(nickname, username, chan)		":" + nickname	+ "!~" + username + "@ft_irc" + " JOIN " + chan	+ "\r\n"
#define PARTNOTICE(nickname, username, chan, arg)	":" + nickname	+ "!~" + username + "@ft_irc" + " PART " + chan	+ " :"+ arg + "\r\n"
#define MSGNOTICE(nickname, username, dest, msg)	":" + nickname	+ "!~" + username + "@ft_irc" + " NOTICE "	+ dest	+ " " + msg + "\r\n"
#define PRIVMSGNOTICE(nickname, username, dest, msg)":" + nickname	+ "!~" + username + "@ft_irc" + " PRIVMSG "	+ dest	+ " " + msg + "\r\n"
// #define TESTPONGNOTICE								"PONG :ft_irc\r\n"
