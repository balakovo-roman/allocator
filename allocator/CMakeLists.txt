cmake_minimum_required(VERSION 3.16)

project(allocator)

add_library(allocator INTERFACE)

target_include_directories(allocator INTERFACE .)
