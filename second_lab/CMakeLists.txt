cmake_minimum_required(VERSION 3.14)
project(ArrayProject)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(array_lib src/array.cpp)
target_include_directories(array_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(array_main src/main.cpp)
target_link_libraries(array_main PRIVATE array_lib)

enable_testing()
find_package(GTest REQUIRED)

add_executable(tests test.cpp)
target_link_libraries(tests PRIVATE array_lib GTest::gtest_main)

add_test(NAME ArrayTests COMMAND tests)
