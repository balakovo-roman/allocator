cmake_minimum_required(VERSION 3.16)

project(allocator)

add_compile_options(-Wall -Wextra -Wpedantic -Werror)

add_subdirectory(allocator_lib)
add_subdirectory(tests)

add_executable(allocator main.cpp)

target_include_directories(allocator PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/allocator_lib)

target_link_libraries(allocator allocator_lib)

set_target_properties(
        allocator tests PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
)