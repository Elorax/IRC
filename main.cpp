
#include "inc/IRC.hpp"

bool	isNumber(char *s)
{
	while (*s)
		if (!isdigit(*s++))
			return (false);

	return (true);
}

int	main(int ac, char **av) {

	try {
		if (ac != 3)
			throw std::invalid_argument("Wrong parameters.\nUse case: './ircserv <port> <password>'");
		else if (!isNumber(av[1]))
			throw std::invalid_argument("Wrong parameters.\n'PORT' must be a number");
		std::string port(av[1]);
		std::string password(av[2]);
		Server server(port, password);
		server.run();
	}
	catch (std::exception& e) {
		std::cout << e.what() << std::endl;
	}

	return (0);
}
