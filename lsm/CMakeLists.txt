cmake_minimum_required(VERSION 3.20)

project(lsm)

include(../cmake/CheckCompiler.cmake)
include(../cmake/CompileOptions.cmake)
include(../cmake/Development.cmake)
include(../cmake/Sanitize.cmake)

add_subdirectory(lsm)
add_subdirectory(client)
add_subdirectory(contrib)
