project(Google_tests)
add_subdirectory(lib)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

add_executable(Google_Tests_run generator/random_generator.cpp tests.cpp)

target_link_libraries(Google_Tests_run RingBuffer_lib)

target_link_libraries(Google_Tests_run gtest gtest_main)
