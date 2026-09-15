cmake_minimum_required(VERSION 3.20)
project(week3)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(GTest REQUIRED)

add_executable(week3 ./src/AllTests.cpp ./src/QueueTests.cpp)
target_link_libraries(week3 PRIVATE GTest::gtest_main)

enable_testing()
include(GoogleTest)
gtest_discover_tests(week3)
