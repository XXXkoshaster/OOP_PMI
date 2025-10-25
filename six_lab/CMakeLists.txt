cmake_minimum_required(VERSION 3.14)
project(FigureProject)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(rpg_lib src/rpg.cpp)
target_include_directories(rpg_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(rpg_main main.cpp)
target_link_libraries(rpg_main PRIVATE rpg_lib)

enable_testing()
find_package(GTest REQUIRED)

add_executable(tests test.cpp)
target_link_libraries(tests PRIVATE rpg_lib GTest::gtest_main)

add_test(NAME RpgTests COMMAND tests)
