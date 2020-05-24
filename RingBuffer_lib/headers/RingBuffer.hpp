#ifndef RINGBUFFER_RINGBUFFER_HPP
#define RINGBUFFER_RINGBUFFER_HPP

#include <vector>
#include <mutex>

class RingBuffer {

public:

    explicit RingBuffer(size_t bufferCapacity = 8);

    virtual ~RingBuffer() = default;

/**
 * @brief Add one byte.
   *
   * @param[in] ch char to add
   * @throws RingBufferError
   * if buffer hasn't enough space
   *
   * Add one byte if buffer has free space.
   */
    void addByte(char ch);

/** @brief Add all bytes if possible.
 * @throws RingBufferException if buffer hasn't enough space for all elements in [bytes] vector
   */
    void addAllBytes(const std::vector<char> &bytes);

/** @brief Add all bytes from vector if buffer has enough space or add free space amount bytes.
   */
    size_t addSomeBytes(const std::vector<char> &bytes);

/** @brief Read one byte.
 *   @throws RingBufferException if buffer hasn't got at least one element
   */
    char readByte();

/** @brief Read and return exactly 'amount' bytes if possible.
  *   @throws RingBufferException if buffer hasn't got less then 'amount' elements
    */
    std::vector<char> readAllBytes(size_t count);

/** @brief Try to read 'count' of bytes or less(if buffer has less elements then count).
    *
    * If buffer has less then count elements, return all elements in buffer, else return exactly 'count' elements.
    */
    std::vector<char> readSomeBytes(size_t count);

    /** @brief Return current number of elements in buffer
    */
    size_t getSize();

    /** @brief Return true if buffer contains maximum number of elements else return false
   */
    bool isFull();

    /** @brief Return current getCapacity of buffer
   */
    size_t getCapacity();

    /** @brief Return true if buffer doesn't contain any element else return false;
    */
    bool empty();
    /**
    * @brief Change getCapacity of buffer
    *
    * @param[in] newSize getCapacity of buffer
    * @throws RingBufferError
    * if buffer contains more element then new getCapacity
    *
    * Change getCapacity of buffer saving order
    */
    void resize(size_t newSize);

private:
    using LockGuard = std::lock_guard<std::recursive_mutex>;
    std::recursive_mutex lock;
    std::vector<char> buffer;
    size_t writeIndex;
    size_t readIndex;
    size_t size;

    bool isPossibleTo(bool add, size_t numberOfBytes);

    void addByteWithoutCheck(char ch);

    char readByteWithoutCheck();

    void checkIfPossibleTo(bool add, size_t numberOfBytes);

    std::vector<char> readBytesWithoutCheck(size_t number);

    size_t incrIndex(size_t &index);

    size_t freeSpace();
};


#endif //RINGBUFFER_RINGBUFFER_HPP
