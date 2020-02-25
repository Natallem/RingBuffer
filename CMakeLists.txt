cmake_minimum_required(VERSION 3.4)
project(RingBuffer)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

set(SOURCE_FILES main.cpp)
add_executable(RingBuffer_run ${SOURCE_FILES})


include_directories(RingBuffer_lib)
add_subdirectory(RingBuffer_lib)

target_link_libraries(RingBuffer_run RingBuffer_lib)
add_subdirectory(google_tests)
add_executable(RingBuffer ${SOURCE_FILES} main.cpp)
