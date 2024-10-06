include(FetchContent)

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
    cli
    GIT_REPOSITORY https://github.com/daniele77/cli.git
    GIT_TAG        master
)

FetchContent_MakeAvailable(cli)
