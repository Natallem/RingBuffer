#include <fstream>
#include <future>
#include <headers/RingBuffer.hpp>
#include <headers/RingBufferError.hpp>
#include "gtest/gtest.h"
#include "generator/random_generator.h"

const static int MAX_BYTE = 8;
using std::cout;

std::vector<char> getRandomVectorChar(size_t length) {
    random_generator generator = random_generator();

    std::vector<char> vector;
    for (int i = 0; i < length; i++) {
        vector.push_back(generator.get_byte());
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
    random_generator generator = random_generator();
    size_t sum = 0;
    for (int i = 0; i < N; i++) {
        char number = generator.get_int_in_range(0, MAX_BYTE - 1);
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

size_t addMBytes(RingBuffer &buffer, size_t M) {
    random_generator generator = random_generator();
    size_t buffer_size = buffer.size();
    size_t sum = 0;
    while (M > 0) {
        int counter = 0;
        std::vector<char> toAdd = getRandomVectorChar(generator.get_int_in_range(1, std::min(M, buffer_size)));
        while (true) {
            try {
                buffer.addAllBytes(toAdd);
                break;
            } catch (RingBufferError &ignored) {
                if (counter++ > 20) {
                    toAdd = getRandomVectorChar(generator.get_int_in_range(1, std::min(M, buffer_size)));
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
    random_generator generator = random_generator();
    size_t sum = 0;
    size_t buffer_size = buffer.size();

    while (M > 0) {
        int toRead;
        std::vector<char> readVector;
        while (true) {
            toRead = generator.get_int_in_range(1, std::min(M, buffer_size));
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

size_t threadingAddByte(RingBuffer &buffer, int threadNumber, int amountOfAddInThreads,
                        const std::function<size_t(RingBuffer &, size_t)> &function) {
    std::vector<std::future<size_t>> futures;
    size_t sum = 0;
    for (int i = 0; i < threadNumber; ++i) {
        futures.push_back(std::async(function, std::ref(buffer), amountOfAddInThreads));
    }
    for (int i = 0; i < threadNumber; ++i) {
        auto resultOfFunc = futures[i].get();
        sum += resultOfFunc;
    }
    return sum;
}


size_t threadingReadByte(RingBuffer &buffer, int threadNumber, int amountOfAddInThreads,
                         const std::function<size_t(RingBuffer &, size_t)> &function) {
    std::vector<std::future<size_t>> futures;
    size_t sum = 0;
    for (int i = 0; i < threadNumber; ++i) {
        futures.push_back(std::async(/*readByteNTimes*/function, std::ref(buffer), amountOfAddInThreads));
    }
    for (int i = 0; i < threadNumber; ++i) {
        auto resultOfFunc = futures[i].get();
        sum += resultOfFunc;
    }
    return sum;
}

::testing::AssertionResult
testReadWriteByte(size_t bufferSize, size_t threadsAmount, size_t amountOfOperationInThread,
                  const std::function<size_t(RingBuffer &, size_t)> &reader,
                  const std::function<size_t(RingBuffer &, size_t)> &writer) {
    RingBuffer buffer(bufferSize);
    std::future<size_t> futureRead = std::async(threadingReadByte, std::ref(buffer), threadsAmount,
                                                amountOfOperationInThread, std::ref(reader));
    std::future<size_t> futureAdd = std::async(threadingAddByte, std::ref(buffer), threadsAmount,
                                               amountOfOperationInThread, std::ref(writer));
//    cout << "created future\n";
    auto added = futureAdd.get();
    auto read = futureRead.get();
    if (added == read) {
        return ::testing::AssertionSuccess() << "Sums are the same";
    } else {
        return ::testing::AssertionFailure() << "Sums are different: added=" << added << " read= " << read;
    }
}

::testing::AssertionResult
testReadWriteMBytes(size_t bufferSize, size_t threadsAmount, size_t amountOfOperationInThread) {
    RingBuffer buffer(bufferSize);
    std::future<size_t> futureRead = std::async(threadingReadByte, std::ref(buffer), threadsAmount,
                                                amountOfOperationInThread, readMBytes);
    std::future<size_t> futureAdd = std::async(threadingAddByte, std::ref(buffer), threadsAmount,
                                               amountOfOperationInThread, addMBytes);
    auto added = futureAdd.get();
    auto read = futureRead.get();
    if (added == read) {
        return ::testing::AssertionSuccess() << "Sums are the same";
    } else {
        return ::testing::AssertionFailure() << "Sums are different: added=" << added << " read= " << read;
    }
}

RingBuffer &fullBuffer(RingBuffer &buffer) {
    random_generator generator = random_generator();
    while (!buffer.isFull()) {
        buffer.addByte(generator.get_byte());
    }
    return buffer;
}

RingBuffer &halfFullBuffer(RingBuffer &buffer) {
    random_generator generator = random_generator();
    for (int i = buffer.getCapacity(); i < buffer.size() / 2; i++) {
        buffer.addByte(generator.get_byte());
    }
    return buffer;
}

void emptyBuffer(RingBuffer &buffer) {
    buffer.readSomeBytes(buffer.size());
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
    random_generator generator = random_generator();
    for (int i = 0; i < 60; i += 5) {
        RingBuffer buffer(i);
        EXPECT_EQ(buffer.getCapacity(), 0);
        halfFullBuffer(buffer);
        EXPECT_EQ(buffer.getCapacity(), i / 2);
        fullBuffer(buffer);
        EXPECT_EQ(buffer.getCapacity(), i);
        emptyBuffer(buffer);
        if (i != 0) {
            int full = generator.get_int_in_range(0, i - 1);
            buffer.addAllBytes(getRandomVectorChar(full));
            EXPECT_EQ(buffer.getCapacity(), full);
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
        EXPECT_EQ(buffer.size(), i);
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
        EXPECT_EQ(buffer.size(), i * 2);
        EXPECT_TRUE(buffer.empty());

        auto iter = vecInitial.begin();
        std::advance(iter, i / 3 + 1);

        buffer.addAllBytes(std::vector<char>(vecInitial.begin(), iter));
        EXPECT_THROW(buffer.resize(i / 3), RingBufferError);
        buffer.readByte();
        EXPECT_NO_THROW(buffer.resize(i / 3));
        EXPECT_THROW(buffer.addByte(0), RingBufferError);
        EXPECT_EQ(buffer.readAllBytes(buffer.getCapacity()), std::vector<char>(++vecInitial.begin(), iter));
    }
}

TEST(zero_buffer, all_methods) {
    RingBuffer buffer;
    buffer.resize(0);
    EXPECT_THROW(fullBuffer(buffer).addAllBytes({1, 3, 5}), RingBufferError);
    EXPECT_THROW(buffer.readByte(), RingBufferError);
    EXPECT_NO_THROW(buffer.readSomeBytes(10));
    EXPECT_TRUE(buffer.readSomeBytes(10).empty());
    EXPECT_EQ(buffer.size(), 0);
    EXPECT_EQ(buffer.getCapacity(), 0);
    EXPECT_TRUE(buffer.isFull());
    EXPECT_TRUE(buffer.empty());
    size_t newSize = 4;
    auto vec = getRandomVectorChar(newSize);
    auto iter = vec.begin();
    std::advance(iter, newSize / 2);
    buffer.resize(newSize);
    buffer.resize(buffer.size() / 2);
    buffer.addAllBytes(std::vector<char>(vec.begin(), iter));

    buffer.resize(buffer.size() * 2);
    EXPECT_EQ(buffer.getCapacity(), newSize / 2);
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
                std::cout << "running size=" << size << " threads=" << threads << " add=" << add << std::endl;
                EXPECT_TRUE(testReadWriteByte(size, threads, add, readByteNTimes, addByteNTimes));
            }
        }
    }
}

TEST(multitradingTest, addWriteMBytes) {
    for (int threads = 1; threads < 30; threads += 10) {
        for (int size = 1; size < 50; size += 15) {
            for (int add = 1; add < 30; add += 10) {
                std::cout << "running size=" << size << " threads=" << threads << " add=" << add << std::endl;
                EXPECT_TRUE(testReadWriteByte(size, threads, add, readMBytes, addMBytes));
            }
        }
    }
}


int main(int argc, char *argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
