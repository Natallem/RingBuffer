project(RingBuffer)

set(HEADER_FILES
        headers/RingBuffer.hpp headers/RingBuffer_range_error.hpp)

set(SOURCE_FILES
        sources/RingBuffer.cpp sources/RingBuffer_range_error.cpp)

add_library(RingBuffer_lib STATIC ${SOURCE_FILES} ${HEADER_FILES})