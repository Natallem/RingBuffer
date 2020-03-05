#include <iostream>
#include "headers/RingBuffer.hpp"
#include "headers/RingBuffer_range_error.hpp"


RingBuffer::RingBuffer(size_t bufferSize) : buffer(std::vector<char>(bufferSize)), readIndex(0), writeIndex(0),
                                            fullness(0) {
//    std::cout << "RingBuffer created";
}

void RingBuffer::addByte(char ch) {
    LockGuard locker(lock);

    checkIfPossibleTo(true, 1);
//    std::cout << " before added "<< (int) ch << " size=" <<fullness << "\n";
    addByteWithoutCheck(ch);
//    std::cout << "added "<< (int) ch << " size=" << fullness << "\n";
}

void RingBuffer::checkIfPossibleTo(bool add, size_t numberOfBytes) {//after lockGuard
//    LockGuard locker(lock);
    if (!isPossibleTo(add, numberOfBytes)) {
        std::string action = (add) ? "add" : "read";
        std::string problem = (add) ? "full" : "empty";

        throw RingBuffer_range_error(
                "Impossible to " + action + " " + std::to_string(numberOfBytes) + " bytes. Buffer is " + problem,
                numberOfBytes, freeSpace());
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
//    LockGuard locker(lock);
    if (add) return (fullness + numberOfBytes <= buffer.size());
    else return (fullness >= numberOfBytes && fullness >= 0);

}

void RingBuffer::addByteWithoutCheck(char ch) {
    buffer[writeIndex++] = ch;
    if (writeIndex == buffer.size()) writeIndex = 0;
    ++fullness;
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
    return buffer.size() - fullness;
}

char RingBuffer::readByte() {
    LockGuard locker(lock);
    checkIfPossibleTo(false, 1);
    return readByteWithoutCheck();
}

char RingBuffer::readByteWithoutCheck() {
//    LockGuard locker(lock);

    fullness--;
    size_t index = readIndex;
    readIndex = (readIndex + 1) % buffer.size();

    return buffer[index];
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
        amount = fullness;
    }
    return readAllBytes(amount);
}

size_t RingBuffer::getFullness() {
    LockGuard locker(lock);
    return fullness;
}

bool RingBuffer::isFull() {
    LockGuard locker(lock);
    return fullness == buffer.size();
}

size_t RingBuffer::size() {
    LockGuard locker(lock);
    return buffer.size();
}

bool RingBuffer::empty() {
    LockGuard locker(lock);

    return fullness==0;
}

std::string RingBuffer::show() {
    LockGuard locker(lock);
    std::string s;
    for (auto t : buffer){
        s+= std::to_string((int) t)+ " ";
    }
    s+=" fullness = "+std::to_string(fullness)+"\n";
    return s;
}

//std::string RingBuffer::show() {
//
//    return std::__cxx11::string();
//}



