
#include "inc/IRC.hpp"

bool	isNumber(char *s)
{
	while (*s)
		if (!isdigit(*s++))
			return (false);
	return (true);
}

int	main(int ac, char **av)
{
	//av[1] : port ; av[2] : password
	if (ac != 3)
	{
		std::cerr << "Error: Wrong number of arguments" << std::endl;
		return (1);
	}
	if(!isNumber(av[1]))
	{
		std::cerr << "Error: PORT must be a number" << std::endl;
		return (1);
	}
	std::string port(av[1]);
	std::string password(av[2]);
	Server server(port, password);
	server.run();
	
	return (0);
	
}
