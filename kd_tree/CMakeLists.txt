cmake_minimum_required(VERSION 3.20)

project(kd_tree)

include(../cmake/CheckCompiler.cmake)
include(../cmake/CompileOptions.cmake)
include(../cmake/Development.cmake)
include(../cmake/Sanitize.cmake)

add_subdirectory(kd_tree)
add_subdirectory(contrib)
add_subdirectory(tests)
add_subdirectory(benchmarks)
