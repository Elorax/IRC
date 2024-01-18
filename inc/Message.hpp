#pragma once

#include <string>
#include <iostream>

class Message{
    private:
        std::string _msg;
        int         _fd;

    public:
        Message(std::string const &msg, int fd);
        ~Message();
        const std::string   &getMsg( void ) const;
        const int           &getFD( void) const;

}

