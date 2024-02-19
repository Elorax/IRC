#include "Message.hpp"

Message::Message(std::string const &msg, int fd) {
    _msg = msg;
    _fd = fd;
}

Message::~Message(){}

const std::string   &Message::getMsg( void ) const {
    return (_msg);
}

const int           &Message::getFD( void) const {
    return (_fd);
}
