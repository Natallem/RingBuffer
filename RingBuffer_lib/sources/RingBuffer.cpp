#include "headers/RingBuffer.hpp"
#include "headers/RingBufferError.hpp"

#define ADD true
#define READ false

RingBuffer::RingBuffer(size_t bufferCapacity) : buffer(std::vector<char>(bufferCapacity)), writeIndex(0), readIndex(0),
                                                size(0) {
}

void RingBuffer::addByte(char ch) {
    LockGuard locker(lock);
    checkIfPossibleTo(ADD, 1);
    addByteWithoutCheck(ch);
}

void RingBuffer::checkIfPossibleTo(bool add, size_t numberOfBytes) {//after lockGuard
    if (!isPossibleTo(add, numberOfBytes)) {
        std::string action = (add) ? "add" : "read";
        std::string lack = (add) ? "free space" : "elements";
        throw RingBufferError("Impossible to " + action + " " + std::to_string(numberOfBytes) +
                              " bytes. Buffer has not enough " + lack);
    }
}

void RingBuffer::addAllBytes(const std::vector<char> &bytes) {
    LockGuard locker(lock);
    checkIfPossibleTo(ADD, bytes.size());
    for (char byte : bytes) {
        addByteWithoutCheck(byte);
    }
}

bool RingBuffer::isPossibleTo(bool add, size_t numberOfBytes) {
    if (add) return (size + numberOfBytes <= buffer.size());
    return size >= numberOfBytes;
}

void RingBuffer::addByteWithoutCheck(char ch) {
    buffer[incrIndex(writeIndex)] = ch;
    ++size;
}

size_t RingBuffer::addSomeBytes(const std::vector<char> &bytes) {
    LockGuard locker(lock);
    size_t toAdd = bytes.size();
    if (!isPossibleTo(ADD, toAdd)) {
        toAdd = freeSpace();
    }
    for (size_t i = 0; i < toAdd; i++) {
        addByteWithoutCheck(bytes[i]);
    }
    return toAdd;
}

size_t RingBuffer::freeSpace() {
    return buffer.size() - size;
}

char RingBuffer::readByte() {
    LockGuard locker(lock);
    checkIfPossibleTo(READ, 1);
    return readByteWithoutCheck();
}

char RingBuffer::readByteWithoutCheck() {
    size--;
    return buffer[incrIndex(readIndex)];
}

std::vector<char> RingBuffer::readAllBytes(size_t number) {
    LockGuard locker(lock);
    checkIfPossibleTo(READ, number);
    return readBytesWithoutCheck(number);
}

std::vector<char> RingBuffer::readSomeBytes(size_t number) {
    LockGuard locker(lock);
    if (size < number) number = size;
    return readBytesWithoutCheck(number);
}

std::vector<char> RingBuffer::readBytesWithoutCheck(size_t number) {
    std::vector<char> result(number);
    for (size_t i = 0; i < number; i++) {
        result[i] = readByteWithoutCheck();
    }
    return result;
}

size_t RingBuffer::getSize() {
    LockGuard locker(lock);
    return size;
}

bool RingBuffer::isFull() {
    LockGuard locker(lock);
    return size == buffer.size();
}

size_t RingBuffer::getCapacity() {
    LockGuard locker(lock);
    return buffer.size();
}

bool RingBuffer::empty() {
    LockGuard locker(lock);

    return size == 0;
}

void RingBuffer::resize(size_t newSize) {
    LockGuard locker(lock);
    if (newSize < size)
        throw RingBufferError("Cannot resize buffer. New Buffer length is less then RingBuffer getCapacity");
    std::vector<char> newBuffer(newSize);
    for (size_t i = 0; i < size; i++) {
        newBuffer[i] = buffer[incrIndex(readIndex)];
    }
    writeIndex = size;
    readIndex = 0;
    buffer = newBuffer;
}

size_t RingBuffer::incrIndex(size_t &index) {
    size_t index_ = index;
    index = (index + 1) % buffer.size();
    return index_;
}



