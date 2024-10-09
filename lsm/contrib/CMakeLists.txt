include(FetchContent)

FetchContent_Declare(
    abseil
    GIT_REPOSITORY https://github.com/abseil/abseil-cpp.git
    GIT_TAG master
)
FetchContent_MakeAvailable(abseil)

FetchContent_Declare(
    fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG master
)
FetchContent_MakeAvailable(fmt)

FetchContent_Declare(
    rapidhash
    GIT_REPOSITORY https://github.com/Nicoshev/rapidhash.git
    GIT_TAG        master
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
)
FetchContent_Populate(rapidhash)
add_library(rapidhash INTERFACE)
target_include_directories(rapidhash INTERFACE ${rapidhash_SOURCE_DIR})

FetchContent_Declare(
    stdgenerator
    GIT_REPOSITORY https://github.com/lewissbaker/generator.git
    GIT_TAG        main
)
FetchContent_Populate(stdgenerator)
add_library(stdgenerator INTERFACE)
target_include_directories(stdgenerator INTERFACE ${stdgenerator_SOURCE_DIR}/include)


FetchContent_Declare(
    argparse
    GIT_REPOSITORY https://github.com/p-ranav/argparse.git
    GIT_TAG        master
)
FetchContent_MakeAvailable(argparse)

FetchContent_Declare(
    flat_map
    GIT_REPOSITORY https://github.com/Flast/flat_map.git
    GIT_TAG        master
)
FetchContent_Populate(flat_map)
add_library(flat_map INTERFACE)
target_include_directories(flat_map INTERFACE ${flat_map_SOURCE_DIR})

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/releases/download/v1.15.2/googletest-1.15.2.tar.gz
)
FetchContent_MakeAvailable(googletest)

FetchContent_Declare(googlebenchmark
    GIT_REPOSITORY https://github.com/google/benchmark.git
    GIT_TAG main
)
set(BENCHMARK_ENABLE_TESTING OFF)
FetchContent_MakeAvailable(googlebenchmark)
