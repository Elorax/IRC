
#include <iostream>
#include <ctype.h>

bool    isNumber(char *s)
{
    while (*s)
        if (!isdigit(*s++))
            return (false);
    return (true);
}

int main(int ac, char **av)
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
    server server(av[1], av[2]);
    
    return (0);
}


