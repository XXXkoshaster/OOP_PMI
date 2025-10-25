cmake_minimum_required(VERSION 3.14)
project(FigureProject)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(figure_lib src/figure.cpp src/array.cpp)
target_include_directories(figure_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(figure_main main.cpp)
target_link_libraries(figure_main PRIVATE figure_lib)

enable_testing()
find_package(GTest REQUIRED)

add_executable(tests test.cpp)
target_link_libraries(tests PRIVATE figure_lib GTest::gtest_main)

add_test(NAME ArrayTests COMMAND tests)
