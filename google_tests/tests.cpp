#include <fstream>
#include <future>
#include <RingBuffer.hpp>
#include <RingBufferError.hpp>
#include "gtest/gtest.h"
#include "generator/randomGenerator.h"

const static int MAX_BYTE = 8;

std::vector<char> getRandomVectorChar(size_t length) {
    randomGenerator generator = randomGenerator();

    std::vector<char> vector;
    for (int i = 0; i < length; i++) {
        vector.push_back(generator.getByte());
    };
    return vector;
}

size_t readByteNTimes(RingBuffer &buffer, size_t N) {
    size_t sum = 0;
    for (int i = 0; i < N; i++) {
        int number = -1;
        while (number == -1) {
            try {
                number = buffer.readByte();
            } catch (RingBufferError &ignored) {}
        }
        sum += number;
    }
    return sum;
}

size_t addByteNTimes(RingBuffer &buffer, size_t N) {
    randomGenerator generator = randomGenerator();
    size_t sum = 0;
    for (int i = 0; i < N; i++) {
        char number = generator.getIntInRange(0, MAX_BYTE - 1);
        bool catchError = true;
        while (catchError) {
            catchError = false;
            try {
                buffer.addByte(number);
            } catch (RingBufferError &ignored) {
                catchError = true;
            }
        }
        sum += number;
    }
    return sum;
}

/**
 * add to buffer blocks of elements by addAllBytes method
 * @param buffer
 * @param M sum number of all added elements
 * @return sum of all added elements
 */
size_t addMBytes(RingBuffer &buffer, size_t M) {
    randomGenerator generator = randomGenerator();
    size_t buffer_size = buffer.getCapacity();
    size_t sum = 0;
    while (M > 0) {
        int counter = 0;
        std::vector<char> toAdd = getRandomVectorChar(generator.getIntInRange(1, std::min(M, buffer_size)));
        while (true) {
            try {
                buffer.addAllBytes(toAdd);
                break;
            } catch (RingBufferError &ignored) {
                if (counter++ > 20) {
                    toAdd = getRandomVectorChar(generator.getIntInRange(1, std::min(M, buffer_size)));
                }
            }
        }
        M -= toAdd.size();
        std::string s;
        for (auto j : toAdd) {
            sum += j;
            s += std::to_string(j) + " ";
        }
        std::string s1 = std::to_string(toAdd.size());
    }
    return sum;
}

size_t readMBytes(RingBuffer &buffer, size_t M) {
    randomGenerator generator = randomGenerator();
    size_t sum = 0;
    size_t buffer_size = buffer.getCapacity();

    while (M > 0) {
        int toRead;
        std::vector<char> readVector;
        while (true) {
            toRead = generator.getIntInRange(1, std::min(M, buffer_size));
            try {
                readVector = buffer.readAllBytes(toRead);
                break;
            } catch (RingBufferError &ignored) {
            }
        }
        M -= toRead;
        std::string s;
        for (auto j : readVector) {
            sum += j;
            s += std::to_string(j) + " ";
        }
        std::string s1 = std::to_string(readVector.size());
    }
    return sum;
}

size_t threadingAddByte(RingBuffer &buffer, int threadNumber, int numberOfAddInThreads,
                        const std::function<size_t(RingBuffer &, size_t)> &function) {
    std::vector<std::future<size_t>> futures;
    size_t sum = 0;
    for (int i = 0; i < threadNumber; ++i) {
        futures.push_back(std::async(function, std::ref(buffer), numberOfAddInThreads));
    }
    for (int i = 0; i < threadNumber; ++i) {
        auto resultOfFunc = futures[i].get();
        sum += resultOfFunc;
    }
    return sum;
}


size_t threadingReadByte(RingBuffer &buffer, int threadNumber, int numberOfAddInThreads,
                         const std::function<size_t(RingBuffer &, size_t)> &function) {
    std::vector<std::future<size_t>> futures;
    size_t sum = 0;
    for (int i = 0; i < threadNumber; ++i) {
        futures.push_back(std::async(/*readByteNTimes*/function, std::ref(buffer), numberOfAddInThreads));
    }
    for (int i = 0; i < threadNumber; ++i) {
        auto resultOfFunc = futures[i].get();
        sum += resultOfFunc;
    }
    return sum;
}

::testing::AssertionResult
testReadWriteByte(size_t bufferSize, size_t threadsAmount, size_t numberThreadOperations,
                  const std::function<size_t(RingBuffer &, size_t)> &reader,
                  const std::function<size_t(RingBuffer &, size_t)> &writer) {
    RingBuffer buffer(bufferSize);
    std::future<size_t> futureRead = std::async(threadingReadByte, std::ref(buffer), threadsAmount,
                                                numberThreadOperations, std::ref(reader));
    std::future<size_t> futureAdd = std::async(threadingAddByte, std::ref(buffer), threadsAmount,
                                               numberThreadOperations, std::ref(writer));
    auto added = futureAdd.get();
    auto read = futureRead.get();
    if (added == read) {
        return ::testing::AssertionSuccess() << "Sums are the same";
    } else {
        return ::testing::AssertionFailure() << "Sums are different: added=" << added << " read= " << read;
    }
}

