cmake_minimum_required(VERSION 3.16)

project(main)

add_compile_options(-Wall -Wextra -Wpedantic -Werror)

add_subdirectory(allocator)
add_subdirectory(tests)

add_executable(main main.cpp)

target_include_directories(main PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/allocator)

target_link_libraries(main allocator)

set_target_properties(
        main tests PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
)