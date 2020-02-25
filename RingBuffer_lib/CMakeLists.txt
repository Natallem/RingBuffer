project(RingBuffer)

set(HEADER_FILES
        headers/RingBuffer.hpp)

set(SOURCE_FILES
        sources/RingBuffer.cpp)

add_library(RingBuffer_lib STATIC ${SOURCE_FILES} ${HEADER_FILES})