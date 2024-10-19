cmake_minimum_required(VERSION 3.20)

project(inverted_index)

include(../cmake/CheckCompiler.cmake)
include(../cmake/CompileOptions.cmake)
include(../cmake/Development.cmake)
include(../cmake/Sanitize.cmake)

add_subdirectory(inverted_index)
add_subdirectory(contrib)
add_subdirectory(tests)
