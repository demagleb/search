set(TARGET lsm_client)

add_executable(${TARGET} main.cpp)
target_link_libraries(${TARGET} PUBLIC
    lsm
    argparse)