RingBuffer &fullBuffer(RingBuffer &buffer) {
    randomGenerator generator = randomGenerator();
    while (!buffer.isFull()) {
        buffer.addByte(generator.getByte());
    }
    return buffer;
}

RingBuffer &halfFullBuffer(RingBuffer &buffer) {
    randomGenerator generator = randomGenerator();
    for (int i = buffer.getSize(); i < buffer.getCapacity() / 2; i++) {
        buffer.addByte(generator.getByte());
    }
    return buffer;
}

void emptyBuffer(RingBuffer &buffer) {
    buffer.readSomeBytes(buffer.getCapacity());
}

TEST(full_buffer_add, add_one_element) {
    for (int i = 0; i < 500; i += 20) {
        RingBuffer buffer(i);
        EXPECT_THROW(fullBuffer(buffer).addByte(3), RingBufferError);
    }
}

TEST(full_buffer_add, add_all_elements) {
    for (int i = 0; i < 500; i += 20) {
        RingBuffer buffer(i);
        EXPECT_THROW(fullBuffer(buffer).addAllBytes({1, 3, 5}), RingBufferError);
    }
}

TEST(full_buffer_add, add_some_elements) {
    for (int i = 0; i < 500; i += 20) {
        RingBuffer buffer(i);
        EXPECT_EQ(fullBuffer(buffer).addSomeBytes({1, 3, 5}), 0);
        EXPECT_NO_THROW(buffer.addSomeBytes({1, 3, 5}));
    }
}

TEST(check_buffer_methods, getCapacity) {
    randomGenerator generator = randomGenerator();
    for (int i = 0; i < 60; i += 5) {
        RingBuffer buffer(i);
        EXPECT_EQ(buffer.getSize(), 0);
        halfFullBuffer(buffer);
        EXPECT_EQ(buffer.getSize(), i / 2);
        fullBuffer(buffer);
        EXPECT_EQ(buffer.getSize(), i);
        emptyBuffer(buffer);
        if (i != 0) {
            int full = generator.getIntInRange(0, i - 1);
            buffer.addAllBytes(getRandomVectorChar(full));
            EXPECT_EQ(buffer.getSize(), full);
        }
    }
}

TEST(check_buffer_methods, isFull) {
    for (int i = 0; i < 60; i += 5) {
        RingBuffer buffer(i);
        EXPECT_EQ(buffer.isFull(), i == 0);
        halfFullBuffer(buffer);
        EXPECT_EQ(buffer.isFull(), i == 0);
        fullBuffer(buffer);
        EXPECT_EQ(buffer.isFull(), true);
    }
}

TEST(check_buffer_methods, size) {
    for (int i = 0; i < 60; i += 10) {
        RingBuffer buffer(i);
        EXPECT_EQ(buffer.getCapacity(), i);
    }
}

TEST(check_buffer_methods, addReadAllBytes) {
    for (int i = 1; i < 60; i += 5) {
        RingBuffer buffer(i);
        auto vector = getRandomVectorChar(i);
        buffer.addAllBytes(vector);
        auto res = buffer.readAllBytes(i);
        EXPECT_EQ(vector, res);
        vector = getRandomVectorChar(i / 2);
        buffer.addAllBytes(vector);
        res = buffer.readAllBytes(i / 2);
        EXPECT_EQ(res, vector);
    }
}

TEST(check_buffer_methods, addReadByte) {
    for (int i = 1; i < 60; i += 5) {
        RingBuffer buffer(i);
        auto vector = getRandomVectorChar(i);
        buffer.addAllBytes(vector);
        auto res = buffer.readAllBytes(i);
        EXPECT_EQ(vector, res);
        vector = getRandomVectorChar(i / 2);
        buffer.addAllBytes(vector);
        res = buffer.readAllBytes(i / 2);
        EXPECT_EQ(res, vector);
    }
}

TEST(check_buffer_methods, addReadSomeBytes) {
    for (int i = 1; i < 60; i += 5) {
        RingBuffer buffer(i);
        size_t size = 2 * i;
        auto vector = getRandomVectorChar(size);
        buffer.addSomeBytes(vector);
        auto res = buffer.readSomeBytes(size);
        vector.resize(i);
        EXPECT_EQ(vector, res);


        size /= 4;
        vector = getRandomVectorChar(size);
        buffer.addSomeBytes(vector);
        res = buffer.readSomeBytes(size);
        EXPECT_EQ(res, vector);

        size = i;
        vector = getRandomVectorChar(size);
        buffer.addSomeBytes(vector);
        res = buffer.readSomeBytes(size);
        EXPECT_EQ(res, vector);
    }
}

