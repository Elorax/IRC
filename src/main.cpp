
#include "IRC.hpp"

bool	isNumber(char *s)
{
	while (*s)
		if (!isdigit(*s++))
			return (false);

	return (true);
}

int	main(int ac, char **av) {

	try {
		std::cout << "\x1b[2J\x1b[H\n";
		if (ac != 3) {
			std::cerr << "Wrong parameters.\n'PORT' must be a number\n";
			return (EXIT_FAILURE);
		}
		else if (!isNumber(av[1])) {
			std::cerr << "Wrong parameters.\n'PORT' must be a number\n";
			return (EXIT_FAILURE);
		}
		std::string port(av[1]);
		std::string password(av[2]);
		Server server(port, password);
		server.run();
	}
	catch (std::exception& e) {
		std::cout << "erreur de throw :" << e.what() << std::endl;
		return (-1);
	}

	return (0);
} 