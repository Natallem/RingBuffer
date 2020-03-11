# this string is required for CMake
cmake_minimum_required(VERSION 3.16)

project(RingBuffer)

#set(HEADER_FILES
#        headers/RingBuffer.hpp headers/RingBufferError.hpp)

#set(SOURCE_FILES
#        sources/RingBuffer.cpp sources/RingBufferError.cpp)

#add_library(RingBuffer_lib STATIC ${SOURCE_FILES} ${HEADER_FILES})
# add_library arguments are ONLY source files, not headers
# of course, iti si possible to pick headers as if they are sourses, but it's not our way )

# It would be better if we go this way:

# 1. define target
add_library(RingBuffer_lib STATIC)

#2. add source files for this target
target_sources(	RingBuffer_lib PUBLIC
		"sources/RingBuffer.cpp"
		"sources/RingBufferError.cpp"
                )
#3. pick directories to be included for the target
target_include_directories( RingBuffer_lib PUBLIC
                           "headers"
                           )
# thats's all!
