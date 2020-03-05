#include "headers/RingBuffer_range_error.hpp"


RingBuffer_range_error::RingBuffer_range_error(const char *string, size_t requestSpace, size_t freeSpace) : range_error(
        string), requestSpace(requestSpace), freeSpace(freeSpace) {}

RingBuffer_range_error::RingBuffer_range_error(const std::string &arg, size_t requestSpace, size_t freeSpace)
        : range_error(arg), requestSpace(requestSpace), freeSpace(freeSpace) {}
