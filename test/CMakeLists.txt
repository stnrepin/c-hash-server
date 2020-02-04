cmake_minimum_required(VERSION 3.10)

file(GLOB TEST_LIB_SOURCE_FILES
    RELATIVE
    ${CMAKE_CURRENT_LIST_DIR}
    "*.c"
)

add_library(hash-server_test_lib STATIC ${TEST_LIB_SOURCE_FILES})
target_include_directories(hash-server_test_lib
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
)
target_link_libraries(hash-server_test_lib unity)

foreach(TEST_FILE ${TEST_LIB_SOURCE_FILES})
    get_filename_component(TEST_FILE_NAME ${TEST_FILE} NAME_WE)
    set(EXECUTABLE_NAME "hash-server_${TEST_FILE_NAME}")

    add_executable(${EXECUTABLE_NAME} ${TEST_FILE})
    target_link_libraries(${EXECUTABLE_NAME}
                          hash-server_test_lib hash-server-lib)

    set(TEST_NAME ${TEST_FILE_NAME})
    add_test(${TEST_NAME} ${EXECUTABLE_NAME})
endforeach()

