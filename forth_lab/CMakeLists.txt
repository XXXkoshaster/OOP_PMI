cmake_minimum_required(VERSION 3.14)
project(FigureProject)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Header-only библиотека (шаблоны)
add_library(figure_lib INTERFACE)
target_include_directories(figure_lib INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Основная программа
add_executable(figure_main main.cpp)
target_link_libraries(figure_main PRIVATE figure_lib)

# Тесты
enable_testing()
find_package(GTest REQUIRED)

add_executable(tests test.cpp)
target_link_libraries(tests PRIVATE figure_lib GTest::gtest_main)

add_test(NAME FigureTests COMMAND tests)
