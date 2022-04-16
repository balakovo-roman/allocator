cmake_minimum_required(VERSION 3.16)

project(allocator_lib)

add_library(allocator_lib INTERFACE)

target_include_directories(allocator_lib INTERFACE .)
