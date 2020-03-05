#ifndef RINGBUFFER_RINGBUFFER_RANGE_ERROR_HPP
#define RINGBUFFER_RINGBUFFER_RANGE_ERROR_HPP


#include <stdexcept>

class RingBuffer_range_error : public std::range_error {
private:
    size_t requestSpace;
    size_t freeSpace;
public:
    RingBuffer_range_error(const char *string, size_t requestSpace, size_t freeSpace);

    RingBuffer_range_error(const std::string &arg, size_t requestSpace, size_t freeSpace);


};


#endif //RINGBUFFER_RINGBUFFER_RANGE_ERROR_HPP
