cmake_minimum_required(VERSION 3.16)

project(Google_tests)

set(gtest_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../googletest/googletest")

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../RingBuffer_lib" "${CMAKE_CURRENT_SOURCE_DIR}/../RingBuffer_lib/build")

include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

find_library(LIB_GTEST gtestd PATHS "D:/students/build-googletest-Qt_5_12_3_MinGW_32_bit-Debug/lib")
message ("LIB_GTEST = " ${LIB_GTEST})

find_library(LIB_GTEST_MAIN gtest_maind PATHS "D:/students/build-googletest-Qt_5_12_3_MinGW_32_bit-Debug/lib")
message ("LIB_GTEST_MAIN = " ${LIB_GTEST_MAIN})


add_executable(Google_Tests_run generator/randomGenerator.cpp tests.cpp)

target_link_libraries(Google_Tests_run RingBuffer_lib)

target_link_libraries(Google_Tests_run ${LIB_GTEST} ${LIB_GTEST_MAIN})
