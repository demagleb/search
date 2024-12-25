enable_testing()

set(TESTS_TARGET kd_tree_tests)

get_filename_component(TESTS_INCLUDE_PATH "." ABSOLUTE)
get_filename_component(TESTS_PATH "." ABSOLUTE)

file(GLOB_RECURSE TESTS_CXX_SOURCES ${TESTS_PATH}/*.cpp)
file(GLOB_RECURSE TESTS_HEADERS ${TESTS_PATH}/*.hpp)

add_executable(${TESTS_TARGET} ${TESTS_CXX_SOURCES} ${TESTS_HEADERS})

target_include_directories(${TESTS_TARGET} PRIVATE ${TESTS_INCLUDE_PATH})

target_link_libraries(
    ${TESTS_TARGET}
    PRIVATE kd_tree
    PUBLIC GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(${TESTS_TARGET})
