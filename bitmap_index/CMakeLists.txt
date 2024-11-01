cmake_minimum_required(VERSION 3.20)

project(bitmap_index)

include(../cmake/CheckCompiler.cmake)
include(../cmake/CompileOptions.cmake)
include(../cmake/Development.cmake)
include(../cmake/Sanitize.cmake)

add_subdirectory(bitmap_index)
add_subdirectory(contrib)
add_subdirectory(tests)
