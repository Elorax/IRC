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
#include "Message.hpp"

#define BUFFER_SIZE 1024
#define MAX_CLIENTS 10

# define RPL_WELCOME			001
# define RPL_YOURHOST			002
# define RPL_CREATED			003
# define RPL_MYINFO				004
# define RPL_ISUPPORT			005
# define RPL_UMODEIS			221
# define RPL_LUSERCLIENT		251
# define RPL_LUSERME			255
# define RPL_WHOISCERTFP 		276
# define RPL_AWAY				301
# define RPL_WHOISREGNICK 		307
# define RPL_WHOISUSER			311
# define RPL_WHOISSERVER		312
# define RPL_WHOISOPERATOR 		313
# define RPL_ENDOFWHO			315
# define RPL_WHOISIDLE 			317
# define RPL_ENDOFWHOIS			318
# define RPL_WHOISCHANNELS 		319
# define RPL_WHOISSPECIAL 		320
# define RPL_LISTSTART			321
# define RPL_LIST				322
# define RPL_LISTEND			323
# define RPL_WHOISACCOUNT 		330
# define RPL_NOTOPIC			331
# define RPL_TOPIC				332
# define RPL_CHANNELMODEIS		324
# define RPL_TOPICWHOTIME		333
# define RPL_WHOISACTUALLY 		338
# define RPL_INVITING			341
# define RPL_WHOREPLY			352
# define RPL_NAMREPLY			353
# define RPL_ENDOFNAMES			366
# define RPL_MOTD				372
# define RPL_MOTDSTART			375
# define RPL_ENDOFMOTD			376
# define RPL_WHOISHOST 			378
# define RPL_WHOISMODES 		379
# define RPL_YOUREOPER			381
# define RPL_WHOISSECURE 		671

# define RPL_KILLREPLY			1001
# define ERR_NOTREGISTERED		1002
# define ERR_NOLS				1003
# define ERR_NOCAP				1004
# define ERR_NOPASS				1005
# define ERR_NORIGHT			1007

# define ERR_NOSUCHNICK			401
# define ERR_NOSUCHCHANNEL		403
# define ERR_CANNOTSENDTOCHAN	404
# define ERR_TOOMANYCHANNELS	405
# define ERR_TOOMANYTARGETS		407
# define ERR_NOORIGIN			409
# define ERR_NORECIPIENT		411
# define ERR_NOTEXTTOSEND		412
# define ERR_NOTOPLEVEL			413
# define ERR_WILDTOPLEVEL		414
# define ERR_UNKNOWNCOMMAND		421
# define ERR_NOMOTD				422
# define ERR_NONICKNAMEGIVEN	431
# define ERR_ERRONEUSNICKNAME	432
# define ERR_NICKNAMEINUSE		433
# define ERR_USERNOTINCHANNEL	441
# define ERR_NOTONCHANNEL		442
# define ERR_USERONCHANNEL		443
# define ERR_NEEDMOREPARAMS		461
# define ERR_ALREADYREGISTERED	462
# define ERR_PASSWDMISMATCH 	464
# define ERR_CHANNELISFULL		471
# define ERR_UNKNOWNMODE		472
# define ERR_INVITEONLYCHAN		473
# define ERR_BANNEDFROMCHAN		474
# define ERR_BADCHANNELKEY		475
# define ERR_BADCHANMASK		476
# define ERR_NOPRIVILEGES		481
# define ERR_CHANOPRIVSNEEDED	482
# define ERR_NOOPERHOST			491
# define ERR_UMODEUNKNOWNFLAG	501

enum	eCommand {

	// eCAP,
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

typedef std::vector<Client> vecClient;

// void	cmd_cap( void );
void	cmd_invite( std::vector<std::string>& args, int fd );
void	cmd_join( std::vector<std::string>& args, int fd );
void	cmd_kick( std::vector<std::string>& args, int fd );
void	cmd_mode( std::vector<std::string>& args, int fd );
void	cmd_nick( std::vector<std::string>& args, int fd );
void	cmd_notice( std::vector<std::string>& args, int fd );
void	cmd_part( std::vector<std::string>& args, int fd );
void	cmd_pass( std::vector<std::string>& args, int fd );
void	cmd_privmsg( std::vector<std::string>& args, int fd );
void	cmd_quit( std::vector<std::string>& args, int fd );
void	cmd_topic( std::vector<std::string>& args, int fd );
void	cmd_user( std::vector<std::string>& args, int fd );
void	cmd_who( std::vector<std::string>& args, int fd );