TEST(check_buffer_methods, resize) {
    for (int i = 1; i < 500; i++) {
        RingBuffer buffer(i);
        auto vecInitial = getRandomVectorChar(i);
        buffer.addAllBytes(vecInitial);
        buffer.resize(i * 2);
        EXPECT_NO_THROW(buffer.addAllBytes(vecInitial));
        std::vector<char> verResult = buffer.readAllBytes(i * 2);
        vecInitial.resize(i * 2);
        for (int j = 0; j < i; j++) {
            vecInitial[i + j] = vecInitial[j];
        }
        EXPECT_EQ(vecInitial, verResult);
        EXPECT_EQ(buffer.getCapacity(), i * 2);
        EXPECT_TRUE(buffer.empty());

        auto iter = vecInitial.begin();
        std::advance(iter, i / 3 + 1);

        buffer.addAllBytes(std::vector<char>(vecInitial.begin(), iter));
        EXPECT_THROW(buffer.resize(i / 3), RingBufferError);
        buffer.readByte();
        EXPECT_NO_THROW(buffer.resize(i / 3));
        EXPECT_THROW(buffer.addByte(0), RingBufferError);
        EXPECT_EQ(buffer.readAllBytes(buffer.getSize()), std::vector<char>(++vecInitial.begin(), iter));
    }
}

TEST(zero_buffer, all_methods) {
    RingBuffer buffer;
    buffer.resize(0);
    EXPECT_THROW(fullBuffer(buffer).addAllBytes({1, 3, 5}), RingBufferError);
    EXPECT_THROW(buffer.readByte(), RingBufferError);
    EXPECT_NO_THROW(buffer.readSomeBytes(10));
    EXPECT_TRUE(buffer.readSomeBytes(10).empty());
    EXPECT_EQ(buffer.getCapacity(), 0);
    EXPECT_EQ(buffer.getSize(), 0);
    EXPECT_TRUE(buffer.isFull());
    EXPECT_TRUE(buffer.empty());
    size_t newSize = 4;
    auto vec = getRandomVectorChar(newSize);
    auto iter = vec.begin();
    std::advance(iter, newSize / 2);
    buffer.resize(newSize);
    buffer.resize(buffer.getCapacity() / 2);
    buffer.addAllBytes(std::vector<char>(vec.begin(), iter));

    buffer.resize(buffer.getCapacity() * 2);
    EXPECT_EQ(buffer.getSize(), newSize / 2);
    EXPECT_THROW(buffer.addAllBytes(getRandomVectorChar(newSize/2 + 1)), RingBufferError);
    EXPECT_NO_THROW(buffer.addAllBytes(std::vector<char>(iter, vec.end())));
    EXPECT_EQ(buffer.readAllBytes(newSize), vec);
}

TEST(check_sequance, addReadOneByte) {
    for (int i = 1; i < 500; i += 5) {
        RingBuffer buffer(i);
        for (int j = 1; j < i; j++) {
            std::vector<char> input = getRandomVectorChar(j);
            for (auto t:input) {
                buffer.addByte(t);
            }
            std::vector<char> output(j);
            for (int k = 0; k < j; k++) {
                output[k] = buffer.readByte();
            }
            EXPECT_EQ(input, output);
        }
    }
}

TEST(check_sequance, addReadAllBytes) {
    for (int i = 1; i < 500; i += 5) {
        RingBuffer buffer(i);
        for (int j = 1; j < i; j++) {
            std::vector<char> input = getRandomVectorChar(j);
            buffer.addAllBytes(input);
            std::vector<char> output = buffer.readAllBytes(j);
            EXPECT_EQ(input, output);
        }
    }
}

TEST(multitradingTest, addWriteOneByte) {
    for (int threads = 1; threads < 30; threads += 10) {
        for (int size = 1; size < 50; size += 15) {
            for (int add = 1; add < 30; add += 10) {
                std::cout << "running getCapacity=" << size << " threads=" << threads << " add=" << add << std::endl;
                EXPECT_TRUE(testReadWriteByte(size, threads, add, readByteNTimes, addByteNTimes));
            }
        }
    }
}

TEST(multitradingTest, addWriteMBytes) {
    for (int threads = 1; threads < 30; threads += 10) {
        for (int size = 1; size < 50; size += 15) {
            for (int add = 1; add < 30; add += 10) {
                std::cout << "running getCapacity=" << size << " threads=" << threads << " add=" << add << std::endl;
                EXPECT_TRUE(testReadWriteByte(size, threads, add, readMBytes, addMBytes));
            }
        }
    }
}


int main(int argc, char *argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
