#include "headers/RingBuffer.hpp"
#include "headers/RingBufferError.hpp"


RingBuffer::RingBuffer(size_t bufferSize) : buffer(std::vector<char>(bufferSize)), readIndex(0), writeIndex(0),
                                            capacity(0) {
}

void RingBuffer::addByte(char ch) {
    LockGuard locker(lock);
    checkIfPossibleTo(true, 1);
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
    checkIfPossibleTo(true, bytes.size());
    for (char byte : bytes) {
        addByteWithoutCheck(byte);
    }
}

bool RingBuffer::isPossibleTo(bool add, size_t numberOfBytes) {
    if (add) return (capacity + numberOfBytes <= buffer.size());
    else return (capacity >= numberOfBytes && capacity >= 0);

}

void RingBuffer::addByteWithoutCheck(char ch) {
    buffer[incrIndex(writeIndex)] = ch;
    ++capacity;
}

size_t RingBuffer::addSomeBytes(const std::vector<char> &bytes) {
    LockGuard locker(lock);
    size_t toAdd = bytes.size();
    if (!isPossibleTo(true, toAdd)) {
        toAdd = freeSpace();
    }
    for (int i = 0; i < toAdd; i++) {
        addByteWithoutCheck(bytes[i]);
    }
    return toAdd;
}

size_t RingBuffer::freeSpace() {
    return buffer.size() - capacity;
}

char RingBuffer::readByte() {
    LockGuard locker(lock);
    checkIfPossibleTo(false, 1);
    return readByteWithoutCheck();
}

char RingBuffer::readByteWithoutCheck() {
    capacity--;
    return buffer[incrIndex(readIndex)];
}

std::vector<char> RingBuffer::readAllBytes(size_t amount) {
    LockGuard locker(lock);

    checkIfPossibleTo(false, amount);
    std::vector<char> result(amount);
    for (int i = 0; i < amount; i++) {
        result[i] = readByteWithoutCheck();
    }
    return result;
}

std::vector<char> RingBuffer::readSomeBytes(size_t amount) {
    LockGuard locker(lock);
    if (!isPossibleTo(true, amount)) {
        amount = capacity;
    }
    return readAllBytes(amount);
}

size_t RingBuffer::getCapacity() {
    LockGuard locker(lock);
    return capacity;
}

bool RingBuffer::isFull() {
    LockGuard locker(lock);
    return capacity == buffer.size();
}

size_t RingBuffer::size() {
    LockGuard locker(lock);
    return buffer.size();
}

bool RingBuffer::empty() {
    LockGuard locker(lock);

    return capacity == 0;
}

void RingBuffer::resize(size_t newSize) {
    LockGuard locker(lock);
    if (newSize < capacity)
        throw RingBufferError("Cannot resize buffer. New Buffer length is less then RingBuffer capacity");
    std::vector<char> newBuffer(newSize);
    for (int i = 0; i < capacity; i++) {
        newBuffer[i] = buffer[incrIndex(readIndex)];
    }
    writeIndex = capacity;
    readIndex = 0;
    buffer = newBuffer;
}

size_t RingBuffer::incrIndex(size_t &index) {
    size_t index_ = index;
    index = (index + 1) % buffer.size();
    return index_;
}



