include(FetchContent)

FetchContent_Declare(
    fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG master
)
FetchContent_MakeAvailable(fmt)

FetchContent_Declare(
    bitmap_index
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../bitmap_index
)
FetchContent_MakeAvailable(bitmap_index)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/releases/download/v1.15.2/googletest-1.15.2.tar.gz
)
FetchContent_MakeAvailable(googletest)
