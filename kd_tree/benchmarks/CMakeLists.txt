set(BENCHMARKS_TARGET kd_tree_benchmarks)

get_filename_component(BENCHMARKS_INCLUDE_PATH "." ABSOLUTE)
get_filename_component(BENCHMARKS_PATH "." ABSOLUTE)

file(GLOB_RECURSE BENCHMARKS_CXX_SOURCES ${BENCHMARKS_PATH}/*.cpp)
file(GLOB_RECURSE BENCHMARKS_HEADERS ${BENCHMARKS_PATH}/*.hpp)

add_executable(${BENCHMARKS_TARGET} ${BENCHMARKS_CXX_SOURCES} ${BENCHMARKS_HEADERS})

target_include_directories(${BENCHMARKS_TARGET} PRIVATE ${BENCHMARKS_INCLUDE_PATH})

target_link_libraries(
    ${BENCHMARKS_TARGET}
    kd_tree
    benchmark::benchmark
)
