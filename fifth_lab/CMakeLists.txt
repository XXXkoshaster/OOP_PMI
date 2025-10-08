cmake_minimum_required(VERSION 3.14)
project(FigureProject)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(allocator_lib allocator.cpp array.cpp)

add_executable(allocator_main main.cpp)
target_link_libraries(allocator_main PRIVATE allocator_lib)

enable_testing()
find_package(GTest REQUIRED)

add_executable(tests test.cpp)
target_link_libraries(tests PRIVATE allocator_lib GTest::gtest_main)

add_test(NAME AllocatoeTests COMMAND tests)
