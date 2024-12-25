include(FetchContent)

FetchContent_Declare(
    fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG master
)
FetchContent_MakeAvailable(fmt)

FetchContent_Declare(
    stdgenerator
    GIT_REPOSITORY https://github.com/lewissbaker/generator.git
    GIT_TAG        main
)
FetchContent_Populate(stdgenerator)
add_library(stdgenerator INTERFACE)
target_include_directories(stdgenerator INTERFACE ${stdgenerator_SOURCE_DIR}/include)

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
