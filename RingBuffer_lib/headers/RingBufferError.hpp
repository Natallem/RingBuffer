#ifndef RINGBUFFER_RINGBUFFERERROR_HPP
#define RINGBUFFER_RINGBUFFERERROR_HPP


#include <stdexcept>

class RingBufferError : public std::exception {
public:
    explicit RingBufferError(const std::string &message);

    const char *what() const throw() override;

private:
    std::string message;
};


#endif //RINGBUFFER_RINGBUFFERERROR_HPP
