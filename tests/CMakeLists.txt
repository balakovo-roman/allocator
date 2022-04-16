cmake_minimum_required(VERSION 3.16)

project(tests)

find_package(GTest REQUIRED)
find_package(Threads REQUIRED)

enable_testing()

add_executable(tests main.cpp allocator_tests.h)

target_include_directories(tests PRIVATE ${CMAKE_SOURCE_DIR}/allocator ${GTEST_INCLUDE_DIRS})

target_link_libraries(tests PRIVATE allocator gtest gtest_main Threads::Threads)

gtest_discover_tests(tests)
