#include "RingBufferError.hpp"

RingBufferError::RingBufferError(const std::string &message) : message(message) {}

const char *RingBufferError::what() const throw()  {
    return message.c_str();
}
