project(RingBuffer)

set(HEADER_FILES
        headers/RingBuffer.hpp headers/RingBufferError.hpp)

set(SOURCE_FILES
        sources/RingBuffer.cpp sources/RingBufferError.cpp)

add_library(RingBuffer_lib STATIC ${SOURCE_FILES} ${HEADER_FILES})