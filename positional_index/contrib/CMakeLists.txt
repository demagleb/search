include(FetchContent)

FetchContent_Declare(
    fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG master
)
FetchContent_MakeAvailable(fmt)

FetchContent_Declare(
    lsm
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../lsm
)
FetchContent_MakeAvailable(lsm)

FetchContent_Declare(
    bitmap_index
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../bitmap_index
)
FetchContent_MakeAvailable(bitmap_index)

FetchContent_Declare(
    porter2_stemmer
    GIT_REPOSITORY https://github.com/smassung/porter2_stemmer.git
    GIT_TAG        master
)
FetchContent_Populate(porter2_stemmer)
add_library(porter2-stemmer ${porter2_stemmer_SOURCE_DIR}/porter2_stemmer.cpp)
target_include_directories(porter2-stemmer PUBLIC ${porter2_stemmer_SOURCE_DIR})
target_link_libraries(porter2-stemmer PUBLIC ${LIBCXX_LIB_PATH})

FetchContent_Declare(
    CRoaring
    GIT_REPOSITORY https://github.com/RoaringBitmap/CRoaring.git
    GIT_TAG        master
)
FetchContent_MakeAvailable(CRoaring)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/releases/download/v1.15.2/googletest-1.15.2.tar.gz
)
FetchContent_MakeAvailable(googletest)
