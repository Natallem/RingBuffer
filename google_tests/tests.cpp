#include <fstream>
#include <future>
#include <headers/RingBuffer.hpp>
#include <headers/RingBuffer_range_error.hpp>
#include "gtest/gtest.h"
#include "generator/random_generator.h"

const static int MAX_BYTE = 8;
using std::cout;


size_t readByteNTimes(RingBuffer &buffer, int N) {
//    std::cout << "readByteNTimes | ";
    size_t sum = 0;
    for (int i = 0; i < N; i++) {
        int number = -1;
        while (number == -1) {
            try {
//                std::cout << "try to read" << "i = " << i << " of " << N << std::endl;
                number = buffer.readByte();
//                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            } catch (RingBuffer_range_error &ignored) {
//                std::cout << "try to read i = " << i << " of " << N << " error ";
//                std::cout << ignored.what() << " \n";
//                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        }
//        std::cout << "readed " << number << std::endl;
        sum += number;
//        std::cout << "read " << number << " i = " << i << " of " << N << " sum= " << sum
//                  << std::endl;
    }
//    std::cout << "readed all " << N << std::endl;
    return sum;
}

size_t addByteNTimes(RingBuffer &buffer, int N) {
    random_generator generator = random_generator();
//    std::cout << "in addByteBuffer" << N << std::endl;
    size_t sum = 0;
    for (int i = 0; i < N; i++) {
        char number = generator.get_int_in_range(0, MAX_BYTE - 1);
        bool catchError = true;
        while (catchError) {
            catchError = false;
            try {
//                std::cout << "add " <<(int) number << " i=" << i << " of " << N << "\n";
                buffer.addByte(number);
//                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            } catch (RingBuffer_range_error &ignored) {
//                std::cout << "buffer size=" << buffer.size();
//                std::cout << " add " << (int) number << " i=" << i << " of " << N << " error = " << ignored.what()
//                          << "\n";
                catchError = true;
//                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        }
        sum += number;
//        std::cout << "successfully add " << (int) number << " i=" << i << " of " << N
//                  << " sum=" << sum << "\n";
    }
    return sum;
}

std::vector<char> getRandomVectorChar(size_t length) {
    random_generator generator = random_generator();

    std::vector<char> vector;
    for (int i = 0; i < length; i++) {
        vector.push_back(generator.get_byte());
    };
    return vector;
}

size_t addMBytesNTimes(RingBuffer &buffer, size_t M) {
    random_generator generator = random_generator();
    size_t buffer_size = buffer.size();
    size_t sum = 0;
    while (M > 0) {
//        std::vector<char> toAdd;
        int counter = 0;
        std::vector<char> toAdd = getRandomVectorChar(generator.get_int_in_range(1, std::min(M, buffer_size)));
        while (true) {
            try {
                cout << "try add\n";
                buffer.addAllBytes(toAdd);
                break;
            } catch (RingBuffer_range_error &ignored) {
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
        cout << "read vector of size " << s1 << " M=" << M << " vector is " << s << " sum=" << sum << "\n";
    }
    return sum;
}

size_t readMBytesNTimes(RingBuffer &buffer, size_t M) {
    random_generator generator = random_generator();
    size_t sum = 0;
    size_t buffer_size = buffer.size();

    while (M > 0) {
        int toRead;
        std::vector<char> readVector;
        while (true) {
            toRead = generator.get_int_in_range(1, std::min(M, buffer_size));
            try {
                cout << "try read " << toRead << "\n";
                readVector = buffer.readAllBytes(toRead);

                break;
            } catch (RingBuffer_range_error &ignored) {
            }
        }
        M -= toRead;
        std::string s;
        for (auto j : readVector) {
            sum += j;
            s += std::to_string(j) + " ";
        }
        std::string s1 = std::to_string(readVector.size());
        cout << "read vector of size " << s1 << " M=" << M << " vector is " << s << " sum=" << sum << "\n";
    }
    return sum;
}

size_t threadingAddByteBuffer(RingBuffer &buffer, int threadNumber, int amountOfAddInThreads) {
//    std::cout << "threadingAddByteBuffer | ";
    std::vector<std::future<size_t>> futures;
    size_t sum = 0;
    for (int i = 0; i < threadNumber; ++i) {
        futures.push_back(std::async(addByteNTimes, std::ref(buffer), amountOfAddInThreads));
    }
    for (int i = 0; i < threadNumber; ++i) {
        auto resultOfFunc = futures[i].get();
        sum += resultOfFunc;
    }
    return sum;
}

size_t threadingAddMByteBuffer(RingBuffer &buffer, int threadNumber, size_t N) {
//    std::cout << "threadingAddByteBuffer | ";
    random_generator generator = random_generator();
    std::vector<std::future<size_t>> futures;
    size_t sum = 0;

    for (int i = 0; i < threadNumber; ++i) {
        futures.push_back(std::async(addMBytesNTimes, std::ref(buffer), N));
        cout << "create task write i=" << i << " repeat " << N << "\n";
    }
    for (int i = 0; i < threadNumber; ++i) {
        auto resultOfFunc = futures[i].get();
        sum += resultOfFunc;
    }
    return sum;
}

size_t threadingReadByteBuffer(RingBuffer &buffer, int threadNumber, int amountOfAddInThreads) {
    std::vector<std::future<size_t>> futures;
    size_t sum = 0;
    for (int i = 0; i < threadNumber; ++i) {
        futures.push_back(std::async(readByteNTimes, std::ref(buffer), amountOfAddInThreads));
    }
    for (int i = 0; i < threadNumber; ++i) {
        auto resultOfFunc = futures[i].get();
        sum += resultOfFunc;
    }
    return sum;
}


size_t threadingReadMBytesBuffer(RingBuffer &buffer, int threadNumber, size_t amountOfAddInThreads) {
    std::vector<std::future<size_t>> futures;
    size_t sum = 0;
    for (int i = 0; i < threadNumber; ++i) {
        futures.push_back(std::async(readMBytesNTimes, std::ref(buffer), amountOfAddInThreads));
    }
    for (int i = 0; i < threadNumber; ++i) {
        auto resultOfFunc = futures[i].get();
        sum += resultOfFunc;
    }
    return sum;
}

::testing::AssertionResult
testReadWriteOneByte(size_t bufferSize, size_t threadsAmount, size_t amountOfOperationInThread) {
    RingBuffer buffer(bufferSize);
    std::future<size_t> futureRead = std::async(threadingReadByteBuffer, std::ref(buffer), threadsAmount,
                                                amountOfOperationInThread);
    std::future<size_t> futureAdd = std::async(threadingAddByteBuffer, std::ref(buffer), threadsAmount,
                                               amountOfOperationInThread);
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
    std::future<size_t> futureRead = std::async(threadingReadMBytesBuffer, std::ref(buffer), threadsAmount,
                                                amountOfOperationInThread);
    std::future<size_t> futureAdd = std::async(threadingAddMByteBuffer, std::ref(buffer), threadsAmount,
                                               amountOfOperationInThread);
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
    for (int i = buffer.getFullness(); i < buffer.size() / 2; i++) {
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
        EXPECT_THROW(fullBuffer(buffer).addByte(3), RingBuffer_range_error);
    }
}

TEST(full_buffer_add, add_all_elements) {
    for (int i = 0; i < 500; i += 20) {
        RingBuffer buffer(i);
        EXPECT_THROW(fullBuffer(buffer).addAllBytes({1, 3, 5}), RingBuffer_range_error);
    }
}

TEST(full_buffer_add, add_some_elements) {
    for (int i = 0; i < 500; i += 20) {
        RingBuffer buffer(i);
        EXPECT_EQ(fullBuffer(buffer).addSomeBytes({1, 3, 5}), 0);
        EXPECT_NO_THROW(buffer.addSomeBytes({1, 3, 5}));
    }
}

TEST(check_buffer_methods, check_buffer_methods) {
    for (int i = 1; i < 60; i += 7) {
        RingBuffer buffer(i);
        EXPECT_NO_THROW(halfFullBuffer(buffer).addByte(5));
        EXPECT_THROW(buffer.readByte(), RingBuffer_range_error);
        EXPECT_NO_THROW(buffer.readSomeBytes(10));
        EXPECT_EQ(buffer.size(), 0);
        EXPECT_EQ(buffer.getFullness(), 0);
        EXPECT_EQ(buffer.isFull(), true);
    }
}

TEST(check_buffer_methods, getFullness) {
    random_generator generator = random_generator();
    for (int i = 0; i < 60; i += 5) {
        RingBuffer buffer(i);
        EXPECT_EQ(buffer.getFullness(), 0);
        halfFullBuffer(buffer);
        EXPECT_EQ(buffer.getFullness(), i / 2);
        fullBuffer(buffer);
        EXPECT_EQ(buffer.getFullness(), i);
        emptyBuffer(buffer);
        if (i != 0) {
            int full = generator.get_int_in_range(0, i - 1);
            buffer.addAllBytes(getRandomVectorChar(full));
            EXPECT_EQ(buffer.getFullness(), full);
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

TEST(zero_buffer, zero_buffer_all_methods_Test) {
    RingBuffer buffer(0);
    EXPECT_THROW(fullBuffer(buffer).addAllBytes({1, 3, 5}), RingBuffer_range_error);
    EXPECT_THROW(buffer.readByte(), RingBuffer_range_error);
    EXPECT_NO_THROW(buffer.readSomeBytes(10));
    EXPECT_TRUE(buffer.readSomeBytes(10).size() == 0);
    EXPECT_EQ(buffer.size(), 0);
    EXPECT_EQ(buffer.getFullness(), 0);
    EXPECT_TRUE(buffer.isFull());
    EXPECT_TRUE(buffer.empty());
}

TEST(multitradingTest, addWriteOneByte) {
    for (int threads = 1; threads < 30; threads += 10) {
        for (int size = threads / 2; size < 50; size += 15) {
            for (int add = 1; add < 30; add += 10) {
                std::cout << "running size=" << size << " threads=" << threads << " add=" << add << std::endl;
                EXPECT_TRUE(testReadWriteOneByte(size, threads, add));
            }
        }
    }
}


TEST(multitradingTest, addWriteMBytes) {
    for (int threads = 1; threads < 30; threads += 10) {
        for (int size = 1; size < 50; size += 15) {
            for (int add = 1; add < 30; add += 10) {
                std::cout << "running size=" << size << " threads=" << threads << " add=" << add << std::endl;
                EXPECT_TRUE(testReadWriteMBytes(size, threads, add));
            }
        }
    }
}

int main(int argc, char *argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
