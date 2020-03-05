#ifndef RINGBUFFER_RINGBUFFER_HPP
#define RINGBUFFER_RINGBUFFER_HPP

#include <vector>
#include <mutex>


class RingBuffer {

public:
    explicit RingBuffer(size_t bufferSize = 8);

    virtual ~RingBuffer() = default;

/** @brief Add one byte or throw error.
   *
   * Add one byte if buffer has free space else throw RingBuffer_range_error.
   */
    void addByte(char ch);

/** @brief Add all bytes of throw error.
   */
    void addAllBytes(const std::vector<char> &bytes);

/** @brief Add all bytes if buffer has enough space or add free space amount bytes.
   */
    size_t addSomeBytes(const std::vector<char> &bytes);

/** @brief Read one byte if buffer has at least one byte or throw error.
    */
    char readByte();

/** @brief Read exactly 'amount' bytes if buffer has them or throw error.
    *
    */
    std::vector<char> readAllBytes(size_t amount);

/** @brief Try to read amount of bytes. If buffer has less or equal then amount, return it, else return all bytes in buffer.
    *
    */
    std::vector<char> readSomeBytes(size_t amount);

    size_t getFullness();

    bool isFull();

    size_t size();

    bool empty();

    std::string show();

private:
    using LockGuard = std::lock_guard<std::recursive_mutex>;
    std::recursive_mutex lock;
    std::vector<char> buffer;
    size_t writeIndex;
    size_t readIndex;
    size_t fullness;

    bool isPossibleTo(bool add, size_t numberOfBytes);

    void addByteWithoutCheck(char ch);

    char readByteWithoutCheck();

    void checkIfPossibleTo(bool add, size_t numberOfBytes);

    size_t freeSpace();


};


#endif //RINGBUFFER_RINGBUFFER_HPP
