#include "Server.hpp"

void	Server::cmdWho( std::vector<std::string>& args, int fd ) {

	//if no mask, show all visible users without common channel without requester
	//same with 0
	vecChannel::iterator chanList = getClientByFD(fd)->getUserChanList();
	if (args.size() == 1) {
		for (; chanList != chanList->end(); chanList++)
			if ()
	}

	//otherwise return list of info matching the <mask> given
}