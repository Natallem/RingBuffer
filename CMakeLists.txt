cmake_minimum_required(VERSION 3.4)

project(RingBuffer)

# add error proof code
add_executable(RingBuffer_err_test)

target_sources(	RingBuffer_err_test PUBLIC
		"main.cpp"
			  )

set (RingBuffer_lib_include_dir "${CMAKE_CURRENT_SOURCE_DIR}/RingBuffer_lib/headers")

#3. pick directories to be included for the target
target_include_directories( RingBuffer_err_test PUBLIC
						   "./"
						   ${RingBuffer_lib_include_dir}
						   )

target_compile_options(RingBuffer_err_test BEFORE PRIVATE "-std=c++17")

find_library(LIB_RING_BUFFER RingBuffer_lib PATHS "${CMAKE_CURRENT_SOURCE_DIR}/RingBuffer_lib/build")

message ("LIB_RING_BUFFER = " ${LIB_RING_BUFFER})

target_link_libraries(RingBuffer_err_test ${LIB_RING_BUFFER})


