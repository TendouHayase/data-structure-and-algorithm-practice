cmake_minimum_required(VERSION 3.20)
project(week3)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/heads/main.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE) # Windows CRT(C Runtime) 설정 충돌 방지
FetchContent_MakeAvailable(googletest)

enable_testing()
add_executable(week3 ./src/AllTests.cpp ./src/QueueTests.cpp)
target_link_libraries(week3 PRIVATE GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(week3)
