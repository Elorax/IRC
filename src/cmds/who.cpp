#include "Server.hpp"
//!Affichage pas correct (teste avec /who Elo), manque des morceaux, verifier en copiant sur fchouky.
//if no mask, show all visible users without common channel with requester
//same with 0
//otherwise return list of info matching the <mask> given
void	Server::cmdWho( vecString& args, int fd ) {

	Client& client = *getClientByFD(fd);

	if (!isUserSet(client))
		return (buildMsg(ERR_NOTREGISTERED, fd));

	if (args.size() == 1 || args[1][0] == '0')
		whoAll(client);

	else if (doesChanExist(args[1])) {
		Channel& target = getChanByRef(args[1]);
		whoChannel(target, client);
	}

	else if (doesUserExist(args[1])) {
		Client& target = *getClientByName(args[1]);
		whoClient(target, client);
	}
	buildMsg(RPL_ENDOFWHO(client.getNickname(), args[1]), fd);
}
/* --------------------------------- Helpers -------------------------------- */

void	Server::whoAll( Client& client ) {

	std::string	nick = client.getNickname();
	vecChannel::iterator itChan = _channels.begin();

	for (; itChan != _channels.end(); itChan++) {

		vecClient chanUsers = itChan->getChanUsers();
		if (!itChan->isUserOnChan(client.getFD()))
			whoChannel(*itChan, client);
	}
}

void	Server::whoClient( Client& target, Client& client ) {

	std::string whoMsg;


	std::cout << "DEBUG WHO : " << target.getUserChanList().back().getName() << std::endl;
	whoMsg = target.getNickname() + " ";
	if (!target.getUserChanList().empty())
		whoMsg += target.getUserChanList().back().getName();
	whoMsg += target.getUsername() + " ft_irc" + " ft_irc ~" + target.getNickname() + " H@ :@ " + target.getRealname();

//to_send = ":"+_name+" 352 " + get_client_by_fd(param.get_fd()).get_nickname()+" "+get_client_by_fd(param.get_fd()).get_last_channel()+" ~"+get_client_by_fd(param.get_fd()).get_username()+" "+_name+" " + _name + " " + args[0] + " H@ :0 " + get_client_by_fd(param.get_fd()).get_realname()+"\r\n";
//to_send = ":ft_irc_all 352 " + (*it).get_nickname()+" "+chan_name+" ~"+(*it).get_username()+" "+_name+" " + _name + " " + (*it).get_nickname() + " H :0 " + (*it).get_realname()+"\r\n";

	buildMsg(WHONOTICE(client.getNickname(), client.getUsername(), target.getNickname()), client.getFD());
	buildMsg(RPL_WHOREPLY(client.getNickname(), whoMsg), client.getFD());
}

void	Server::whoChannel( Channel& target, Client& client ) {

	vecString whoMsg;
	whoMsg.push_back(target.getName());

	vecClient chanUsers = target.getChanUsers();
	vecClient::iterator itUser = chanUsers.begin();
	for (; itUser != chanUsers.end(); itUser++) {
		whoClient(*itUser, client);
	}
}