cmake_minimum_required(VERSION 3.14)
project(BitsCounter)

set(CMAKE_CXX_STANDARD 17)

add_library(bits_counter_lib bits_counter.cpp)
target_include_directories(bits_counter_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(bits_counter main.cpp)
target_link_libraries(bits_counter PRIVATE bits_counter_lib)

enable_testing()
find_package(GTest REQUIRED)

add_executable(tests test.cpp)
target_link_libraries(tests PRIVATE bits_counter_lib GTest::gtest_main)

add_test(NAME BitsCounterTests COMMAND tests)