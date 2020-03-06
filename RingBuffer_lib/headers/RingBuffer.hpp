#ifndef RINGBUFFER_RINGBUFFER_HPP
#define RINGBUFFER_RINGBUFFER_HPP

#include <vector>
#include <mutex>

class RingBuffer {

public:

    explicit RingBuffer(size_t bufferSize = 8);

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
    std::vector<char> readAllBytes(size_t amount);

/** @brief Try to read 'amount' of bytes or less(if buffer has less elements then amount).
    *
    * If buffer has less then amount elements, return all elements in buffer, else return exactly 'amount' elements.
    */
    std::vector<char> readSomeBytes(size_t amount);

    /** @brief Return current amount of elements in buffer
    */
    size_t getCapacity();

    /** @brief Return true if buffer contains maximum amount of elements else return false
   */
    bool isFull();

    /** @brief Return current size of buffer
   */
    size_t size();

    /** @brief Return true if buffer doesn't contain any element else return false;
    */
    bool empty();
    /**
    * @brief Change size of buffer
    *
    * @param[in] newSize size of buffer
    * @throws RingBufferError
    * if buffer contains more element then new size
    *
    * Change size of buffer saving order
    */
    void resize(size_t newSize);

private:
    using LockGuard = std::lock_guard<std::recursive_mutex>;
    std::recursive_mutex lock;
    std::vector<char> buffer;
    size_t writeIndex;
    size_t readIndex;
    size_t capacity;

    bool isPossibleTo(bool add, size_t numberOfBytes);

    void addByteWithoutCheck(char ch);

    char readByteWithoutCheck();

    void checkIfPossibleTo(bool add, size_t numberOfBytes);

    size_t incrIndex(size_t &index);

    size_t freeSpace();
};


#endif //RINGBUFFER_RINGBUFFER_HPP
