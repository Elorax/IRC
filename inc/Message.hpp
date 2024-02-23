#pragma once

#include "IRC.hpp"

class Message{
    private:
        std::string _msg;
        int         _fd;

    public:
        Message( std::string const& msg, int fd );
        ~Message();

        const std::string&	getMsg( void ) const;
        const int&			getFD( void ) const;
        void                setMsg(std::string &string);
        void                setFD(int fd);

};
